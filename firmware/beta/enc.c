#include "enc.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "pin.h"
#include "state.h"

/////////////////////////////////////////////////////////////////////
// encoder daisy-chain setup
//
// spi4 cs   = pe11, af 5
// spi4 sclk = pe12, af 5
// spi4 miso = pe13, af 5
// spi4 mosi = pe14, af 5

#define PORTE_ENC_CS   11
#define PORTE_ENC_SCLK 12
#define PORTE_ENC_MISO 13
#define PORTE_ENC_MOSI 14

void enc_init()
{
  printf("enc_init()\r\n");
  RCC->APB2ENR |= RCC_APB2ENR_SPI4EN; // turn on SPI4

  pin_set_output(GPIOE, PORTE_ENC_CS);
  pin_set_output_level(GPIOE, PORTE_ENC_CS, 1);
  pin_set_alternate_function(GPIOE, PORTE_ENC_SCLK, 5);
  pin_set_alternate_function(GPIOE, PORTE_ENC_MISO, 5);
  pin_set_alternate_function(GPIOE, PORTE_ENC_MOSI, 5);

  // spi4 is running from a 84 MHz pclk. set it up with 
  // sclk = pclk/128 to be super slow for now.
  SPI4->CR1 = SPI_CR1_DFF  | // 16-bit mode
              SPI_CR1_BR_2 |
              SPI_CR1_BR_1 |
              SPI_CR1_MSTR | // master mode
              SPI_CR1_CPHA | // cpha=1, cpol=0 for AS5048A
              SPI_CR1_SSM  | // software slave-select mode
              SPI_CR1_SSI  |
              SPI_CR1_SPE;

  enc_poll(); // first one will be garbage
  enc_poll(); // and the second one
}

void enc_poll()
{
  GPIOE->BSRRH = 1 << PORTE_ENC_CS; // assert (pull down) CS
  for (volatile int i = 0; i < 10; i++) { } // needs at least 350 ns
  SPI4->DR; // clear the rx data register in case it has some garbage
  for (int i = 0; i < NUM_ENC; i++)
  {
    SPI4->DR = 0xffff; 
    while (!(SPI4->SR & SPI_SR_TXE)) { } // wait for buffer room
    while (!(SPI4->SR & SPI_SR_RXNE)) { }
    g_state.encoders[i] = SPI4->DR & 0x3fff;
  }
  for (volatile int i = 0; i < 1; i++) { } // needs at least 50 ns
  GPIOE->BSRRL = 1 << PORTE_ENC_CS; // de-assert (pull up) CS
  /*
  printf("       enc: %06d  %06d  %06d\r\n",
         g_state.encoders[0],
         g_state.encoders[1],
         g_state.encoders[2]);
  */
}

