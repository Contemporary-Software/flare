/*
 * Copyright 2025 Contemporary Software
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <stdbool.h>

#include <io/board-io.h>
#include <timer/board-timer.h>
#include <timer/sleep.h>
#include <sdhci/sdhci.h>
#include <sdhci/sdhci_hw.h>

#define SDHCI_DEBUG_NONE 0
#define SDHCI_DEBUG_ENA  1
#define SDHCI_DEBUG_VERB 2

static const int sdhci_debug = SDHCI_DEBUG_NONE;

static bool initialised = false;
static uint8_t card_version;
static uint32_t card_id[4];
static uint32_t card_csd[4];
static uint16_t card_rca;

#define SDHCI_DEBUG(...)           \
    do {                          \
        if (sdhci_debug) {         \
            printf(__VA_ARGS__);  \
        }                         \
    } while (0)

#define SDHCI_REG_DEBUG(...)                   \
    do {                                      \
        if (sdhci_debug >= SDHCI_DEBUG_VERB) {  \
            printf(__VA_ARGS__);              \
        }                                     \
    } while (0)

uint32_t sdhci_reg_read(uint32_t offset) {
    uint32_t val = board_reg_read(sdhci_address() + offset);
    SDHCI_REG_DEBUG("sdhci: read32: 0x%08x = %08x\n",
        sdhci_address() + offset, val);
    return val;
}

uint16_t sdhci_reg_read_16(uint32_t offset) {
    uint16_t val = board_reg_read_16(sdhci_address() + offset);
    SDHCI_REG_DEBUG("sdhci: read16: 0x%08x = %04x\n",
        sdhci_address() + offset, val);
    return val;
}

uint8_t sdhci_reg_read_8(uint32_t offset) {
    uint8_t val = board_reg_read_8(sdhci_address() + offset);
    SDHCI_REG_DEBUG("sdhci: read8: 0x%08x = %02x\n",
        sdhci_address() + offset, val);
    return val;
}

void sdhci_reg_write(uint32_t offset, uint32_t val) {
    SDHCI_REG_DEBUG("sdhci: write32: 0x%08x = %08x\n", sdhci_address() + offset, val);
    board_reg_write(sdhci_address() + offset, val);
}

void sdhci_reg_write_16(uint32_t offset, uint16_t val) {
    SDHCI_REG_DEBUG("sdhci: write16: 0x%08x = %04x\n", sdhci_address() + offset, val);
    board_reg_write_16(sdhci_address() + offset, val);
}

void sdhci_reg_write_8(uint32_t offset, uint8_t val) {
    SDHCI_REG_DEBUG("sdhci: write8: 0x%08x = %02x\n", sdhci_address() + offset, val);
    board_reg_write_8(sdhci_address() + offset, val);
}

static sdhci_error initialise();

static inline void check_initialised() {
    if (!initialised) {
        initialise();
    }
}

static sdhci_error set_clock(bool slow) {
    SDHCI_DEBUG("sdhci: set_clock(slow = %c)\n", slow ? 'T' : 'F');
    uint16_t clk_val;

    sdhci_reg_write_16(SDHCI_CLOCK_CONTROL, 0);

    clk_val = sdhci_reg_read_16(SDHCI_CLOCK_CONTROL);
    clk_val |= SDHCI_CLOCK_INT_EN;
    if (initialise) {
        /* Set clock to 50Mhz / 128 = 400Khz */
        uint16_t divisor_power = 7;
        clk_val |= (1 << (divisor_power - 1)) << SDHCI_DIVIDER_SHIFT;
    } else {
        /* Set clock to 50Mhz / 2 = 25Mhz */
        uint16_t divisor_power = 1;
        clk_val |= (1 << (divisor_power - 1)) << SDHCI_DIVIDER_SHIFT;
    }
    sdhci_reg_write_16(SDHCI_CLOCK_CONTROL, clk_val);

    /* Poll for clock stabilisation complete */
    uint32_t poll_mask = SDHCI_CLOCK_INT_STABLE;
    uint32_t timeout = 150000;
    clk_val = sdhci_reg_read_16(SDHCI_CLOCK_CONTROL) & poll_mask;
    while (timeout != 0) {
        clk_val = sdhci_reg_read_16(SDHCI_CLOCK_CONTROL) & poll_mask;
        if (clk_val != 0) {
            break;
        }
        timeout--;
        usleep(1);
    }
    if (timeout == 0) {
        return SDHCI_CLK_ERROR;
    }

    clk_val = sdhci_reg_read_16(SDHCI_CLOCK_CONTROL);
    clk_val |= SDHCI_CLOCK_CARD_EN;
    sdhci_reg_write_16(SDHCI_CLOCK_CONTROL, clk_val);

    usleep(10000);
    return SDHCI_NO_ERROR;
}

static sdhci_error reset_config() {
    SDHCI_DEBUG("sdhci: reset_config()\n");
    disable_bus_power();

    sdhci_reg_write_8(SDHCI_SOFTWARE_RESET, SDHCI_RESET_ALL);

    uint32_t timeout = 100000;
    uint8_t cval;
    while (timeout != 0) {
        cval = sdhci_reg_read_8(SDHCI_SOFTWARE_RESET) & SDHCI_RESET_ALL;
        if (cval == 0) {
            break;
        }
        timeout--;
        usleep(1);
    }

    if (timeout == 0) {
        SDHCI_DEBUG("sdhci: reset timed out\n");
        return SDHCI_RESET_FAILED;
    }

    enable_bus_power();
    return SDHCI_NO_ERROR;
}

static void config_power() {
    SDHCI_DEBUG("sdhci: config_power()\n");
    uint8_t pl = 0;
    uint32_t caps = sdhci_reg_read(SDHCI_CAPABILITIES);

    if (caps & SDHCI_CAN_VDD_330) {
        pl = SDHCI_POWER_330;
    } else if (caps & SDHCI_CAN_VDD_300) {
        pl = SDHCI_POWER_300;
    } else if (caps & SDHCI_CAN_VDD_180) {
        pl = SDHCI_POWER_180;
    }

    sdhci_reg_write_8(SDHCI_POWER_CONTROL, pl | SDHCI_POWER_ON);
}

static inline bool check_idle() {
    uint32_t status = sdhci_reg_read(SDHCI_PRESENT_STATE);
    return !(status & SDHCI_CMD_INHIBIT);
}

static uint32_t flag_generator(uint32_t cmd) {
    uint32_t flags = 0;

    if (cmd != CMD16) {
        flags |= SDHCI_TRNS_READ;
    }

    if (cmd == CMD0) {
        /* R None */
        flags |= SDHCI_CMD_RESP_NONE << 16;
    } else if (cmd == CMD2 || cmd == CMD9) {
        /* R2 */
        flags |= SDHCI_CMD_RESP_LONG << 16;
        flags |= SDHCI_CMD_CRC << 16;
    } else if (cmd == ACMD41) {
        /* R3 */
        flags |= SDHCI_CMD_RESP_SHORT << 16;
    } else if (cmd == CMD3 || cmd == CMD7) {
        /* R6 and R1b */
        flags |= SDHCI_CMD_RESP_SHORT_BUSY << 16;
        flags |= SDHCI_CMD_CRC << 16;
        flags |= SDHCI_CMD_INDEX << 16;
    } else {
        /* R1 */
        flags |= SDHCI_CMD_RESP_SHORT << 16;
        flags |= SDHCI_CMD_CRC << 16;
        flags |= SDHCI_CMD_INDEX << 16;
    }

    if (cmd == CMD17 || cmd == CMD18) {
        flags |= SDHCI_CMD_DATA << 16;
    }

    if (cmd == CMD18) {
        flags |= SDHCI_TRNS_BLK_CNT_EN;
        flags |= SDHCI_TRNS_MULTI;
        flags |= SDHCI_TRNS_ACMD12;
    }

    return flags;
}

static sdhci_error cmd_transfer(
    uint32_t cmd, uint32_t arg, uint16_t blk_cnt, uint32_t* res) {
    SDHCI_DEBUG(
        "sdhci: cmd_transfer(cmd=%c%d, arg=0x%08x, blk_cnt=%d, res=0x%08x)\n",
        cmd&0x80 ? 'A' : 'C', cmd&0x3F , arg, blk_cnt, res);
    if (!check_idle()) {
        SDHCI_DEBUG("sdhci: cmd_transfer: SDHCI_BUSY\n");
        return SDHCI_BUSY;
    }

    uint32_t flags = flag_generator(cmd);

    sdhci_reg_write_16(SDHCI_BLOCK_COUNT, blk_cnt);
    sdhci_reg_write_8(SDHCI_TIMEOUT_CONTROL, 0xE);
    sdhci_reg_write(SDHCI_ARGUMENT, arg);

    sdhci_reg_write(SDHCI_INT_STATUS,
        SDHCI_INT_NORMAL_MASK | SDHCI_INT_ERROR_MASK);

    cmd &= 0x3F;
    sdhci_reg_write(SDHCI_TRANSFER_MODE, (cmd << 24) | flags);

    /* Poll for transfer complete */
    uint32_t poll_mask = SDHCI_INT_ERROR | SDHCI_INT_RESPONSE;
    uint64_t end_time;
    uint64_t curr_time;
    board_timer_get(&curr_time);
    end_time = curr_time + 100000;
    uint32_t cval;
    while (curr_time < end_time) {
        check_idle();
        cval = sdhci_reg_read(SDHCI_INT_STATUS) & poll_mask;
        if (cval != 0) {
            break;
        }
        board_timer_get(&curr_time);
    }
    if (cval & SDHCI_INT_ERROR || (curr_time >= end_time)) {
        /* Write to clear and return error */
        sdhci_reg_write(SDHCI_INT_STATUS, SDHCI_INT_ERROR_MASK);
        SDHCI_DEBUG("sdhci: cmd_transfer: SDHCI_TRANSFER_FAILED\n");
        return SDHCI_TRANSFER_FAILED;
    }

    /* Write to clear */
    sdhci_reg_write_16(SDHCI_INT_STATUS, SDHCI_INT_RESPONSE);

    if (res != NULL) {
        *res = sdhci_reg_read(SDHCI_RESPONSE0);
        SDHCI_DEBUG("sdhci: cmd_transfer: SDHCI_NO_ERROR: 0x%08x\n", *res);
    } else {
        SDHCI_DEBUG("sdhci: cmd_transfer: SDHCI_NO_ERROR\n");
    }

    return SDHCI_NO_ERROR;
}

static uint32_t read_fifo(char* buffer) {
    SDHCI_DEBUG("sdhci: read_fifo(buffer = 0x%08x)\n", buffer);
    uint32_t* buff = (uint32_t*)buffer;
    sdhci_reg_write(SDHCI_INT_STATUS, SDHCI_INT_DATA_AVAIL);
    uint32_t total_reads = SDHCI_BLK_SIZE / sizeof(uint32_t);
    for (uint32_t i = 0; i < total_reads; i++) {
        buff[i] = sdhci_reg_read(SDHCI_BUFFER);
        usleep(1);
    };
    return SDHCI_BLK_SIZE;
}

static sdhci_error read_data_transfer(char* buffer) {
    SDHCI_DEBUG("sdhci: read_data_transfer(buffer = 0x%08x)\n", buffer);
    uint32_t status;
    uint64_t end_time;
    uint64_t curr_time;
    board_timer_get(&curr_time);
    end_time = curr_time + 5000000;
    while (curr_time < end_time) {
        status = sdhci_reg_read(SDHCI_INT_STATUS);
        if ((status & SDHCI_INT_DATA_AVAIL) != 0) {
            buffer += read_fifo(buffer);
        }
        if ((status & (SDHCI_INT_ERROR | SDHCI_INT_DATA_END)) != 0) {
            break;
        }
        board_timer_get(&curr_time);
    }
    if ((status & SDHCI_INT_ERROR) || (curr_time >= end_time)) {
        /* Write to clear and return error */
        sdhci_reg_write(SDHCI_INT_STATUS, SDHCI_INT_ERROR_MASK);
        SDHCI_DEBUG("sdhci: read_data_transfer: SDHCI_TRANSFER_FAILED\n");
        return SDHCI_TRANSFER_FAILED;
    }
    SDHCI_DEBUG("sdhci: read_data_transfer: SDHCI_NO_ERROR\n");
    return SDHCI_NO_ERROR;
}

static sdhci_error initialise() {
    SDHCI_DEBUG("sdhci: initialise()\n");
    sdhci_error err;
    uint32_t res;

    err = reset_config();
    if (err != SDHCI_NO_ERROR) {
        return err;
    }

    config_power();

    bool card_present =
        sdhci_reg_read(SDHCI_PRESENT_STATE) & SDHCI_CARD_PRESENT;
    if (!card_present) {
        return SDHCI_CARD_NOT_PRESENT;
    }

    /* Set clock to 400Khz */
    err = set_clock(true);
    if (err != SDHCI_NO_ERROR) {
        return err;
    }

    /* Enable status bits */
    sdhci_reg_write(SDHCI_INT_ENABLE,
        SDHCI_INT_NORMAL_MASK | SDHCI_INT_ERROR_MASK);

    /* SW Reset the SD Card */
    err = cmd_transfer(CMD0, 0, 0, NULL);
    if (err != SDHCI_NO_ERROR) {
        return err;
    }

    /* Get card interface */
    err = cmd_transfer(CMD8, 0x1AA, 0, &res);
    if (err != SDHCI_NO_ERROR && err != SDHCI_RESPONSE_TIMEOUT) {
        return err;
    }
    if (res != 0x1AA) {
        card_version = 1;
    } else {
        card_version = 2;
    }

    /* Start initialisation process. ACMD41 */
    res = 0;
    while (!(res & SDHCI_OCR_READY)) {
        err = cmd_transfer(CMD55, 0, 0, NULL);
        if (err != SDHCI_NO_ERROR) {
            return err;
        }

        uint32_t arg = SDHCI_ACMD41_HCS | SDHCI_ACMD41_3V3 | (0x1FFU << 15U);
        err = cmd_transfer(ACMD41, arg, 0, &res);
        if (err != SDHCI_NO_ERROR) {
            return err;
        }
    }

    /* Get card id */
    err = cmd_transfer(CMD2, 0, 0, &card_id[0]);
    if (err != SDHCI_NO_ERROR && err != SDHCI_RESPONSE_TIMEOUT) {
        return err;
    }
    card_id[1] = sdhci_reg_read(SDHCI_RESPONSE1);
    card_id[2] = sdhci_reg_read(SDHCI_RESPONSE2);
    card_id[3] = sdhci_reg_read(SDHCI_RESPONSE3);

    /* Get card address */
    err = cmd_transfer(CMD3, 0, 0, &res);
    if (err != SDHCI_NO_ERROR && err != SDHCI_RESPONSE_TIMEOUT) {
        return err;
    }
    card_rca = (res >> 16) & 0xFFFF;

    /*
     * Get card CSD.
     * CSD is shifted 8 bits to the left because of SD host controller
     */
    err = cmd_transfer(CMD9, (card_rca << 16), 0, &card_csd[0]);
    if (err != SDHCI_NO_ERROR && err != SDHCI_RESPONSE_TIMEOUT) {
        return err;
    }
    card_csd[1] = sdhci_reg_read(SDHCI_RESPONSE1);
    card_csd[2] = sdhci_reg_read(SDHCI_RESPONSE2);
    card_csd[3] = sdhci_reg_read(SDHCI_RESPONSE3);

    uint32_t csd_struct = ((card_csd[3] & CSD_STRUCT_MASK) >> CSD_STRUCT_SHIFT);
    if (csd_struct == 0) {
        uint32_t blk_len = (card_csd[2] & READ_BLK_LEN_MASK) >> 8;
        uint32_t mult_shift = ((card_csd[1] & C_SIZE_MULT_MASK) >> 7) + 2;
        uint32_t mult = 1 << mult_shift;
        uint32_t size_mb = (card_csd[1] & C_SIZE_LOWER_MASK) >> 22;
        size_mb |= (card_csd[2] & C_SIZE_UPPER_MASK) << 10;
        size_mb = (size_mb + 1) * mult;
        size_mb = size_mb * blk_len;
        size_mb = size_mb / (1024 * 1024);
        printf("SDSD card found (%08x MiB)\n", size_mb);
    } else if (csd_struct == 1) {
        uint32_t size_gb = ((card_csd[1] & CSD_V2_C_SIZE_MASK) >> 8) + 1;
        size_gb = size_gb / (2 * 1024);
        printf("SDHC/SDXC card found (%d GiB)\n", size_gb);
    } else {
        return SDHCI_CARD_NOT_SUPPORTED;
    }

    /* Set clock speed to 25Mhz */
    set_clock(false);

    /* Moving card to transfer mode */
    err = cmd_transfer(CMD7, (card_rca << 16), 0, &res);
    if (err != SDHCI_NO_ERROR && err != SDHCI_RESPONSE_TIMEOUT) {
        return err;
    }

    /* Set block size to 512 */
    sdhci_reg_write_16(SDHCI_BLOCK_SIZE_REG, SDHCI_BLK_SIZE);
    err = cmd_transfer(CMD16, SDHCI_BLK_SIZE, 0, &res);
    if (err != SDHCI_NO_ERROR) {
        return err;
    }

    initialised = true;
    SDHCI_DEBUG("sdhci: initialise: success\n");
    return SDHCI_NO_ERROR;
}

sdhci_error sdhci_open(void) {
    return initialise();
}

sdhci_error sdhci_close(void) {
    initialised = false;
}

sdhci_error sdhci_read(uint32_t sector, uint32_t count, char* buffer) {
    SDHCI_DEBUG("sdhci: read(sector = 0x%08x, count = %d, buffer = 0x%08x)\n",
        sector, count, buffer);
    sdhci_error err;
    uint32_t res;
    if (count == 0) {
        return SDHCI_NO_ERROR;
    } else if (count == 1) {
        err = cmd_transfer(CMD17, sector, 0, &res);
        if (err != SDHCI_NO_ERROR) {
            return err;
        }
    } else {
        err = cmd_transfer(CMD18, sector, count, &res);
        if (err != SDHCI_NO_ERROR) {
            return err;
        }
    }
    err = read_data_transfer(buffer);
    if (err != SDHCI_NO_ERROR) {
        return err;
    }
    return SDHCI_NO_ERROR;
}
