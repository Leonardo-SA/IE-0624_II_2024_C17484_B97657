//C17484_B97657

#include <stdint.h>
#include <math.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include "clock.h"
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"
#include "spi-mems.c"
#include <stdio.h>
#include <string.h>

// Global Variables
uint8_t usart_enabled = 0; // Flag for USART communication
uint8_t battery_warning; // Flag for battery warning status

// Constants for Gyroscope configuration
#define GYRO_READ_WRITE (1 << 7) /* Write operation when zero */
#define GYRO_REG_CTRL1  0x20
#define GYRO_REG_CTRL1_PD (1 << 3) // Power down mode
#define GYRO_REG_CTRL1_XEN (1 << 1) // Enable X-axis
#define GYRO_REG_CTRL1_YEN (1 << 0) // Enable Y-axis
#define GYRO_REG_CTRL1_ZEN (1 << 2) // Enable Z-axis
#define GYRO_REG_CTRL1_BW_SHIFT 4   // Shift for bandwidth control
#define GYRO_REG_CTRL4  0x23
#define GYRO_REG_CTRL4_FS_SHIFT 4   // Shift for full-scale selection
#define GYRO_OUT_X_L    0x28
#define GYRO_OUT_X_H    0x29
#define GYRO_OUT_Y_L    0x2A
#define GYRO_OUT_Y_H    0x2B
#define GYRO_OUT_Z_L    0x2C
#define GYRO_OUT_Z_H    0x2D
#define GYRO_SENSITIVITY_250DPS (0.00875F) // Sensitivity for 250 degrees/sec

// Function to send strings over USART
static void usart_send_string(const char *string_data) {
    for(int i = 0; string_data[i] != '\0'; i++) {
        uint16_t data_to_send = (uint16_t)string_data[i];
        usart_send_blocking(USART1, data_to_send); // Send character
    }
}

// Button setup function with interrupt
static void configure_button(void) {
    rcc_periph_clock_enable(RCC_GPIOA); // Enable clock for GPIOA
    rcc_periph_clock_enable(RCC_GPIOG); // Enable clock for GPIOG
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0); // Configure button as input
    gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13 | GPIO14); // Configure LEDs as output
    rcc_periph_clock_enable(RCC_SYSCFG); // Enable system configuration controller
    exti_select_source(EXTI0, GPIOA); // Set EXTI0 source to GPIOA
    exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING); // Trigger on falling edge
    exti_enable_request(EXTI0); // Enable EXTI0 interrupt request
    nvic_enable_irq(NVIC_EXTI0_IRQ); // Enable EXTI0 interrupt in NVIC
}

// Interrupt handler for button to toggle USART communication
void exti0_isr(void) {
    if (exti_get_flag_status(EXTI0)) {
        usart_enabled = !usart_enabled;  // Toggle USART enable flag
        exti_reset_request(EXTI0); // Clear the interrupt flag
    }
}

int main(void) {
    clock_setup(); // Setup system clock
    console_setup(115200); // Setup console with baud rate 115200
    spi_setup(); // Initialize SPI for communication
    uint16_t adc_input;
    adc_setup(); // Initialize ADC
    sdram_init(); // Initialize SDRAM
    lcd_spi_init(); // Initialize LCD over SPI
    configure_button(); // Configure button with EXTI interrupt

    // Variables for gyro and battery readings
    int16_t gyro_x, gyro_y, gyro_z;
    char gyro_x_str[15], gyro_y_str[15], gyro_z_str[15], battery_str[15];
    int battery_voltage;

    // Initialize LCD display
    gfx_init(lcd_draw_pixel, 240, 320); // Initialize graphics for LCD
    gfx_fillScreen(LCD_BLACK); // Clear screen to black

    while (1) {
        // Read X-axis gyro low and high bytes
        gpio_clear(GPIOC, GPIO1); // Select the gyro chip
        spi_send(SPI5, GYRO_OUT_X_L | GYRO_READ_WRITE); // Request low byte
        spi_read(SPI5); // Dummy read
        spi_send(SPI5, 0); // Send 0 to clock data out
        gyro_x = spi_read(SPI5); // Read data
        gpio_set(GPIOC, GPIO1); // Deselect chip

        gpio_clear(GPIOC, GPIO1); // Select gyro chip
        spi_send(SPI5, GYRO_OUT_X_H | GYRO_READ_WRITE); // Request high byte
        spi_read(SPI5); // Dummy read
        spi_send(SPI5, 0); // Send 0 to clock data out
        gyro_x |= spi_read(SPI5) << 8; // Combine low and high bytes
        gpio_set(GPIOC, GPIO1); // Deselect chip

        // Repeat reading for Y-axis
        gpio_clear(GPIOC, GPIO1);
        spi_send(SPI5, GYRO_OUT_Y_L | GYRO_READ_WRITE);
        spi_read(SPI5);
        spi_send(SPI5, 0);
        gyro_y = spi_read(SPI5);
        gpio_set(GPIOC, GPIO1);

        gpio_clear(GPIOC, GPIO1);
        spi_send(SPI5, GYRO_OUT_Y_H | GYRO_READ_WRITE);
        spi_read(SPI5);
        spi_send(SPI5, 0);
        gyro_y |= spi_read(SPI5) << 8;
        gpio_set(GPIOC, GPIO1);

        // Repeat reading for Z-axis
        gpio_clear(GPIOC, GPIO1);
        spi_send(SPI5, GYRO_OUT_Z_L | GYRO_READ_WRITE);
        spi_read(SPI5);
        spi_send(SPI5, 0);
        gyro_z = spi_read(SPI5);
        gpio_set(GPIOC, GPIO1);

        gpio_clear(GPIOC, GPIO1);
        spi_send(SPI5, GYRO_OUT_Z_H | GYRO_READ_WRITE);
        spi_read(SPI5);
        spi_send(SPI5, 0);
        gyro_z |= spi_read(SPI5) << 8;
        gpio_set(GPIOC, GPIO1);

        // Scale gyro readings based on sensitivity
        gyro_x = gyro_x * GYRO_SENSITIVITY_250DPS;
        gyro_y = gyro_y * GYRO_SENSITIVITY_250DPS;
        gyro_z = gyro_z * GYRO_SENSITIVITY_250DPS;

        // Read battery voltage
        adc_input = read_adc_naiive(3); // Read ADC channel 3
        battery_voltage = (adc_input * 9) / 4095; // Scale reading

        // Format readings for display and USART
        sprintf(gyro_x_str, "X: %d", gyro_x);
        sprintf(gyro_y_str, "Y: %d", gyro_y);
        sprintf(gyro_z_str, "Z: %d", gyro_z);
        sprintf(battery_str, "BATT: %d", battery_voltage);

        // Display data on LCD
        gfx_fillRoundRect(0, 0, LCD_WIDTH, LCD_HEIGHT, 0, LCD_WHITE); // Clear display
        gfx_setTextColor(LCD_BLACK, LCD_WHITE);
        gfx_setTextSize(2);
        gfx_setCursor(15, 25);
        gfx_puts("SEISMOGRAPH");
        gfx_setTextSize(1);
        gfx_setTextColor(LCD_BLUE, LCD_WHITE);
        gfx_setCursor(15, 55);
        gfx_puts("C17484_B97657");
        gfx_setTextColor(LCD_BLACK, LCD_WHITE);
        gfx_setTextSize(2);
        gfx_setCursor(15, 95);
        gfx_puts(gyro_x_str);
        gfx_setCursor(15, 135);
        gfx_puts(gyro_y_str);
        gfx_setCursor(15, 175);
        gfx_puts(gyro_z_str);
        gfx_setCursor(15, 240);
        gfx_puts(battery_str);
        gfx_setCursor(15, 270);
        if (usart_enabled) {
            gfx_puts("USART: ON");
        } else {
            gfx_puts("USART: OFF");
            gpio_clear(GPIOG, GPIO13); // Turn off status LED
        }
        lcd_show_frame();

        // Toggle warning LED based on battery level
        if (battery_voltage < 7.5) {
            gpio_toggle(GPIOG, GPIO14); // Low battery warning
        } else {
            gpio_clear(GPIOG, GPIO14); // Turn off warning LED
        }

        // Send data over USART if enabled
        if (usart_enabled) {
            gpio_toggle(GPIOG, GPIO13); // Toggle status LED
            usart_send_string(gyro_x_str);
            usart_send_blocking(USART1, ',');
            usart_send_string(gyro_y_str);
            usart_send_blocking(USART1, ',');
            usart_send_string(gyro_z_str);
            usart_send_blocking(USART1, ',');
            usart_send_string(battery_str);
            usart_send_blocking(USART1, ',');
            usart_send_blocking(USART1, '\r');
            usart_send_blocking(USART1, '\n');
        }

        msleep(250); // Delay 250 ms
    }

    return 0;
}

