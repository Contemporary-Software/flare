
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
#if !defined(UBOOT_H)
#define UBOOT_H

#define UBOOT_MAGIC_NUM_OFF  0x00
#define UBOOT_CRC_OFF        0x04
#define UBOOT_TIME_STAMP_OFF 0x08
#define UBOOT_DATA_SIZE_OFF  0x0C
#define UBOOT_LOAD_ADDR_OFF  0x10
#define UBOOT_ENTRY_PT_OFF   0x14
#define UBOOT_IMAGE_CRC_OFF  0x18
#define UBOOT_OS_OFF         0x1C
#define UBOOT_ARCH_OFF       0x1D
#define UBOOT_TYPE_OFF       0x1E
#define UBOOT_COMP_OFF       0x1F
#define UBOOT_IMAGE_NAME_OFF 0x20
#define UBOOT_DATA_OFF       0x40

#define UBOOT_COMPRESSION_NONE 0
#define UBOOT_COMPRESSION_GZIP 1

#define UBOOT_MAGIC_NUMBER 0x27051956

#define UBOOT_NAME_LEN 32

#endif
