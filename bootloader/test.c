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

#include <stdio.h>
#include "flash.c"
#include "board-timer.h"


void print_buff(uint8_t* buff, int len) {

  printf("Buffer:");
  for (int i = 0; i < len; i++) {
    if (i%8 == 0) {
      printf("\n");
    }
    printf("%02x ", buff[i]);
  }
  printf("\n");

}

int main(void) {
  uint32_t manufactureCode;
  uint32_t memIfaceType;
  uint32_t density;
  uint64_t timer;
  uint8_t buffer[0x100] = {0};
  
  board_timer_reset();
  
  printf("Test\n");
  
  board_timer_get(&timer);
  printf("%ld\n", timer);
  usleep(10000000);
  board_timer_get(&timer);
  printf("%ld\n", timer);

  printf("Begin\n");

  flash_RegisterWaitHandler(NULL, NULL);

  flash_ReadId(&manufactureCode, &memIfaceType, &density);

  flash_Read(0x03001000, buffer, 20);

  print_buff(buffer, 20);

  flash_EraseSector(0x03000000);

  flash_Read(0x03001000, buffer, 20);

  print_buff(buffer, 20);
  printf("String: %s\n", buffer);

  char test[10] = "Test TEST";

  flash_WriteSector(0x03001000, test, 10);

  flash_Read(0x03001000, buffer, 20);
  print_buff(buffer, 20);

  printf("String: %s\n", buffer);

  printf("manufactureCode: %x\nmemIfaceType %x\ndensity: %x\n", manufactureCode, memIfaceType, density);

  return 0;
}
