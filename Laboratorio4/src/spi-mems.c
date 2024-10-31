/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2013 Chuck McManis <cmcmanis@mcmanis.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * SPI Port example
 */

#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include "clock.h"
#include "console.h"
#include <libopencm3/stm32/adc.h>

/*
 * Functions defined for accessing the SPI port 8 bits at a time
 */
static uint16_t read_reg(int reg) __attribute__((unused));
static uint8_t read_xyz(int16_t vecs[3]) __attribute__((unused));
static void spi_setup(void) __attribute__((unused));
static int print_decimal_xyz(int num) __attribute__((unused));
static void adc_setup(void) __attribute__((unused));
static int adc_update(void) __attribute__((unused));
void spi_setup(void)
{
    rcc_periph_clock_enable(RCC_SPI5);
    rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOF);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);
    gpio_set(GPIOC, GPIO1);
    gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO7 | GPIO8 | GPIO9);   
	gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO8 | GPIO9);
    spi_set_master_mode(SPI5);
	spi_set_baudrate_prescaler(SPI5, SPI_CR1_BR_FPCLK_DIV_64);
	spi_set_clock_polarity_0(SPI5);
	spi_set_clock_phase_0(SPI5);
	spi_set_full_duplex_mode(SPI5);
	spi_set_unidirectional_mode(SPI5);
	spi_enable_software_slave_management(SPI5);
	spi_send_msb_first(SPI5);
	spi_set_nss_high(SPI5);
    SPI_I2SCFGR(SPI5) &= ~SPI_I2SCFGR_I2SMOD;
	spi_enable(SPI5);
	/* Enable GPIOG clock. */
	rcc_periph_clock_enable(RCC_GPIOG);
	/* Set GPIO13 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13 | GPIO14);
}


/*
 * read_reg(int reg)
 *
 * This reads the MEMs registers. The chip registers are 16 bits
 * wide, but I read it as two 8 bit bytes. Originally I tried
 * swapping between an 8 bit and 16 bit wide bus but that confused
 * both my code and the chip after a while so this was found to
 * be a more stable solution.
 */
static uint16_t
read_reg(int reg)
{
	uint16_t d1, d2;
	d1 = 0x80 | (reg & 0x3f); /* Read operation */
	/* Nominallly a register read is a 16 bit operation */
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, d1);
	d2 = spi_read(SPI5);
	d2 <<= 8;
	/*
	 * You have to send as many bits as you want to read
	 * so we send another 8 bits to get the rest of the
	 * register.
	 */
	spi_send(SPI5, 0);
	d2 |= spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);
	return d2;
}

/*
 * uint8_t status = read_xyz(int16_t [])
 *
 * This function exploits the fact that you can do a read +
 * auto increment of the SPI registers. It starts at the
 * address of the X register and reads 6 bytes.
 *
 * Then the status register is read and returned.
 */
static uint8_t
read_xyz(int16_t vecs[3])
{
	uint8_t	 buf[7];
	int		 i;
	gpio_clear(GPIOC, GPIO1); /* CS* select */
	spi_send(SPI5, 0xc0 | 0x28);
	(void) spi_read(SPI5);
	for (i = 0; i < 6; i++) {
		spi_send(SPI5, 0);
		buf[i] = spi_read(SPI5);
	}
	gpio_set(GPIOC, GPIO1); /* CS* deselect */
	vecs[0] = (buf[1] << 8 | buf[0]);
	vecs[1] = (buf[3] << 8 | buf[2]);
	vecs[2] = (buf[5] << 8 | buf[4]);
	return read_reg(0x27); /* Status register */
}

static int print_decimal_xyz(int num)
{
	int		ndx = 0;
	char	buf[10];
	int		len = 0;
	char	is_signed = 0;
	if (num < 0) {
		is_signed++;
		num = 0 - num;
	}
	buf[ndx++] = '\000';
	do {
		buf[ndx++] = (num % 10) + '0';
	num = num / 10;
	} while (num != 0);
	ndx--;
	if (is_signed != 0) {
		console_putc('-');
		len++;
	}
	while (buf[ndx] != '\000') {
		console_putc(buf[ndx--]);
		len++;
	}
	return len; /* number of characters printed */
}

//Functions to manage the ADC communication
static void adc_setup(void)
{
	rcc_periph_clock_enable(RCC_ADC1);
  	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
	adc_power_off(ADC1);
  	adc_disable_scan_mode(ADC1);
  	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);
	adc_power_on(ADC1);
}
static uint16_t read_adc_naiive(uint8_t channel)
{
	uint8_t channel_array[16];
	channel_array[0] = channel;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	uint16_t reg16 = adc_read_regular(ADC1);
	return reg16;
}
static int adc_update(void){
	float bat = read_adc_naiive(1);
	return bat;
}