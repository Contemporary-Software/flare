/*
 * Copyright 2024 Contemporary Software
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

#if !defined(_BOOTLOADER_FLASH_H_)
#define _BOOTLOADER_FLASH_H_

#include <stdbool.h>
#include <stdlib.h>

//#include "drivers/flash/flash.h"
#include "flare-io.h"

/*
 * Driver configuration.
 */
#define FLASH_4BYTE_ADDRESSING  1
#define FLASH_FAST_READ         1

/*
 * Static allocation of needed buffers.
 */
#define DDR_RAM_BASE         (0x00100000UL)
#define FLASH_WORKSPACE_BASE DDR_RAM_BASE
#define FLASH_WORKSPACE_STEP (0x00100000UL)

#define FLASH_WORKSPACE_ALLOC(_slot) \
    ((void*) (FLASH_WORKSPACE_BASE + ((_slot) * FLASH_WORKSPACE_STEP)))

/*
 * Workspace slots.
 */
#define FLASH_WS_CFI           (0)
#define FLASH_WS_FLASH_REGIONS (1)
#define FLASH_WS_ERASE_BUFFER  (2)
#define FLASH_WS_USER_BUFFER   (3)

/*
 * The flash device constants.
 */
#if FLARE_VERSAL
  #define FLASH_PAGE_SIZE 0x1000
  #define qspi_base 0xf1030000
#elif FLARE_ZYNQMP
  #define FLASH_PAGE_SIZE 0x1000
  #define qspi_base 0xff0f0000
#else
  #define FLASH_PAGE_SIZE         (512)
  #define qspi_base 0xe000d000
#endif /* FLARE_ZYNQMP */

#define FLASH_COMMAND_OFFSET    (0) /* FLASH instruction */
#define FLASH_ADDRESS_1_OFFSET  (1) /* Bits 31-24 of the address */
#define FLASH_ADDRESS_2_OFFSET  (2) /* Bits 23-16 of the address */
#define FLASH_ADDRESS_3_OFFSET  (3) /* Bits 16-8 of the address */
#if FLASH_4BYTE_ADDRESSING
 #define FLASH_ADDRESS_4_OFFSET (4) /* Bits 8-0 of the address */
 #define FLASH_DATA_OFFSET      (5) /* Start of Data for Read/Write */
#else
 #define FLASH_DATA_OFFSET      (4) /* Start of Data for Read/Write */
#endif
#define FLASH_SPI_MAX_PADDING   (4) /* Maximum amount of padding. */
#define FLASH_BUFFER_SIZE       (FLASH_DATA_OFFSET + \
                                 FLASH_PAGE_SIZE + \
                                 FLASH_SPI_MAX_PADDING)

#define FLASH_TX_TRANS 0
#define FLASH_RX_TRANS 1

#define FLASH_COMM_METHOD_PARALLEL      0
#define FLASH_COMM_METHOD_SINGLE_TOP    1
#define FLASH_COMM_METHOD_SINGLE_BOTTOM 2

/*
 * A region is a collection of sections in the flash device.
 */
typedef struct
{
    size_t count;  /**< The number of sections in the region. */
    size_t size;   /**< The size of the sections. */
} flash_region;

/*
 * Flash driver errors.
 */
typedef enum
{
    FLASH_NO_ERROR = 0,
    FLASH_NOT_OPEN,
    FLASH_ALREADY_OPEN,
    FLASH_NO_MEMORY,
    FLASH_4BYTE_ADDR_NOT_SUPPORTED,
    FLASH_BUFFER_OVERFLOW,
    FLASH_BUFFER_UNDERFLOW,
    FLASH_BAD_ADDRESS,
    FLASH_NOT_BLANK,
    FLASH_ERASE_FAILURE,
    FLASH_READ_ONLY,
    FLASH_WRITE_LATCH_CLEAR_FAIL,
    FLASH_WRITE_LOCK_FAIL,
    FLASH_WRITE_ACROSS_SECTION,
    FLASH_WRITE_ERASE_CMD_FAIL,
    FLASH_LOCK_FAIL,
    FLASH_INVALID_DEVICE
} flash_error;

void flash_Init(void);

flash_error flash_Open (void);
flash_error flash_Close (void);

flash_error flash_Read(uint32_t address, void* buffer, size_t length);
flash_error flash_Write(uint32_t address, const void* buffer, size_t length);

flash_error flash_Blank(uint32_t address, size_t length);
flash_error flash_EraseSector(uint32_t address);
flash_error flash_WriteSector(uint32_t address, const void* buffer, size_t length);

flash_error flash_EraseDevice(void);

flash_error flash_ReadId(uint32_t* manufactureCode,
                         uint32_t* memIfaceType,
                         uint32_t* density);
flash_error flash_PrintCFI(void);

flash_error flash_GetRegions(flash_region** regions, size_t* size);

size_t flash_DeviceSize(void);
size_t flash_DeviceSectorEraseSize(void);

void flash_TraceControl(bool state);

const char* flash_ErrorText(flash_error fe);
/*
 * A transfer buffer has a buffer and length.
 */
typedef struct
{
    size_t    length;
    size_t    padding;
    size_t    in;
    size_t    out;
    uint8_t   buffer[FLASH_BUFFER_SIZE];
    uint32_t  trans_dir;
    uint32_t  command_len;
    uint32_t  comm_method;
} flash_transfer_buffer;

/*
 * A write buffer.
 */
static flash_transfer_buffer flash_buf_;
#define flash_buf &flash_buf_
/*
 * Special handler function used by the tester.
 */
typedef void (*flash_WaitHandler)(void* user);

void flash_RegisterWaitHandler(flash_WaitHandler handler, void* user);

void flash_TransferTrace(const char* message, const flash_transfer_buffer* transfer);

void qspi_reg_write(uint32_t reg, uint32_t value);

uint32_t qspi_reg_read(uint32_t reg);

flash_error flash_TransferBuffer_Skip(flash_transfer_buffer* transfer, const size_t size);

#endif
