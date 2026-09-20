

/*
Tein tehtävän ja sen kaikki annetut lisätehtävät, joten periaatteessa tavoittelen täyttä pistemäärää. Toki palautus on myöhässä.
Tekijä: Lauri Knuuttila
*/

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <stdlib.h>

// Led pin configurations
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
// static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
volatile int led_status = 0; // 0: red, 1: green, 2: yellow

int  init_led();
int init_button();
#define STACKSIZE 500
#define PRIORITY 5
void red_led_task(void *, void *, void*);
void green_led_task(void *, void *, void*);
void yellow_led_task(void *, void *, void*);
K_THREAD_DEFINE(red_thread,STACKSIZE,red_led_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(green_thread,STACKSIZE,green_led_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(yellow_thread,STACKSIZE,yellow_led_task,NULL,NULL,NULL,PRIORITY,0,0);
//prototyypit napeille
void button_0_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void button_1_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void button_2_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void button_3_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void button_4_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
//nappien määritys
#define BUTTON_0 DT_ALIAS(sw0)
#define BUTTON_1 DT_ALIAS(sw1)
#define BUTTON_2 DT_ALIAS(sw2)
#define BUTTON_3 DT_ALIAS(sw3)
#define BUTTON_4 DT_ALIAS(sw4)
// UART initialization
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

// Create dispatcher FIFO buffer
K_FIFO_DEFINE(dispatcher_fifo);
K_FIFO_DEFINE(red_fifo);
K_FIFO_DEFINE(yellow_fifo);
K_FIFO_DEFINE(green_fifo);

K_SEM_DEFINE(led_done, 1, 1);

// FIFO dispatcher data type
struct data_t {
	/*************************
	// Add fifo_reserved below
	*************************/
	void *fifo_reserved;
	char msg[20];
};

/********************
 * init UART
 */
int init_uart(void) {
	// UART initialization
	if (!device_is_ready(uart_dev)) {
		return 1;
	} 
	return 0;
}



// Main program
int main(void)
{
	init_led();

    	int ret = init_uart();
	if (ret != 0) {
		printk("UART initialization failed!\n");
		return ret;
	}

	return 0;
}

// Initialize leds
int  init_led() {
    int ret;

    ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error: Red led configure failed\n");
        return ret;
    }

    ret = gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error: Green led configure failed\n");
        return ret;
    }

    gpio_pin_set_dt(&red, 0);
    gpio_pin_set_dt(&green, 0);

    printk("Leds initialized ok\n");

    return 0;
}

/********************
 * UART task
 */
static void uart_task(void *unused1, void *unused2, void *unused3)
{
	// Received character from UART
	char rc=0;
	// Message from UART
	char uart_msg[20];
	memset(uart_msg,0,20);
	int uart_msg_cnt = 0;

	while (true) {
		// Ask UART if data available
		if (uart_poll_in(uart_dev,&rc) == 0) {
			printk("Received: %c\n",rc);
			// If character is not newline, add to UART message buffer
			if (rc != '\r') {
				uart_msg[uart_msg_cnt] = rc;
				uart_msg_cnt++;
			// Character is newline, copy dispatcher data and put to FIFO buffer
			} else {
				printk("UART msg: %s\n", uart_msg);
                
				struct data_t *buf = k_malloc(sizeof(struct data_t));
				if (buf == NULL) {
					return;
				}
				// Copy UART message to dispatcher data
				// strncpy(buf->msg, 20, uart_msg); // mitä ihmettä, miksi kaatuu!!
				snprintf(buf->msg, 20, "%s", uart_msg);

				// You need to:
				// Put dispatcher data to FIFO buffer
                k_fifo_put(&dispatcher_fifo, buf);
				// Clear UART receive buffer
				uart_msg_cnt = 0;
				memset(uart_msg,0,20);

				// Clear UART message buffer
				uart_msg_cnt = 0;
				memset(uart_msg,0,20);
			}
		}
		k_msleep(10);
	}
	return;
}

/********************
 * Dispatcher task
 */
static void dispatcher_task(void *unused1, void *unused2, void *unused3)
{
    while (true) {

        struct data_t *rec_item =
            k_fifo_get(&dispatcher_fifo, K_FOREVER);

        char sequence[20];
        memcpy(sequence, rec_item->msg, 20);
        sequence[19] = '\0';

        k_free(rec_item);

        // Erotellaan väri ja aika
        char color = sequence[0];
        int time = atoi(sequence + 2);

        printk("Data: %c %d\n", color, time);

        k_sem_take(&led_done, K_FOREVER);

        struct data_t *buf = k_malloc(sizeof(struct data_t));


        if (buf == NULL) {
            k_sem_give(&led_done);
            continue;
        }

        snprintf(buf->msg, sizeof(buf->msg), "%d", time);

        if (color == 'R') {
            k_fifo_put(&red_fifo, buf);
        }
        else if (color == 'Y') {
            k_fifo_put(&yellow_fifo, buf);
        }
        else if (color == 'G') {
            k_fifo_put(&green_fifo, buf);
        }
        else {
            printk("Unknown color: %c\n", color);
            k_free(buf);
            k_sem_give(&led_done);
        }
    }
}

// Task to handle red led
void red_led_task(void *, void *, void*)
{
    while (true) {

        struct data_t *data =
            k_fifo_get(&red_fifo, K_FOREVER);

        int time = atoi(data->msg);

        printk("RED: %d ms\n", time);

        k_free(data);

        gpio_pin_set_dt(&red, 1);
        k_msleep(time);
        gpio_pin_set_dt(&red, 0);

        k_sem_give(&led_done);
    }
}
void green_led_task(void *, void *, void*) {
	
	printk("Green led thread started\n");
	while (true) {
        struct data_t *data =
            k_fifo_get(&green_fifo, K_FOREVER);

        int time = atoi(data->msg);

		gpio_pin_set_dt(&green,1);
		k_msleep(time);
		gpio_pin_set_dt(&green,0);

        k_free(data);
        
        k_sem_give(&led_done);
        }
    }

void yellow_led_task(void *, void *, void*) {
	
	printk("Yellow led thread started\n");
	while (true) {
        struct data_t *data =
            k_fifo_get(&yellow_fifo, K_FOREVER);

        int time = atoi(data->msg);

		gpio_pin_set_dt(&red,1);
        gpio_pin_set_dt(&green,1);

        k_msleep(time);

		gpio_pin_set_dt(&red,0);
        gpio_pin_set_dt(&green,0);

        k_free(data);

        k_sem_give(&led_done);
        }

}

void button_0_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (led_status == 4) {
        led_status = 0;
    } else {
        led_status = 4; // pause
        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);
    }
}

void button_1_handler(const struct device *dev,struct gpio_callback *cb,uint32_t pins){
    printk("Button 1 pressed\n");

    if (led_status == 5) {
        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);
        led_status = 0;
    }
    else {
        led_status = 5;

        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);

        gpio_pin_set_dt(&red, 1);
    }
}

void button_2_handler(const struct device *dev,struct gpio_callback *cb,uint32_t pins){
    printk("Button 2 pressed\n");

    if (led_status == 6) {
        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);
        led_status = 0;
    }
    else {
        led_status = 6;

        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);

        gpio_pin_set_dt(&green, 1);
    }
}

void button_3_handler(const struct device *dev,struct gpio_callback *cb,uint32_t pins){
    printk("Button 3 pressed\n");

    if (led_status == 7) {
        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);
        led_status = 0;
    }
    else {
        led_status = 7;

        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);

        gpio_pin_set_dt(&red, 1);
        gpio_pin_set_dt(&green, 1);
    }
}

void button_4_handler(const struct device *dev,struct gpio_callback *cb,uint32_t pins){
    printk("Button 4 pressed\n");
    if (led_status == 8) {
        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);
        led_status = 0;
    }
    else {
        led_status = 8;
    }
}


K_THREAD_DEFINE(dis_thread,STACKSIZE,dispatcher_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(uart_thread,STACKSIZE,uart_task,NULL,NULL,NULL,PRIORITY,0,0);