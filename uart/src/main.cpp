// file:     main.cpp (uart)
// brief:    VIHAL UART Test
// created:  2021-10-03
// authors:  nvitya

#include "platform.h"
//#include "hwclkctrl.h"
#include "cppinit.h"
#include "clockcnt.h"

#include "hwclk.h"
#include "hwpins.h"
#include "hwuart.h"
#include "traces.h"

THwUart   conuart;  // console uart

#if defined(BOARD_VRV100_103) || defined(BOARD_VRV100_104) || defined(BOARD_VRV100_241) \
    || defined(BOARD_VRV100_403) || defined(BOARD_VRV100_441)|| defined(BOARD_VRV100_443) || defined(BOARD_VRV100_543)

#include "spiflash.h"
#include "hwspi.h"
#include "self_flashing.h"

TGpioPin  pin_led1(PORTNUM_A, 0, false);
THwSpi    spi;
TSpiFlash spiflash;

#define LED_COUNT 1

void setup_board()
{
	pin_led1.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);

	conuart.Init(1); // UART1

  spi.speed = 10000000;
  spi.Init(1); // flash

  spiflash.spi = &spi;
  spiflash.has4kerase = false; // warning some ECP devices does not have 4k erase !
  spiflash.Init();
}

#endif

#if defined(BOARD_LONGAN_NANO)

TGpioPin  pin_led1(PORTNUM_C, 13, true);
TGpioPin  pin_led2(PORTNUM_A,  1, true);
TGpioPin  pin_led3(PORTNUM_A,  2, true);

#define LED_COUNT 3

void setup_board()
{
  pin_led1.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);
  pin_led2.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);
  pin_led3.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);

  hwpinctrl.PinSetup(PORTNUM_A,  9, PINCFG_OUTPUT | PINCFG_AF_0);
  hwpinctrl.PinSetup(PORTNUM_A, 10, PINCFG_INPUT  | PINCFG_AF_0);
  conuart.Init(0); // USART0
}

#endif

#if defined(BOARD_MIN_F103)

TGpioPin  pin_led1(2, 13, false); // PC13

void setup_board()
{
  pin_led1.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);

  // USART1
  hwpinctrl.PinSetup(PORTNUM_A,  9,  PINCFG_OUTPUT | PINCFG_AF_0);  // USART1_TX
  hwpinctrl.PinSetup(PORTNUM_A, 10,  PINCFG_INPUT  | PINCFG_AF_0 | PINCFG_PULLUP);  // USART1_RX
  conuart.Init(1);
}
#endif


#ifndef LED_COUNT
  #define LED_COUNT 1
#endif

extern "C" __attribute__((noreturn)) void _start(unsigned self_flashing)  // self_flashing = 1: self-flashing required for RAM-loaded applications
{
  // after ram setup and region copy the cpu jumps here, with probably RC oscillator

  // Set the interrupt vector table offset, so that the interrupts and exceptions work
  mcu_init_vector_table();

  // run the C/C++ initialization (variable initializations, constructors)
  cppinit();

#if defined(MCU_FIXED_SPEED)

  SystemCoreClock = MCU_FIXED_SPEED;

#else

  if (!hwclk_init(EXTERNAL_XTAL_HZ, MCU_CLOCK_SPEED))  // if the EXTERNAL_XTAL_HZ == 0, then the internal RC oscillator will be used
  {
    while (1)
    {
      // error
    }
  }

#endif

	mcu_enable_fpu();    // enable coprocessor if present
	mcu_enable_icache(); // enable instruction cache if present

	clockcnt_init();

	// go on with the hardware initializations
	setup_board();

	TRACE("\r\n--------------------------------------\r\n");
	TRACE("Hello From VIHAL !\r\n");
	TRACE("Board: %s\r\n", BOARD_NAME);

#if SELF_FLASHING

  if (spiflash.initialized)
  {
    TRACE("SPI Flash ID CODE: %08X, size = %u\r\n", spiflash.idcode, spiflash.bytesize);
  }
  else
  {
    TRACE("Error initializing SPI Flash !\r\n");
  }

  if (self_flashing)
  {
    spi_self_flashing(&spiflash);
  }
#endif

	mcu_enable_interrupts();

	unsigned hbclocks = SystemCoreClock / 20;  // start blinking fast
	unsigned hbcounter = 0;

	unsigned t0, t1;

	t0 = CLOCKCNT;

	//volatile uint32_t *  hexnum = (volatile uint32_t *)0xF1000000;

	// Infinite loop
	while (1)
	{
		t1 = CLOCKCNT;

		if (t1-t0 > hbclocks)
		{
			++hbcounter;

			//*hexnum = hbcounter;

      pin_led1.SetTo(hbcounter & 1);
      #if LED_COUNT > 1
        pin_led2.SetTo((hbcounter >> 1) & 1);
      #endif
      #if LED_COUNT > 2
        pin_led3.SetTo((hbcounter >> 2) & 1);
      #endif

			TRACE("hbcounter=%u\r\n", hbcounter);

			t0 = t1;

			if (hbcounter > 20)  hbclocks = SystemCoreClock / 2;  // slow down to 0.5 s
		}
	}
}

// ----------------------------------------------------------------------------
