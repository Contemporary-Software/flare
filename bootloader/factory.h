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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * The size of the factory data table in the flash.
 *
 * @note The table is the size a parameter sector in the flash device on the
 *       flare board however the default for the flash is the paramater
 *       sections are held in the bottom of the flash. This table is written to
 *       the top of the flash device so the bottom can be used to boot the
 *       card. There is a OTP bit in the flash's control register that can
 *       remap the parameter sections to the top of the device but this is
 *       currently not set.
 *
 * @note For Flare-2 the factory settings are copied from the datasafe to
 *       this table. This keeps the number of differences small.
 */
#define FACTORY_TABLE_FLASH_SIZE (4 * 1024)

/*
 * The factory checksum size.
 */
#define FACTORY_CHECKSUM_SIZE (16)

/**
 * The number of MAC addresses.
 */
#define FACTORY_MAC_ADDRESSES (4)

/*
 * A MAC Address.
 */
typedef uint8_t factory_mac_address[6];

/*
 * Maximum string size.
 */
#define FACTORY_STRING_SIZE (64)

/*
 * The factory strings.
 */
#define FACTORY_SERIAL_NUMBER (0)
#define FACTORY_PART_NUMBER   (1)
#define FACTORY_REVISION      (2)
#define FACTORY_MODSTRIKE     (3)
#define FACTORY_BOOT_OPTIONS  (4)
#define FACTORY_STRINGS       (5)

/*
 * Table layout.
 */
#define FACTORY_CHECKSUM_OFF     (0)
#define FACTORY_MAC_ADDR_OFF     (FACTORY_CHECKSUM_SIZE)
#define FACTORY_SERIAL_NUM_OFF   (FACTORY_MAC_ADDR_OFF + \
                                  (FACTORY_MAC_ADDRESSES * sizeof(factory_mac_address)))
#define FACTORY_PART_NUM_OFF     (FACTORY_SERIAL_NUM_OFF + FACTORY_STRING_SIZE)
#define FACTORY_REVISION_OFF     (FACTORY_PART_NUM_OFF + FACTORY_STRING_SIZE)
#define FACTORY_MODSTRIKE_OFF    (FACTORY_REVISION_OFF + FACTORY_STRING_SIZE)
#define FACTORY_BOOT_OPTIONS_OFF (FACTORY_MODSTRIKE_OFF + FACTORY_STRING_SIZE)

#define FACTORY_TABLE_SIZE \
  (FACTORY_CHECKSUM_SIZE + (FACTORY_MAC_ADDRESSES * sizeof(factory_mac_address)) + \
   (FACTORY_STRINGS * FACTORY_STRING_SIZE))

/**
 * Initialsie the board factory data.
 */
bool factory_Init(void);

/**
 * The factory data is valid.
 */
bool factory_DataValid(void);

/**
 * Is there a valid factory configuration?
 */
bool factory_ConfigValid(void);

/**
 * Is the board's MAC address valid?
 *
 * @param mac The mac address to return.
 *
 * @retval true The MAC address is valid.
 * @retval false Invalid MAC address or the factory data is not valid.
 */
bool factory_MAC_Valid(int mac);

/**
 * The board's MAC address.
 *
 * @param mac The mac address to return.
 * @param address The mac address is true is returned.
 *
 * @retval true The MAC address is valid.
 * @retval false Invalid MAC address or the factory data is not valid.
 */
bool factory_MAC (int mac, factory_mac_address* address);

/**
 * The board's serial number. This is an ASCII string.
 *
 * @return const char* The serial number as a string. If NULL The factory data
 *                     is not valid.
 */
const char* factory_SerialNumber (void);

/**
 * The board's part number. This is an ASCII string.
 *
 * @return const char* The part number as a string. If NULL The factory data
 *                     is not valid.
 */
const char* factory_PartNumber (void);

/**
 * The board's revision. This is an ASCII string.
 *
 * @return const char* The revision as a string. If NULL The factory data
 *                     is not valid.
 */
const char* factory_Revision (void);

/**
 * The board's modstrike. This is an ASCII string.
 *
 * @return const char* The modstrike as a string. If NULL The factory data
 *                     is not valid.
 */
const char* factory_Modstrike (void);

/**
 * Export the factory settings.
 */
size_t factory_Export(char* buffer, size_t buffer_size);

/**
 * Import the factory settings.
 */
bool factory_Import(const char* buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
