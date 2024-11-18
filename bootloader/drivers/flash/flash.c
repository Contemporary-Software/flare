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

#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "flash.h"
#include "board-io.h"

#if FLARE_VERSAL
#include "versal-flash.h"
#elif FLARE_ZYNQ7000
#include "zynq7000-flash.h"
#else
#include "zynqmp-flash.h"
#endif /* ARCH */

#include "flare-io.h"

/*
 * Flash commands.
 */

#define FLASH_COMMAND_SIZE      1
#if FLASH_4BYTE_ADDRESSING
 #define FLASH_ADDRESS_SIZE     4
 #define FLASH_WRITE_CMD        0x12
 #if FLASH_FAST_READ
  #define FLASH_READ_CMD        0x0c
 #else
  #define FLASH_READ_CMD        0x13
 #endif
 #define FLASH_SEC_ERASE_CMD    0xDC
#else
 #define FLASH_ADDRESS_SIZE     3
 #define FLASH_WRITE_CMD        0x02
 #if FLASH_FAST_READ
  #define FLASH_READ_CMD        0x0b
 #else
  #define FLASH_READ_CMD        0x03
 #endif
 #define FLASH_SEC_ERASE_CMD    0xD8
#endif

#define FLASH_READ_CONFIG_CMD       0x35
#define FLASH_WRITE_STATUS_CMD      0x01
#define FLASH_WRITE_DISABLE_CMD     0x04
#define FLASH_READ_STATUS_CMD       0x05
#define FLASH_WRITE_DISABLE_CMD     0x04
#define FLASH_WRITE_ENABLE_CMD      0x06
#define FLASH_BULK_ERASE_CMD        0xC7
#define FLASH_READ_ID               0x9F

#ifdef FLARE_ZYNQMP
  #define FLASH_READ_STATUS_FLAG_CMD  0x70
#else
  #define FLASH_READ_STATUS_FLAG_CMD  0x05
#endif


/*
 * The flash page size.
 */
static uint64_t      flash_size;
static uint32_t      flash_read_dummies;
static uint32_t      flash_erase_sector_size;
static uint32_t      flash_page_size;
static size_t        flash_num_regions;
static flash_region* flash_regions;
static uint8_t*      flash_erase_buffer;
static size_t        flash_erase_buffer_size;

/*
 * Initialised state.
 */
static bool initialised = false;

static size_t   cfi_length;
static uint8_t* cfi_data;

/*
 * Wait handler.
 */
static flash_WaitHandler wait_Handler;
static void*             wait_Handler_User;

/*
 * Debug tracing.
 */
#define FLASH_TRACE_TRANSFER 0

#if FLASH_TRACE_TRANSFER
static bool flash_trace = 1;
#endif

static flash_error flash_ReadCFI(void);

static void
flash_TransferTraceHeader(const char*                  message,
                          const flash_transfer_buffer* transfer)
{
#if FLASH_TRACE_TRANSFER
    if (flash_trace)
        printf(" %s: length=%ld in=%ld out=%ld padding=%ld\n",
               message, transfer->length, transfer->in,
               transfer->out, transfer->padding);
#endif
}

void
flash_TransferTrace(const char*                  message,
                    const flash_transfer_buffer* transfer)
{
#if FLASH_TRACE_TRANSFER
    if (flash_trace)
    {
        size_t c;
        flash_TransferTraceHeader(message, transfer);
        for (c = 0; c < transfer->length; ++c)
        {
            if ((c % 16) == 0)
            {
                if (c)
                    printf("\n");
                printf("  %04lx ", c);
            }
#ifdef FLARE_ZYNQMP
            printf("%02x", transfer->buffer[c]);
#else
            printf("%02x", transfer->buffer[c + transfer->padding]);
#endif
            if ((c % 16) == 7)
                printf("-");
            else
                printf(" ");
        }
        printf("\n");
    }
#endif
}

void qspi_reg_write(uint32_t reg, uint32_t value)
{
    board_reg_write (qspi_base + reg, value);
}

uint32_t qspi_reg_read(uint32_t reg)
{
    return board_reg_read (qspi_base + reg);
}

static inline uint8_t
flash_Get8(const uint8_t* data)
{
    return *data;
}

static inline uint16_t
flash_Get16(const uint8_t* data)
{
    return (((uint16_t) data[1]) << 8) | data[0];
}

static void
flash_TransferBuffer_Clear(flash_transfer_buffer* transfer)
{
    transfer->length = 0;
    transfer->padding = 0;
    transfer->in = 0;
    transfer->out = 0;
}

static void
flash_TransferBuffer_SetLength(flash_transfer_buffer* transfer, size_t length)
{
    transfer->length = length;
#ifdef FLARE_ZYNQMP
    transfer->padding = 0;
#else
    transfer->padding = (4 - (length & 3)) & 3;
#endif
    transfer->in = transfer->padding;
    transfer->out = transfer->padding;
}

static flash_error
flash_TransferBuffer_Fill(flash_transfer_buffer* transfer, const uint8_t data, size_t length)
{

    if ((transfer->in + length) >= sizeof(transfer->buffer))
        return FLASH_BUFFER_OVERFLOW;

    memset(transfer->buffer + transfer->in, data, length);

    transfer->in += length;

    return FLASH_NO_ERROR;
}

static flash_error
flash_TransferBuffer_Set8(flash_transfer_buffer* transfer, const uint8_t data)
{
    if (transfer->in >= sizeof(transfer->buffer))
        return FLASH_BUFFER_OVERFLOW;

    volatile uint8_t* p = &transfer->buffer[transfer->in++];
    *p = data;

    return FLASH_NO_ERROR;
}

flash_error
flash_TransferBuffer_Skip(flash_transfer_buffer* transfer, const size_t size)
{
#ifdef FLARE_ARM_ZYNQ7000
    if ((transfer->length - (transfer->out - transfer->padding)) < size)
        return FLASH_BUFFER_UNDERFLOW;

    transfer->out += size;
#endif
    return FLASH_NO_ERROR;
}

static flash_error
flash_TransferBuffer_Get8(flash_transfer_buffer* transfer, uint8_t* data)
{
    if ((transfer->length - (transfer->out - transfer->padding)) < sizeof(uint8_t))
        return FLASH_BUFFER_UNDERFLOW;

    *data = transfer->buffer[transfer->out++];

    return FLASH_NO_ERROR;
}

static flash_error
flash_TransferBuffer_Get16(flash_transfer_buffer* transfer, uint16_t* data)
{
    if ((transfer->length - (transfer->out - transfer->padding)) < sizeof(uint16_t))
        return FLASH_BUFFER_UNDERFLOW;


    *data = ((uint16_t) transfer->buffer[transfer->out++]) << 8;
    *data |= transfer->buffer[transfer->out++];

    return FLASH_NO_ERROR;
}

static flash_error
flash_TransferBuffer_CopyOut(flash_transfer_buffer* transfer,
                             uint8_t*               data,
                             const size_t           length)
{
    if ((transfer->length - (transfer->out - transfer->padding)) < length)
        return FLASH_BUFFER_UNDERFLOW;
    memcpy(data, transfer->buffer + transfer->out, length);
    transfer->out += length;

    return FLASH_NO_ERROR;
}

static flash_error
flash_TransferBuffer_CopyIn(flash_transfer_buffer* transfer,
                            const uint8_t*         data,
                            const size_t           length)
{
#ifdef FLARE_ZYNQMP
    transfer->padding = ((4 - (transfer->in & 3)) & 3);
    transfer->in = transfer->in + transfer->padding;
    transfer->length = transfer->length + transfer->padding;
#endif

    if ((transfer->in + length) > sizeof(transfer->buffer))
        return FLASH_BUFFER_OVERFLOW;

    memcpy(transfer->buffer + transfer->in, data, length);
    transfer->in += length;

    return FLASH_NO_ERROR;
}

static void
flash_TransferBuffer_SetAddr(flash_transfer_buffer* transfer,
                             uint32_t               address)
{
#if FLASH_4BYTE_ADDRESSING
    flash_TransferBuffer_Set8(transfer, (address >> 24) & 0xff);
#endif
    flash_TransferBuffer_Set8(transfer, (address >> 16) & 0xff);
    flash_TransferBuffer_Set8(transfer, (address >> 8) & 0xff);
    flash_TransferBuffer_Set8(transfer, address & 0xff);
}

static void flash_TransferBuffer_SetDir(flash_transfer_buffer* transfer,
                                        int                    trans_dir)
{
    transfer->trans_dir = trans_dir;
}

static void flash_TransferBuffer_SetCommandLen(flash_transfer_buffer* transfer,
                                               int                    comm_len)
{
    transfer->command_len = comm_len;
}

static void flash_TransferBuffer_SetCommMethod(flash_transfer_buffer* transfer,
                                               int                    comm_method)
{
    transfer->comm_method = comm_method;
}

static flash_error flash_SetRegions(void)
{
#ifdef FLARE_ARM_ZYNQ7000
  return flash_ReadCFI();
#else
  uint32_t tmp;
  flash_ReadId(&tmp, &tmp, &tmp);

  flash_num_regions = 1;

  flash_regions = FLASH_WORKSPACE_ALLOC(FLASH_WS_FLASH_REGIONS);
  if (!flash_regions)
  {
    return FLASH_NO_MEMORY;
  }

  flash_regions[0].count = flash_size/flash_erase_sector_size;
  flash_regions[0].size = flash_erase_sector_size;

  if (flash_erase_buffer_size < flash_regions[0].size)
      flash_erase_buffer_size = flash_regions[0].size;

  if (flash_erase_buffer_size)
  {
      flash_erase_buffer = FLASH_WORKSPACE_ALLOC(FLASH_WS_ERASE_BUFFER);
      if (!flash_erase_buffer)
      {
          flash_regions = NULL;
          return FLASH_NO_MEMORY;
      }
  }
  return FLASH_NO_ERROR;
#endif
}

static flash_error
flash_ReadRegister(uint8_t reg, uint16_t* value)
{
    flash_error fe;

    flash_TransferBuffer_Clear(flash_buf);
    flash_TransferBuffer_SetLength(flash_buf, FLASH_COMMAND_SIZE + 2);
    flash_TransferBuffer_Set8(flash_buf, reg);
    flash_TransferBuffer_Set8(flash_buf, 0);
    flash_TransferBuffer_Set8(flash_buf, 0);
    flash_TransferBuffer_SetDir(flash_buf, FLASH_RX_TRANS);
    flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE);
    flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_SINGLE);

    fe = flash_Transfer(flash_buf, initialised);
    if (fe != FLASH_NO_ERROR)
        return fe;

    flash_TransferBuffer_Get16(flash_buf, value);

    return FLASH_NO_ERROR;
}

static flash_error
flash_WaitForWrite(uint32_t wait)
{
    uint32_t checks = 100;
    while (true)
    {
        uint16_t    status;
        flash_error fe;

        if (wait_Handler != NULL)
            wait_Handler(wait_Handler_User);

        fe = flash_ReadRegister(FLASH_READ_STATUS_FLAG_CMD, &status);
        if (fe != FLASH_NO_ERROR)
            return fe;

        status = status & 0xFF;

        if ((status & FLASH_SR_E_ERR) != 0)
            return FLASH_ERASE_FAILURE;

        /*
         * A succuessful write requires the write latch and the write in
         * progress have cleared. If the write in progress is not set yet the
         * write latch remains set it is an error and the write command was not
         * received the flash device.
         */
        fe = flash_ReadRegister(FLASH_READ_STATUS_CMD, &status);
        status = status & 0xFF;
        if (fe != FLASH_NO_ERROR)
            return fe;

        if ((status & FLASH_SR_WIP) == 0)
        {
            if ((status & FLASH_SR_WEL) == 0)
                break;
            if (checks == 0)
                return FLASH_WRITE_ERASE_CMD_FAIL;
            --checks;
        }

        if (wait)
        {
            usleep(wait);
        }
    }
    return FLASH_NO_ERROR;
}

static flash_error
flash_SetWEL(void)
{
    uint16_t    status;
    flash_error fe;

    flash_TransferBuffer_Clear(flash_buf);
    flash_TransferBuffer_SetLength(flash_buf, FLASH_COMMAND_SIZE);
    flash_TransferBuffer_Set8(flash_buf, FLASH_WRITE_ENABLE_CMD);
    flash_TransferBuffer_SetDir(flash_buf, FLASH_TX_TRANS);
    flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE);
    flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_SINGLE);

    fe = flash_Transfer(flash_buf, initialised);
    if (fe != FLASH_NO_ERROR)
        return fe;

    fe = flash_ReadRegister(FLASH_READ_STATUS_CMD, &status);
    if (fe != FLASH_NO_ERROR)
        return fe;

    if ((status & FLASH_SR_WEL) == 0)
        return FLASH_READ_ONLY;

    return FLASH_NO_ERROR;
}

static flash_error
flash_ClearWEL(void)
{
    uint16_t    status;
    flash_error fe;

    flash_TransferBuffer_Clear(flash_buf);
    flash_TransferBuffer_SetLength(flash_buf, FLASH_COMMAND_SIZE);
    flash_TransferBuffer_Set8(flash_buf, FLASH_WRITE_DISABLE_CMD);
    flash_TransferBuffer_SetDir(flash_buf, FLASH_TX_TRANS);
    flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE);
    flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_SINGLE);

    fe = flash_Transfer(flash_buf, initialised);
    if (fe != FLASH_NO_ERROR)
        return fe;

    fe = flash_ReadRegister(FLASH_READ_STATUS_CMD, &status);
    if (fe != FLASH_NO_ERROR)
        return fe;

    if ((status & FLASH_SR_WEL) != 0)
        return FLASH_WRITE_LATCH_CLEAR_FAIL;

    return FLASH_NO_ERROR;
}

static flash_error
flash_ReadCFI(void)
{
    flash_error fe = FLASH_NO_ERROR;
    uint8_t     data = 0;
    size_t      region;

    if (!cfi_data)
    {
        flash_TransferBuffer_Clear(flash_buf);
        flash_TransferBuffer_SetLength(flash_buf, FLASH_COMMAND_SIZE + 7);
        flash_TransferBuffer_Set8(flash_buf, FLASH_READ_ID);
        flash_TransferBuffer_Fill(flash_buf, 0x00, 7);
        flash_TransferBuffer_SetDir(flash_buf, FLASH_RX_TRANS);
        flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE);
        flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_SINGLE);

        fe = flash_Transfer(flash_buf, initialised);
        if (fe != FLASH_NO_ERROR)
            return fe;

        flash_TransferBuffer_Skip(flash_buf, 3);
        flash_TransferBuffer_Get8(flash_buf, &data);

        if (data == 0)
            return FLASH_NO_MEMORY;

        cfi_length = data;

        flash_TransferBuffer_Clear(flash_buf);
        flash_TransferBuffer_SetLength(flash_buf, FLASH_COMMAND_SIZE + cfi_length);
        flash_TransferBuffer_Set8(flash_buf, FLASH_READ_ID);
        fe = flash_TransferBuffer_Fill(flash_buf, 0x00, cfi_length);
        flash_TransferBuffer_SetDir(flash_buf, FLASH_RX_TRANS);
        flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE);
        flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_SINGLE);
        if (fe != FLASH_NO_ERROR)
            return fe;

        fe = flash_Transfer(flash_buf, initialised);
        if (fe != FLASH_NO_ERROR)
            return fe;

        cfi_data = FLASH_WORKSPACE_ALLOC(FLASH_WS_CFI);
        if (!cfi_data)
            return FLASH_NO_MEMORY;

        fe = flash_TransferBuffer_CopyOut(flash_buf, cfi_data, cfi_length);
        if (fe != FLASH_NO_ERROR)
        {
            cfi_data = NULL;
            return fe;
        }

        flash_page_size = 1 << flash_Get16(cfi_data + 0x2a);
        flash_size = 1 << flash_Get8(cfi_data + 0x27);
        flash_num_regions = flash_Get8(cfi_data + 0x2c);

        flash_regions = FLASH_WORKSPACE_ALLOC(FLASH_WS_FLASH_REGIONS);
        if (!flash_regions)
        {
            cfi_data = NULL;
            cfi_length = 0;
            return FLASH_NO_MEMORY;
        }

        for (region = 0; region < flash_num_regions; ++region)
        {
            flash_regions[region].count =
                flash_Get16(cfi_data + 0x2d + (region * 4)) + 1;
            flash_regions[region].size =
                flash_Get16(cfi_data + 0x2f + (region * 4)) * 256;

            if (flash_erase_buffer_size < flash_regions[region].size)
                flash_erase_buffer_size = flash_regions[region].size;
        }

        if (flash_erase_buffer_size)
        {
            flash_erase_buffer = FLASH_WORKSPACE_ALLOC(FLASH_WS_ERASE_BUFFER);
            if (!flash_erase_buffer)
            {
                cfi_data = NULL;
                cfi_length = 0;
                flash_regions = NULL;
                return FLASH_NO_MEMORY;
            }
        }
    }

    return fe;
}

static flash_error
flash_FindRegion(uint32_t address, uint32_t* base, size_t* length)
{
    flash_error fe;
    size_t      region;

    *base = 0;
    *length = 0;

    fe = flash_SetRegions();
    if (fe != FLASH_NO_ERROR)
        return fe;

    for (region = 0; region < flash_num_regions; ++region)
    {
        uint32_t sector;
        for (sector = 0; sector < flash_regions[region].count; ++sector)
        {
            if ((address >= *base) &&
                (address < (*base + flash_regions[region].size)))
            {
                *length = flash_regions[region].size;
                return FLASH_NO_ERROR;
            }

            *base += flash_regions[region].size;
        }
    }

    return FLASH_BAD_ADDRESS;
}

flash_error
flash_GetRegions(flash_region** regions, size_t* size)
{
    flash_error fe;
    size_t      region;

    *regions = NULL;
    *size = 0;

    fe = flash_SetRegions();
    if (fe != FLASH_NO_ERROR)
        return fe;

    *regions = flash_regions;
    *size = flash_num_regions;

    return FLASH_NO_ERROR;
}

flash_error
flash_Read(uint32_t address, void* buffer, size_t length)
{
    uint8_t* data = buffer;

#ifdef FLARE_VERSAL
    if (address%2) {
        return FLASH_BAD_ADDRESS;
    }
    address = address / 2;
#endif

    while (length)
    {
        flash_error fe;
        size_t      size;

        if (flash_page_size == 0) {
          return FLASH_INVALID_DEVICE;
        }
        size = length > flash_page_size ? flash_page_size : length;

        flash_TransferBuffer_Clear(flash_buf);
        flash_TransferBuffer_SetLength(flash_buf,
                                       FLASH_COMMAND_SIZE + FLASH_ADDRESS_SIZE
                                       + flash_read_dummies + size);
        flash_TransferBuffer_Set8(flash_buf, FLASH_READ_CMD);
        flash_TransferBuffer_SetAddr(flash_buf, address);
        fe = flash_TransferBuffer_Fill(flash_buf, 0, flash_read_dummies + size);
        if (fe != FLASH_NO_ERROR)
            return fe;
        flash_TransferBuffer_SetDir(flash_buf, FLASH_RX_TRANS);
        flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE + FLASH_ADDRESS_SIZE + flash_read_dummies);
        flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_ALL);

        fe = flash_Transfer(flash_buf, initialised);
        if (fe != FLASH_NO_ERROR)
            return fe;

        flash_TransferBuffer_Skip(flash_buf, FLASH_ADDRESS_SIZE + flash_read_dummies);

        fe = flash_TransferBuffer_CopyOut(flash_buf, data, size);
        if (fe != FLASH_NO_ERROR)
            return fe;

        length -= size;
        data += size;
#ifdef FLARE_VERSAL
        address = address + (size/2);
#else
        address += size;
#endif
    }

    return FLASH_NO_ERROR;
}

flash_error
flash_Blank(uint32_t address, size_t length)
{
    flash_error fe = FLASH_NO_ERROR;

    if ((address >= flash_size) || ((address + length) > flash_size))
        return FLASH_BAD_ADDRESS;

#ifdef FLARE_ZYNQMP
    if (address%2) {
        return FLASH_BAD_ADDRESS;
    }
    address = address / 2;
#endif

    while (length)
    {
        size_t      size;

        if (flash_page_size == 0) {
          return FLASH_INVALID_DEVICE;
        }
        size = length > flash_page_size ? flash_page_size : length;

        flash_TransferBuffer_Clear(flash_buf);
        flash_TransferBuffer_SetLength(flash_buf,
                                       FLASH_COMMAND_SIZE + FLASH_ADDRESS_SIZE
                                       + flash_read_dummies + size);
        flash_TransferBuffer_Set8(flash_buf, FLASH_READ_CMD);
        flash_TransferBuffer_SetAddr(flash_buf, address);
        fe = flash_TransferBuffer_Fill(flash_buf, 0, flash_read_dummies + size);
        if (fe != FLASH_NO_ERROR)
                return fe;
        flash_TransferBuffer_SetDir(flash_buf, FLASH_RX_TRANS);
        flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE + FLASH_ADDRESS_SIZE + flash_read_dummies);
        flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_ALL);

        fe = flash_Transfer(flash_buf, initialised);
        if (fe != FLASH_NO_ERROR)
            return fe;

        flash_TransferBuffer_Skip(flash_buf, FLASH_ADDRESS_SIZE);

        length -= size;
        address += size;

        while (size)
        {
            uint8_t byte = 0;
            flash_TransferBuffer_Get8(flash_buf, &byte);
            if (byte != 0xff)
                return FLASH_NOT_BLANK;
            --size;
        }
    }

    return fe;
}

flash_error
flash_EraseSector(uint32_t address)
{
    flash_error fe = FLASH_NO_ERROR;
    uint32_t    base = 0;
    size_t      length = 0;

    if ((address >= flash_size) || ((address + length) > flash_size))
        return FLASH_BAD_ADDRESS;

#ifdef FLARE_ZYNQMP
    if (address%2) {
        return FLASH_BAD_ADDRESS;
    }
    address = address / 2;
#endif

    fe = flash_FindRegion(address, &base, &length);
    if (fe != FLASH_NO_ERROR)
        return fe;

    flash_WriteUnlock();

    fe = flash_SetWEL();
    if (fe != FLASH_NO_ERROR)
    {
        flash_WriteLock();
        return fe;
    }

    flash_TransferBuffer_Clear(flash_buf);
    flash_TransferBuffer_SetLength(flash_buf, FLASH_COMMAND_SIZE + FLASH_ADDRESS_SIZE);
    flash_TransferBuffer_Set8(flash_buf, FLASH_SEC_ERASE_CMD);
    flash_TransferBuffer_SetAddr(flash_buf, address);
    flash_TransferBuffer_SetDir(flash_buf, FLASH_TX_TRANS);
    flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE + FLASH_ADDRESS_SIZE);
    flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_ALL);

    fe = flash_Transfer(flash_buf, initialised);
    if (fe != FLASH_NO_ERROR)
    {
        flash_ClearWEL();
        flash_WriteLock();
        return fe;
    }

    fe = flash_WaitForWrite(1000);
    if (fe != FLASH_NO_ERROR)
    {
        flash_ClearWEL();
        flash_WriteLock();
        return fe;
    }

    flash_WriteLock();

    fe = flash_Blank(base, length);
    if (fe != FLASH_NO_ERROR)
        return fe;

    return fe;
}

flash_error
flash_EraseDevice(void)
{
    flash_error fe = FLASH_NO_ERROR;

    flash_WriteUnlock();

    fe = flash_SetWEL();
    if (fe != FLASH_NO_ERROR)
    {
        flash_WriteLock();
        return fe;
    }

    flash_TransferBuffer_Clear(flash_buf);
    flash_TransferBuffer_SetLength(flash_buf, 1);
    flash_TransferBuffer_Set8(flash_buf, FLASH_BULK_ERASE_CMD);
    flash_TransferBuffer_SetDir(flash_buf, FLASH_TX_TRANS);
    flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE + FLASH_ADDRESS_SIZE);
    flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_ALL);

    fe = flash_Transfer(flash_buf, initialised);
    if (fe != FLASH_NO_ERROR)
    {
        flash_ClearWEL();
        flash_WriteLock();
        return fe;
    }

    fe = flash_WaitForWrite(1000000);
    if (fe != FLASH_NO_ERROR)
    {
        flash_ClearWEL();
        flash_WriteLock();
        return fe;
    }

    flash_WriteLock();

    return fe;
}

flash_error
flash_WriteSector(uint32_t address, const void* buffer, size_t length)
{
    flash_error    fe = FLASH_NO_ERROR;
    const uint8_t* data = buffer;

    if ((address >= flash_size) || ((address + length) > flash_size))
        return FLASH_BAD_ADDRESS;

#ifdef FLARE_ZYNQMP
    if (address%2) {
        return FLASH_BAD_ADDRESS;
    }
    address = address / 2;
#endif

    flash_WriteUnlock();

    while (length)
    {
        flash_error fe;
        size_t      size;
        size_t      byte;
        bool        write_block = false;

        if (flash_page_size == 0) {
          return FLASH_INVALID_DEVICE;
        }
        size = length > flash_page_size ? flash_page_size : length;

        /*
         * If the address is not aligned to the start of a page transfer
         * the remainder of the page so the address is aligned.
         */
        if ((address % flash_page_size) != 0)
        {
            size_t remaining = flash_page_size - (address % flash_page_size);
            if (size > remaining)
                size = remaining;
        }

        /*
         * If the block is blank do not write it.
         */
        for (byte = 0; byte < size; ++byte)
        {
            if (data[byte] != 0xff)
            {
                write_block = true;
                break;
            }
        }

        if (write_block)
        {
            fe = flash_SetWEL();
            if (fe != FLASH_NO_ERROR)
            {
                flash_WriteLock();
                return fe;
            }

            flash_TransferBuffer_Clear(flash_buf);
            flash_TransferBuffer_SetLength(flash_buf, FLASH_COMMAND_SIZE
                  + FLASH_ADDRESS_SIZE + size);
            flash_TransferBuffer_Set8(flash_buf, FLASH_WRITE_CMD);
            flash_TransferBuffer_SetAddr(flash_buf, address);
            flash_TransferBuffer_SetDir(flash_buf, FLASH_TX_TRANS);
            flash_TransferBuffer_SetCommandLen(flash_buf,
                FLASH_COMMAND_SIZE + FLASH_ADDRESS_SIZE);
            flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_ALL);
            fe = flash_TransferBuffer_CopyIn(flash_buf, data, size);
            if (fe != FLASH_NO_ERROR)
            {
                flash_WriteLock();
                return fe;
            }

            fe = flash_Transfer(flash_buf, initialised);
            if (fe != FLASH_NO_ERROR)
            {
                flash_ClearWEL();
                flash_WriteLock();
                return fe;
            }

            fe = flash_WaitForWrite(1000);
            if (fe != FLASH_NO_ERROR)
            {
                flash_ClearWEL();
                flash_WriteLock();
                return fe;
            }
        }

        address += size;
        data += size;
        length -= size;
    }

    flash_WriteLock();

    return fe;
}

flash_error
flash_ReadId(uint32_t* manufactureCode,
             uint32_t* memIfaceType,
             uint32_t* density)
{
    uint8_t     buffer[64];
    uint8_t     value = 0;
    flash_error fe;

    flash_TransferBuffer_Clear(flash_buf);
    flash_TransferBuffer_SetLength(flash_buf, 1 + 3);
    flash_TransferBuffer_Set8(flash_buf, FLASH_READ_ID);
    flash_TransferBuffer_Fill(flash_buf, 0x00, 3);
    flash_TransferBuffer_SetDir(flash_buf, FLASH_RX_TRANS);
    flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE);
    flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_SINGLE);


  fe = flash_Transfer(flash_buf, initialised);
    if (fe != FLASH_NO_ERROR)
        return fe;

    flash_TransferBuffer_Get8(flash_buf, &value);
    *manufactureCode = value;
    flash_TransferBuffer_Get8(flash_buf, &value);
    *memIfaceType = value;
    flash_TransferBuffer_Get8(flash_buf, &value);
    *density = value;

    if (*manufactureCode == 1)
    {
        if ((*memIfaceType == 0x20) && (*density == 0x18))
        {
            flash_size = 16UL * 1024UL * 1024UL;
            flash_erase_sector_size = 64UL * 1024UL;
            flash_page_size = 256;
        }
        else if ((*memIfaceType == 0x02) && (*density == 0x20))
        {
            flash_size = 64UL * 1024UL * 1024UL;
            flash_erase_sector_size = 256UL * 1024UL;
            flash_page_size = 512;
        }
    }
    else if (*manufactureCode == 0x20)
    {
        if (*density == 0x19) {
            flash_size = 0x2000000; /* 256 Mib on one flash */
            flash_erase_sector_size = 0x10000UL; /* 64Kib erase sector */
            flash_page_size = 256 * 2;
        }
        else if (*density == 0x21)
        {
            flash_size = 0x8000000UL * 2UL; /* 1 Gib on each flash in parallel */
            flash_erase_sector_size = 0x10000UL * 2UL; /* 128Kib erase sector */
            flash_page_size = 256 * 2;
        }
        else if (*density == 0x22)
        {
            flash_size = 0x10000000UL * 2UL; /* 2Gib on each flash in parallel */
            flash_erase_sector_size = 0x10000UL * 2UL; /* 128Kib erase sector */
            flash_page_size = 256 * 2;
        }
    }

    flash_WriteUnlock();

    fe = flash_SetWEL();
    if (fe != FLASH_NO_ERROR)
    {
        flash_WriteLock();
        return fe;
    }

    flash_TransferBuffer_Clear(flash_buf);
    flash_TransferBuffer_SetLength(flash_buf, 1 + 2);
    flash_TransferBuffer_Set8(flash_buf, FLASH_WRITE_STATUS_CMD);
    flash_TransferBuffer_Set8(flash_buf, 0);
    flash_TransferBuffer_Set8(flash_buf, 0x82);
    flash_TransferBuffer_SetDir(flash_buf, FLASH_TX_TRANS);
    flash_TransferBuffer_SetCommandLen(flash_buf, FLASH_COMMAND_SIZE);
    flash_TransferBuffer_SetCommMethod(flash_buf, FLASH_COMM_METHOD_SINGLE);

    fe = flash_Transfer(flash_buf, initialised);
    if (fe != FLASH_NO_ERROR)
    {
        flash_ClearWEL();
        flash_WriteLock();
        return fe;
    }

    flash_ClearWEL();
    flash_WriteLock();

#if FLASH_FAST_READ
    flash_read_dummies = 1;
#endif

    flash_Read(0, buffer, sizeof(buffer));

    return FLASH_NO_ERROR;
}

size_t
flash_DeviceSize(void)
{
    return flash_size;
}

size_t
flash_DeviceSectorEraseSize(void)
{
    return flash_erase_sector_size;
}

void
flash_RegisterWaitHandler(flash_WaitHandler handler, void* user)
{
    wait_Handler = handler;
    wait_Handler_User = user;
}
