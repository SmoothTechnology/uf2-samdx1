#include "uf2.h"

uint8_t tmp_usr_page[512];

// this actually generates less code than a function
#define wait_ready()                                                                               \
    while (NVMCTRL->STATUS.bit.READY == 0)                                                        \
        ;

void flash_erase_block(uint32_t *dst) {
    wait_ready();

    // Execute "ER" Erase Row
    NVMCTRL->ADDR.reg = (uint32_t)dst;
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_EB;
    wait_ready();
}

void flash_erase_to_end(uint32_t *dst) {
    for (uint32_t i = ((uint32_t) dst); i < FLASH_SIZE; i += NVMCTRL_BLOCK_SIZE) {
        flash_erase_block((uint32_t *)i);
    }
}

void copy_words(uint32_t *dst, uint32_t *src, uint32_t n_words) {
    while (n_words--)
        *dst++ = *src++;
}

#define QUAD_WORD (4 * 4)
void flash_write_words(uint32_t *dst, uint32_t *src, uint32_t n_words) {
    // Set manual page write
    NVMCTRL->CTRLA.bit.WMODE = NVMCTRL_CTRLA_WMODE_MAN;

    // Execute "PBC" Page Buffer Clear
    wait_ready();
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_PBC;
    wait_ready();

    while (n_words > 0) {
        // We write quad words so that we can write 256 byte blocks like UF2
        // provides. Pages are 512 bytes and would require loading data back out
        // of flash for the neighboring row.
        uint32_t len = 4 < n_words ? 4 : n_words;

        wait_ready();
        if(NVMCTRL->INTFLAG.reg > 1) __BKPT();

        for (uint32_t i = 0; i < 4; i++) {
            if (i < len) {
                dst[i] = src[i];
            } else {
                dst[i] = 0xffffffff;
            }
        }

        // Trigger the quad word write.
        NVMCTRL->ADDR.reg = (uint32_t)dst;
        NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_WQW;

        // Advance to quad word
        dst += len;
        src += len;
        n_words -= len;
    }
}

void write_user_page(uint32_t *addr, uint32_t *src, uint32_t sz){
	//write this to the user page
    memcpy(tmp_usr_page, (const uint32_t *)NVMCTRL_USER, sizeof(tmp_usr_page));
    memcpy(tmp_usr_page+((uint32_t)addr-NVMCTRL_USER), src, sz);

    NVMCTRL->ADDR.reg = NVMCTRL_USER;
	NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_EP;
	while (NVMCTRL->STATUS.bit.READY == 0 || !NVMCTRL->INTFLAG.bit.DONE);

    flash_write_words((uint32_t *)NVMCTRL_USER, (uint32_t *)tmp_usr_page, sizeof(tmp_usr_page)/4);
}

uint32_t calculate_crc(uint32_t *addr, uint32_t sz){
	//enable writes to DSU
	PAC->WRCTRL.reg = PAC_WRCTRL_PERID(32+1) | PAC_WRCTRL_KEY_CLR;

	DSU->DATA.reg = 0xFFFFFFFF;
	DSU->ADDR.bit.ADDR = (uint32_t)addr >> 2;
	DSU->LENGTH.reg = sz;
	DSU->STATUSA.reg = 0x1F;

	DSU->CTRL.bit.CRC = 1; //start CRC

	while(!DSU->STATUSA.bit.DONE);
	//TODO: should we check for bus read errors?

	return DSU->DATA.reg;
}

// On the SAMD51 we can only erase 4KiB blocks of 512 byte pages. To reduce wear
// and increase flash speed we only want to erase a block at most once per
// flash. Each 256 byte row from the UF2 comes in an unknown order. So, we wait
// to erase until we see a row that varies with current memory. Before erasing,
// we cache the rows that were the same up to this point, perform the erase and
// flush the previously seen rows. Every row after will get written without
// another erase.

bool block_erased[FLASH_SIZE / NVMCTRL_BLOCK_SIZE];
bool row_same[FLASH_SIZE / NVMCTRL_BLOCK_SIZE][NVMCTRL_BLOCK_SIZE / FLASH_ROW_SIZE];

// Skip writing blocks that are identical to the existing block.
// only disable for debugging/timing
#define QUICK_FLASH 1

void flash_write_row(uint32_t *dst, uint32_t *src) {
    const uint32_t FLASH_ROW_SIZE_WORDS = FLASH_ROW_SIZE / 4;

    // The cache in Rev A isn't reliable when reading and writing to the NVM.
    NVMCTRL->CTRLA.bit.CACHEDIS0 = true;
    NVMCTRL->CTRLA.bit.CACHEDIS1 = true;

    uint32_t block = ((uint32_t) dst) / NVMCTRL_BLOCK_SIZE;
    uint8_t row = (((uint32_t) dst) % NVMCTRL_BLOCK_SIZE) / FLASH_ROW_SIZE;
#if QUICK_FLASH
    bool src_different = false;
    for (uint32_t i = 0; i < FLASH_ROW_SIZE_WORDS; ++i) {
        if (src[i] != dst[i]) {
            src_different = true;
            break;
        }
    }

    // Row is the same, quit early but keep track in case we need to erase its
    // block. This is ok after an erase because the destination will be all 1s.
    if (!src_different) {
        row_same[block][row] = true;
        return;
    }
#endif

    if (!block_erased[block]) {
        uint8_t rows_per_block = NVMCTRL_BLOCK_SIZE / FLASH_ROW_SIZE;
        uint32_t* block_address = (uint32_t *) (block * NVMCTRL_BLOCK_SIZE);

        bool some_rows_same = false;
        for (uint8_t i = 0; i < rows_per_block; i++) {
            some_rows_same = some_rows_same || row_same[block][i];
        }
        uint32_t row_cache[rows_per_block][FLASH_ROW_SIZE_WORDS];
        if (some_rows_same) {
            for (uint8_t i = 0; i < rows_per_block; i++) {
                if(row_same[block][i]) {
                    memcpy(row_cache[i], block_address + i * FLASH_ROW_SIZE_WORDS, FLASH_ROW_SIZE);
                }
            }
        }
        flash_erase_block(dst);
        block_erased[block] = true;
        if (some_rows_same) {
            for (uint8_t i = 0; i < rows_per_block; i++) {
                if(row_same[block][i]) {
                    // dst is a uint32_t pointer so we add the number of words,
                    // not bytes.
                    flash_write_words(block_address + i * FLASH_ROW_SIZE_WORDS, row_cache[i], FLASH_ROW_SIZE_WORDS);
                }
            }
        }
    }

    flash_write_words(dst, src, FLASH_ROW_SIZE_WORDS);

    // Don't return until we're done writing in case something after us causes
    // a reset.
    wait_ready();
}
