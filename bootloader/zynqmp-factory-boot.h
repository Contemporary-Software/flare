
#include "flash-map.h"

#define FLASH_SLOT_FILES     (2)

extern uint8_t factory_header[IMAGE_HEADER_TOTAL(FLASH_SLOT_FILES)];

void zynqmp_factory_second_stage_boot(void);
