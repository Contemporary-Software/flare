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


#if !defined(_FLARE_FLASH_H_)
#define _FLARE_FLASH_H_

#ifdef FLARE_VERSAL
  #include "versal-flash-map.h"
#elif FLARE_ZYNQMP
  #include "zynqmp-flash-map.h"
#else
  #include "zynq7000-flash-map.h"
#endif

/* Flash Dev path */
#define FLASHDEV_PATH "/dev/flashdev0"

/*
 * Table header:
 *
 *   MD5[16]
 *   REC_SIZE[4]
 *   RECORDS (see the record structure)
 */
#define IMAGE_HEADER_HEADER_MD5      (0)
#define IMAGE_HEADER_HEADER_MD5_SIZE (16)
#define IMAGE_HEADER_HEADER_REC      (IMAGE_HEADER_HEADER_MD5 + IMAGE_HEADER_HEADER_MD5_SIZE)
#define IMAGE_HEADER_HEADER_REC_SIZE (4)
#define IMAGE_HEADER_HEADER_SIZE     (IMAGE_HEADER_HEADER_REC + IMAGE_HEADER_HEADER_REC_SIZE)

/*
 * Record structure:
 *
 *  NAME[31]
 *  FLAGS[1]
 *  SIZE[4]
 *  BASE_ADDR[4]
 *  MD5[16]
 */
#define IMAGE_HEADER_NAME            (0)
#define IMAGE_HEADER_NAME_SIZE       (31)
#define IMAGE_HEADER_FLAGS           (IMAGE_HEADER_NAME + IMAGE_HEADER_NAME_SIZE)
#define IMAGE_HEADER_FLAGS_SIZE      (1)
#define IMAGE_HEADER_SIZE            (IMAGE_HEADER_FLAGS + IMAGE_HEADER_FLAGS_SIZE)
#define IMAGE_HEADER_SIZE_SIZE       (4)
#define IMAGE_HEADER_BASE_ADDR       (IMAGE_HEADER_SIZE + IMAGE_HEADER_SIZE_SIZE)
#define IMAGE_HEADER_BASE_ADDR_SIZE  (4)
#define IMAGE_HEADER_ENTRY_PT        (IMAGE_HEADER_BASE_ADDR + IMAGE_HEADER_BASE_ADDR_SIZE)
#define IMAGE_HEADER_ENTRY_PT_SIZE   (4)
#define IMAGE_HEADER_MD5             (IMAGE_HEADER_ENTRY_PT + IMAGE_HEADER_ENTRY_PT_SIZE)
#define IMAGE_HEADER_MD5_SIZE        (16)
#define IMAGE_HEADER_RECORD_SIZE     (IMAGE_HEADER_MD5 + IMAGE_HEADER_MD5_SIZE)
#define IMAGE_HEADER_RECORD(i, r, o) ((i) + IMAGE_HEADER_HEADER_SIZE + \
                                        ((r) * IMAGE_HEADER_RECORD_SIZE) + (o))

#define IMAGE_HEADER_TOTAL(r)        (IMAGE_HEADER_HEADER_SIZE + \
                                        ((r) * IMAGE_HEADER_RECORD_SIZE))

#endif /* _FLARE_FLASH_H_ */
