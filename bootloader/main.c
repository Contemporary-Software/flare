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
 * Boot loader entry.
 */

#include <stdbool.h>

//#include "fsbl.h"
#include "ps7_init.h"
//#include "sd.h"

#include <xil_cache.h>
#include <xil_cache_l.h>
#include <xil_exception.h>

//#include "status.h"
//#include "ssbl-boot.h"

//#include "datasafe.h"

//#include "xwdtps.h"
//#include "xuartps_hw.h"

#include "wdog.h"

/*
 * Always enabled.
 */
#define DCACHE_ICACHE_ENABLE 1

#define WDT_DEVICE_ID   XPAR_XWDTPS_0_DEVICE_ID
#define WDT_EXPIRE_TIME 100
#define WDT_CRV_SHIFT   12

XWdtPs Watchdog;

static int InitWatchDog(void);
static void ErrorLockdown(void);

#if FSBL_BOOT
static void RegisterHandlers(void);
#endif

int main(void)
{
  uint32_t status;

  /*
   * Unlock SLCR for SLCR register write
   */
  SlcrUnlock();

#if FSBL_BOOT
  /*
   * PCW initialization for MIO,PLL,CLK and DDR
   */
  status = ps7_init();
  if (status != PS7_INIT_SUCCESS)
  {
    printf("error: PS7 init_fail : %s\r\n", getPS7MessageInfo(status));
    ErrorLockdown();
  }

#if DCACHE_ICACHE_ENABLE
  Xil_DCacheEnable();
  Xil_ICacheEnable();
#else
  Xil_DCacheDisable();
#endif

  /*
   * Register the Exception handlers
   */
  RegisterHandlers();
#else
  Xil_DCacheDisable();
#if DCACHE_ICACHE_ENABLE
  Xil_DCacheEnable();
  Xil_ICacheEnable();
#endif
#endif

  /*
   * Initialize the Watchdog Timer so that it is ready to use
   */
  status = InitWatchDog();
  if (status != XST_SUCCESS)
  {
    printf("error: watchdog init fail\n");
    ErrorLockdown();
  }

  FlareBoot(Xil_In32(BOOT_MODE_REG) & BOOT_MODES_MASK);

  ErrorLockdown();

  return 0;
}

void cache_flush_invalidate(void)
{
#if DCACHE_ICACHE_ENABLE
  Xil_DCacheFlush();
  Xil_ICacheInvalidate();
#endif
}

void cache_disable(void)
{
#if DCACHE_ICACHE_ENABLE
  Xil_DCacheDisable();
  Xil_ICacheDisable();
#endif
}

static void ErrorLockdown(void)
{
  flare_DataSafe_RequestFactoryBoot();

  printf("Boot failure. Reset to Factory Boot mode ... \r\r\r\r");

  /*
   * Wait for the UART to finish sending data.
   */
  while ((XUartPs_ReadReg(STDOUT_BASEADDRESS, XUARTPS_SR_OFFSET) & XUARTPS_SR_TXEMPTY) == 0);

  flare_wdog_control(true);

  /*
   * Reset PS, so Boot ROM will restart.
   */
  while (true)
  {
    /*
     * Barrier for synchronization
     */
    asm("dsb\n\t"
        "isb");
  }
}

static uint32_t ConvertTime_WdtCounter(uint32_t seconds)
{
	double   time = 0.0;
	double   CounterValue;
	uint32_t Crv = 0;
	uint32_t Prescaler,PrescalerValue;

	Prescaler = XWdtPs_GetControlValue(&Watchdog, XWDTPS_CLK_PRESCALE);

	if (Prescaler == XWDTPS_CCR_PSCALE_0008)
		PrescalerValue = 8;
	if (Prescaler == XWDTPS_CCR_PSCALE_0064)
		PrescalerValue = 64;
	if (Prescaler == XWDTPS_CCR_PSCALE_4096)
		PrescalerValue = 4096;

	time = (double)(PrescalerValue) / (double)XPAR_PS7_WDT_0_WDT_CLK_FREQ_HZ;

	CounterValue = seconds / time;

	Crv = (u32)CounterValue;
	Crv >>= WDT_CRV_SHIFT;

	return Crv;
}

static int InitWatchDog(void)
{
  uint32_t       Status = XST_SUCCESS;
  XWdtPs_Config *ConfigPtr;   /* Config structure of the WatchDog Timer */
  uint32_t       CounterValue = 1;

  ConfigPtr = XWdtPs_LookupConfig(WDT_DEVICE_ID);
  Status = XWdtPs_CfgInitialize(&Watchdog,
        ConfigPtr,
        ConfigPtr->BaseAddress);
  if (Status != XST_SUCCESS) {
    printf("Watchdog Driver init Failed \n\r");
    return XST_FAILURE;
  }

  /*
   * Setting the divider value
   */
  XWdtPs_SetControlValue(&Watchdog,
      XWDTPS_CLK_PRESCALE,
      XWDTPS_CCR_PSCALE_4096);
  /*
   * Convert time to  Watchdog counter reset value
   */
  CounterValue = ConvertTime_WdtCounter(WDT_EXPIRE_TIME);

  /*
   * Set the Watchdog counter reset value
   */
  XWdtPs_SetControlValue(&Watchdog,
      XWDTPS_COUNTER_RESET,
      CounterValue);
  /*
   * enable reset output, as we are only using this as a basic counter
   */
  XWdtPs_EnableOutput(&Watchdog, XWDTPS_RESET_SIGNAL);
  XWdtPs_Stop(&Watchdog);

  return XST_SUCCESS;
}

void WatchDogStop(void)
{
  XWdtPs_Stop(&Watchdog);
}

#if FSBL_BOOT

static void traceError(const char* label)
{
  volatile uint32_t* stack;
  int                i;
  stack = (volatile uint32_t*) &stack;
  printf("Fatal: %s: sp:%08x", label, stack);
  for (i = 0; i < 64; ++i)
  {
    if ((i & 0x7) == 0)
      printf("\n ");
    printf("%08x ", stack[i]);
  }
  printf("\n ");
}

static void Undef_Handler (void)
{
  traceError("UNDEFINED_HANDLER\n");
  ErrorLockdown ();
}

static void SVC_Handler (void)
{
  traceError("SVC_HANDLER\n");
  ErrorLockdown ();
}

static void PreFetch_Abort_Handler (void)
{
  traceError("PREFETCH_ABORT_HANDLER\n");
  ErrorLockdown ();
}

static void Data_Abort_Handler (void)
{

  traceError("DATA_ABORT_HANDLER\n");
  ErrorLockdown ();
}

static void IRQ_Handler (void)
{
  traceError("IRQ_HANDLER\n");
  ErrorLockdown ();
}

static void FIQ_Handler (void)
{
  traceError("FIQ_HANDLER\n");
  ErrorLockdown ();
}

static void RegisterHandlers(void)
{
  Xil_ExceptionInit();

   /*
   * Initialize the vector table. Register the stub Handler for each
   * exception.
   */
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_UNDEFINED_INT,
                               (Xil_ExceptionHandler)Undef_Handler,
                               NULL);
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_SWI_INT,
                               (Xil_ExceptionHandler)SVC_Handler,
                               NULL);
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_PREFETCH_ABORT_INT,
                               (Xil_ExceptionHandler)PreFetch_Abort_Handler,
                               NULL);
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_DATA_ABORT_INT,
                               (Xil_ExceptionHandler)Data_Abort_Handler,
                               (void *) 0);
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
                               (Xil_ExceptionHandler)IRQ_Handler,(void *)
                               NULL);
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_FIQ_INT,
                               (Xil_ExceptionHandler)FIQ_Handler,
                               NULL);
  Xil_ExceptionEnable();
}

#endif
