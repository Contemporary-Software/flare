
#include <stdint.h>
#include <stdio.h>

#include "board-handoff.h"
#include "cache.h"
#include "factory-boot.h"
#include "flare-boot.h"
#include "flash.h"
#include "flash-map.h"
#include "wdog.h"
#include "zynqmp-factory-boot.h"
#include "zynqmp-atf.h"

void zynqmp_factory_second_stage_boot(void) {
    uint8_t  *header = factory_header;
    uint32_t flash_offset;
    uint32_t *load_size_record;
    uint32_t *load_addr_record;
    size_t   header_size = sizeof(factory_header);
    size_t   size;
    size_t   load_size;
    size_t   load_addr;
    bool     rc;
    const char* factory_boot_label = "EXE";

    size = FactoryLoadImage_Get32(header, 1, IMAGE_HEADER_SIZE);
    if (size > FLARE_EXECUTABLE_SIZE)
    {
        printf("error: %s factory image too big: %08x\n", factory_boot_label, size);
        return;
    }

    flash_offset = FLARE_FLASH_EXE_BASE + header_size +
      FactoryLoadImage_Get32(header, 0, IMAGE_HEADER_SIZE);

    load_size_record = (uint32_t*)IMAGE_HEADER_RECORD(header, 1, IMAGE_HEADER_SIZE);
    load_size = (size_t)(*load_size_record);
    load_addr_record = (uint32_t*)IMAGE_HEADER_RECORD(header, 1, IMAGE_HEADER_BASE_ADDR);
    load_addr = (size_t)(*load_addr_record);

    printf(" Executable : %s at 0x%x for 0x%x\n",
           IMAGE_HEADER_RECORD(header, 1, IMAGE_HEADER_NAME), load_addr, load_size);

    rc = factory_load_image(factory_boot_label,
                          flash_offset,
                          size,
                          IMAGE_HEADER_RECORD(header, 1, IMAGE_HEADER_BASE_ADDR),
                          &load_size,
                          IMAGE_HEADER_RECORD(header, 1, IMAGE_HEADER_MD5),
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
    board_handoff_exit(FLARE_BOOT_BASE);
}

void
platform_factory_booter(uint8_t* header, size_t header_size)
{
    uint32_t flash_offset;
    size_t   size;
    size_t   load_size;
    bool     rc;
    const char* factory_boot_label = "ATF";

    //flare_DataSafe_SetFactoryBoot();

    size = FactoryLoadImage_Get32(header, 0, IMAGE_HEADER_SIZE);
    if (size > ATF_SIZE)
    {
        printf("error: %s factory image too big: %08x\n", factory_boot_label, size);
        return;
    }

    flash_offset = FLARE_FLASH_EXE_BASE + header_size;

    printf(" Executable : %s\n",
           IMAGE_HEADER_RECORD(header, 0, IMAGE_HEADER_NAME));

    load_size = ATF_SIZE;

    rc = factory_load_image(factory_boot_label,
                          flash_offset,
                          size,
                          (uint8_t*) ATF_LOAD_ADDRESS,
                          &load_size,
                          IMAGE_HEADER_RECORD(header, 0, IMAGE_HEADER_MD5),
                          false);
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
    board_handoff_exit(ATF_ENTRYPOINT);
}
