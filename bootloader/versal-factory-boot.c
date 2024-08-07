#include "factory-boot.h"

void
platform_factory_booter(uint8_t* header, size_t header_size)
{
    uint32_t flash_offset;
    size_t   size;
    size_t   load_size;
    bool     rc;

    //flare_DataSafe_SetFactoryBoot();

    size = FactoryLoadImage_Get32(header, 0, IMAGE_HEADER_SIZE);
    if (size > EXECUTABLE_LOAD_SIZE)
    {
        printf("error: %s factory image too big: %08x\n", factory_boot_label, size);
        return;
    }

    flash_offset = FLASH_SLOT_BASE + header_size;

    printf("  Executable: FB_%s/%s\n",
           "GITHASH",
           IMAGE_HEADER_RECORD(header, 0, IMAGE_HEADER_NAME));

    load_size = EXECUTABLE_LOAD_SIZE;

    rc = FactoryLoadImage(factory_boot_label,
                          flash_offset,
                          size,
                          (uint8_t*) EXECUTABLE_LOAD_BASE,
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
    board_handoff_exit(EXECUTABLE_LOAD_BASE);
}
