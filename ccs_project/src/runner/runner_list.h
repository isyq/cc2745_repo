#ifndef RUNNER_H
#define RUNNER_H

void runner_gpio_create_task(void);
void runner_log_create_task(void);
void runner_uart_create_task(void);
void runner_iic_create_task(void);

#endif
