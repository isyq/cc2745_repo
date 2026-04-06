#ifndef HAL_DRV_API_H
#define HAL_DRV_API_H

#include <stdint.h>

/* See GPIO.h for input pull option definitions. */
#define HAL_GPIO_IN_PULL_NONE 0
#define HAL_GPIO_IN_PULL_UP   1
#define HAL_GPIO_IN_PULL_DOWN 2

/* See GPIO.h for interrupt option definitions.
 * 
 * NOTE: LOW and HIGH level interrupt types are not supported for F3 low power devices. 
 */
#define HAL_GPIO_INT_NONE    0
#define HAL_GPIO_INT_FALLING 1
#define HAL_GPIO_INT_RISING  2
#define HAL_GPIO_INT_BOTH    3

/* See GPIO.h for output pull option definitions. 
 * 
 * TODO: Add GPIO strength options
 */
#define HAL_GPIO_OUT_PULL_STD     0
#define HAL_GPIO_OUT_PULL_OD_NONE 1
#define HAL_GPIO_OUT_PULL_OD_UP   2
#define HAL_GPIO_OUT_PULL_OD_DOWN 3

typedef void (*hal_gpio_isr_fn)(uint8_t pin);
typedef void (*hal_uart_isr_fn)(uint8_t* p_data, uint16_t data_len);

void hal_gpio_init(void);
void hal_gpio_create_in_pin(uint8_t pin, uint8_t pull, uint8_t int_type, hal_gpio_isr_fn isr_fn);
void hal_gpio_create_out_pin(uint8_t pin, uint8_t pull, uint8_t level);
void hal_gpio_enable_interrupt(uint8_t pin);
uint8_t hal_gpio_read_pin(uint8_t pin);
void hal_gpio_write_pin(uint8_t pin, uint8_t level);
void hal_gpio_toggle_pin(uint8_t pin);

void hal_uart_init(uint8_t inst, uint32_t baud_rate, hal_uart_isr_fn isr_fn);
uint16_t hal_uart_send(uint8_t inst, uint8_t* p_data, uint16_t data_len);
uint16_t hal_uart_receive(uint8_t inst, uint8_t* p_data, uint16_t data_len);


#endif
