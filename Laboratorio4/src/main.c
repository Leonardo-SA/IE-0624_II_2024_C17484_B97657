#include <stdint.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include "lcd-spi.h"
#include "gfx.h"

// Global flag for USART communication
uint8_t usart_enabled = 0;

// Gyroscope constants
#define GYRO_READ_WRITE (1 << 7)
#define GYRO_OUT_X_L    0x28
#define GYRO_OUT_X_H    0x29
#define GYRO_OUT_Y_L    0x2A
#define GYRO_OUT_Y_H    0x2B
#define GYRO_OUT_Z_L    0x2C
#define GYRO_OUT_Z_H    0x2D
#define GYRO_SENSITIVITY_250DPS (0.00875F)

void exti0_isr(void) {
    if (exti_get_flag_status(EXTI0)) {
        usart_enabled = !usart_enabled;
        exti_reset_request(EXTI0);
    }
}

static void configure_button(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
    exti_select_source(EXTI0, GPIOA);
    exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);
    exti_enable_request(EXTI0);
    nvic_enable_irq(NVIC_EXTI0_IRQ);
}

void read_gyro(int16_t *x, int16_t *y, int16_t *z) {
    gpio_clear(GPIOC, GPIO1);
    spi_send(SPI5, GYRO_OUT_X_L | GYRO_READ_WRITE);
    spi_read(SPI5);
    spi_send(SPI5, 0);
    *x = spi_read(SPI5);
    gpio_set(GPIOC, GPIO1);
    *x *= GYRO_SENSITIVITY_250DPS;
    
    gpio_clear(GPIOC, GPIO1);
    spi_send(SPI5, GYRO_OUT_Y_L | GYRO_READ_WRITE);
    spi_read(SPI5);
    spi_send(SPI5, 0);
    *y = spi_read(SPI5);
    gpio_set(GPIOC, GPIO1);
    *y *= GYRO_SENSITIVITY_250DPS;
    
    gpio_clear(GPIOC, GPIO1);
    spi_send(SPI5, GYRO_OUT_Z_L | GYRO_READ_WRITE);
    spi_read(SPI5);
    spi_send(SPI5, 0);
    *z = spi_read(SPI5);
    gpio_set(GPIOC, GPIO1);
    *z *= GYRO_SENSITIVITY_250DPS;
}

int main(void) {
    int16_t gyro_x, gyro_y, gyro_z;
    char gyro_x_str[15], gyro_y_str[15], gyro_z_str[15];
    configure_button();
    lcd_spi_init();
    gfx_init(lcd_draw_pixel, 240, 320);

    while (1) {
        read_gyro(&gyro_x, &gyro_y, &gyro_z);
        sprintf(gyro_x_str, "X: %d", gyro_x);
        sprintf(gyro_y_str, "Y: %d", gyro_y);
        sprintf(gyro_z_str, "Z: %d", gyro_z);

        gfx_fillScreen(LCD_WHITE);
        gfx_setTextSize(2);
        gfx_setCursor(10, 50);
        gfx_puts(gyro_x_str);
        gfx_setCursor(10, 90);
        gfx_puts(gyro_y_str);
        gfx_setCursor(10, 130);
        gfx_puts(gyro_z_str);
        lcd_show_frame();
    }

    return 0;
}
