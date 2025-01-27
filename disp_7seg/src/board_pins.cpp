/*
 *  file:     board_pins.cpp (uart)
 *  brief:    UART Test Board pins
 *  version:  1.00
 *  date:     2021-10-29
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "hwuart.h"
#include "ledandkey.h"

THwUart   conuart;  // console uart
unsigned  pin_led_count = 1;

TGpioPin  pin_led[MAX_LEDS] =
{
  TGpioPin(),
  TGpioPin(),
  TGpioPin(),
  TGpioPin()
};

void board_pins_init_leds()
{
  for (unsigned n = 0; n < pin_led_count; ++n)
  {
    pin_led[n].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);
  }
}

#if 0 == DISP_SEL

  TLedAndKey  disp;

#elif 1 == DISP_SEL

  TTimeDisp_tm1637  disp;

#elif 2 == DISP_SEL

  TMax7219  disp;

#elif 3 == DISP_SEL

  Td7s_595  disp;

#else
  #error "Unhandled display type"
#endif


#if 0  // to use elif everywhere

#elif    defined(BOARD_MIN_F103) \
      || defined(BOARD_MIBO48_STM32F303) \
      || defined(BOARD_MIN_F401) \
      || defined(BOARD_MIN_F411) \
      || defined(BOARD_MIBO48_STM32G473)

void board_pins_init()
{
  #if defined(BOARD_MIBO48_STM32G473)
    // Turn off the USB-C power delivery pull down functionality on B6:
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
    PWR->CR3 |= PWR_CR3_UCPD_DBDIS;
  #endif

  pin_led_count = 1;
  pin_led[0].Assign(PORTNUM_C, 13, false);
  board_pins_init_leds();

  // USART1
  hwpinctrl.PinSetup(PORTNUM_A,  9,  PINCFG_OUTPUT | PINCFG_AF_7);  // USART1_TX
  hwpinctrl.PinSetup(PORTNUM_A, 10,  PINCFG_INPUT  | PINCFG_AF_7 | PINCFG_PULLUP);  // USART1_RX
  conuart.Init(1);

  #if 0 == DISP_SEL
    disp.controller.stb_pin.Assign(PORTNUM_B, 5, false);
    disp.controller.clk_pin.Assign(PORTNUM_B, 6, false);
    disp.controller.dio_pin.Assign(PORTNUM_B, 7, false);
    disp.Init();
  #elif 1 == DISP_SEL
    disp.controller.clk_pin.Assign(PORTNUM_B, 6, false);
    disp.controller.dio_pin.Assign(PORTNUM_B, 7, false);
    disp.Init();
  #elif 2 == DISP_SEL
    disp.dio_pin.Assign(PORTNUM_B, 5, false);
    disp.cs_pin.Assign( PORTNUM_B, 6, false);
    disp.clk_pin.Assign(PORTNUM_B, 7, false);
    disp.Init();
  #elif 3 == DISP_SEL
    disp.pin_sclk.Assign(PORTNUM_B, 5, false);
    disp.pin_rclk.Assign(PORTNUM_B, 6, false);
    disp.pin_din.Assign( PORTNUM_B, 7, false);
    disp.Init();
  #else
    #error "Unhandled display type"
  #endif
}

#else
  #error "Define board_pins_init here"
#endif

