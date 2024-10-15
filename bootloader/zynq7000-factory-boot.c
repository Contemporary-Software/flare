#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "factory-boot.h"
#include "board-handoff.h"
#include "flare-boot.h"
#include "cache.h"
#include "wdog.h"
#include "flash.h"
#include "flash-map.h"

#define EXECUTABLE_LOAD_BASE  FLARE_EXECUTABLE_BASE
#define EXECUTABLE_ENTRY_PT   FLARE_EXECUTABLE_ENTRY_PT
#define EXECUTABLE_LOAD_SIZE  FLARE_EXECUTABLE_SIZE
#define FLASH_SLOT_BASE       FLARE_FLASH_EXE_BASE

static const char* const factory_boot_label = "EXE";

void
platform_factory_booter(uint8_t* header, size_t header_size)
{
    uint32_t flash_offset;
    uint32_t *load_size_record;
    uint32_t *entry_point_record;
    uint32_t *load_addr_record;
    uint8_t  *load_addr;
    size_t   size;
    size_t   load_size;
    size_t   entry_point;
    bool     rc;

    //flare_DataSafe_SetFactoryBoot();

    size = FactoryLoadImage_Get32(header, 0, IMAGE_HEADER_SIZE);
    if (size > EXECUTABLE_LOAD_SIZE)
    {
        printf("error: %s factory image too big: %08x\n", factory_boot_label, size);
        return;
    }

    flash_offset = FLASH_SLOT_BASE + header_size;

    size = FactoryLoadImage_Get32(header, 0, IMAGE_HEADER_SIZE);
    if (size > EXECUTABLE_LOAD_SIZE)
    {
        printf("error: %s factory image too big\n", factory_boot_label);
        return;
    }

    printf("  Executable: %s\n",
           IMAGE_HEADER_RECORD(header, 0, IMAGE_HEADER_NAME));

    load_size = EXECUTABLE_LOAD_SIZE;
    load_addr_record = (uint32_t*)IMAGE_HEADER_RECORD(header, 0, IMAGE_HEADER_BASE_ADDR);
    load_addr = (uint8_t*)(*load_addr_record);
    entry_point_record = (uint32_t*)IMAGE_HEADER_RECORD(header, 0, IMAGE_HEADER_ENTRY_PT);
    entry_point = (size_t)(*entry_point_record);

    printf("            Size: 0x%08x\n",
           load_size);
    printf("    Base Address: 0x%08x\n",
           *load_addr_record);
    printf("     Entry Point: 0x%08x\n",
           entry_point);

    rc = factory_load_image(factory_boot_label,
                            flash_offset,
                            size,
                            load_addr,
                            &load_size,
                            IMAGE_HEADER_RECORD(header, 0, IMAGE_HEADER_MD5),
                            true);
    if (!rc)
        return;
    /*
    flare_DataSafe_FlareSet(QSPI_MODE,
                            (const char*) IMAGE_HEADER_RECORD(header, 0, IMAGE_HEADER_NAME),
                            (const char*) IMAGE_HEADER_RECORD(header, 1, IMAGE_HEADER_NAME),
                            0);

    flare_DataSafe_ClearFactoryBootRequest();
    */
    wdog_control(true);
    cache_disable();
    board_handoff_exit(entry_point);
}
