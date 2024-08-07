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

/*
 * Xilinx Versal.
 */

#include <stdio.h>
#include "board.h"
#include <console.h>
#include <stdbool.h>
#include <inttypes.h>

struct error_stack_frame {
  uint64_t padding[3];
  uint64_t Xn[31];
  uint64_t ELR;
  uint64_t ESR;
  uint64_t FAR;
  uint64_t stack[64];
};

static void
error_lockdown(void)
{
  printf("Bootloader failure. Resetting ...  \b \b \b \b");
  out_flush();
  while (true);
}

static void
trace_error(const char* label)
{
  int                i;
  volatile struct error_stack_frame *frame = (struct error_stack_frame*) &frame;

  printf("\nFATAL: %sStack Pointer: %016lx\n", label, (uint64_t) frame->stack);

  printf("ELR: %016lx\n", frame->ELR);
  printf("ESR: %016lx\n", frame->ESR);
  printf("FAR: %016lx\n", frame->FAR);

  for (i = 0; i < 30; i = i + 2)
  {
    printf("x%02d: 0x%016lx   x%02d: 0x%016lx\n", i, frame->Xn[i], i+1, frame->Xn[i+1]);
  }
  printf("x%02d: 0x%016lx\n", 30, frame->Xn[30]);

  printf("\n--------Stack--------");
  for (i = 0; i < 64; ++i)
  {
    if ((i & 0x3) == 0)
      printf("\n ");
    printf("%016lx ", frame->stack[i]);
  }
  printf("\n ");

  printf("- Stack from error handler -\n");
  printf(" %016lx %016lx %016lx\n", frame->padding[0], frame->padding[1], frame->padding[2]);
}

void SWInterrupt(void);
void FIQInterrupt(void);
void IRQInterrupt(void);
void SError(void);

void
SError(void)
{
  trace_error("SError\n");
  error_lockdown();
}

void
SWInterrupt(void)
{
  trace_error("SWI\n");
  error_lockdown();
}

void
IRQInterrupt(void)
{
  trace_error("IRQ\n");
  error_lockdown();
}

void
FIQInterrupt(void)
{
  trace_error("FIQ\n");
  error_lockdown();
}
void
board_hardware_setup(void)
{
}
