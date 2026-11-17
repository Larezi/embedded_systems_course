*//
tehtävä tehty 2 pisteen arvoisesti. tehty esimerkkiohjelma syööttämään arvoja UART:in kautta 
(testattu esimerkkikoodien pohjalta sekä omaan ohjelmaan tehdyillä koodeilla)
lisäksi tehty lisätestejä pohjautuen virehekoodeihin joita lisättiin aiemmassa tehtävässä
*//

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include "TimeParser.h"

#define STACKSIZE 500
#define PRIORITY 5
int uart_task(void *unused1, void *unused2, void *unused3);
// UART initialization
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

K_THREAD_DEFINE(uart_thread,STACKSIZE,uart_task,NULL, NULL, NULL,PRIORITY,0,0);

int main(void)
{
	// UART initialization
	if (!device_is_ready(uart_dev)) {
		printk("UART device not found!\n");
		return 0;
	}
	// Wait for everything to initialize and threads to start
	k_msleep(100);
	// Sanity check
	printk("Started serial led example\n");
	return 0;
}

int uart_task(void *unused1, void *unused2, void *unused3)
{
	char rc=0;
	char uart_msg[20];
	int uart_msg_cnt=0;
	memset(uart_msg,0,20);			
	while (true) {
		// Ask UART if data available
		while (uart_poll_in(uart_dev,&rc) == 0) {
			// Add characters into buffer until X is received 
			if (rc != 'X') {
				uart_msg[uart_msg_cnt] = rc;
				uart_msg_cnt++;
			} else {
				// Send response to UART
				// X has been removed, so we need to add it to the end
				// printk("-1X"); example return error
				// Example check with time_parse
				int timer_delay = time_parse(uart_msg); 
				printk("%dX\n",timer_delay);

				// Clear UART message buffer
				uart_msg_cnt = 0;
				memset(uart_msg,0,20);			
			}
		}
		k_msleep(10);
	}
}