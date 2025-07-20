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

/**
 * @brief Board Factory Settings.
 *
 * Factory settings are held in the flash device and set in the factory.
 *
 * There is no locking with these calls the commands to set these values needs
 * to be available only to the factory.
 */

#if !defined(BOARD_FACTORY_H)
#define BOARD_FACTORY_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Factory data definition.
 *
 * Definitions can be found in factory-data.txt.
 * This is format 1.
 */
#define FACTORY_CRC_LENGTH 170

typedef struct {
    uint32_t crc32;
    uint32_t length;
    uint32_t format;
#define FACTORY_STRING_SIZE (32)
    char assembly_serial_number[FACTORY_STRING_SIZE];
    char board_serial_number[FACTORY_STRING_SIZE];
    char part_number[FACTORY_STRING_SIZE];
    char revision[FACTORY_STRING_SIZE];
    char modstrike[FACTORY_STRING_SIZE];
    uint8_t mac_address[6];
} flare_factory_data;

#endif
