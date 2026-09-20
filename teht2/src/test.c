

/*
Tein tehtävän ja sen kaikki annetut lisätehtävät, joten periaatteessa tavoittelen täyttä pistemäärää. Toki palautus on myöhässä.
Tekijä: Lauri Knuuttila
*/

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>

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

static const struct gpio_dt_spec button_0 =
    GPIO_DT_SPEC_GET_OR(BUTTON_0, gpios, {0});

static const struct gpio_dt_spec button_1 =
    GPIO_DT_SPEC_GET_OR(BUTTON_1, gpios, {0});

static const struct gpio_dt_spec button_2 =
    GPIO_DT_SPEC_GET_OR(BUTTON_2, gpios, {0});

static const struct gpio_dt_spec button_3 =
    GPIO_DT_SPEC_GET_OR(BUTTON_3, gpios, {0});

static const struct gpio_dt_spec button_4 =
    GPIO_DT_SPEC_GET_OR(BUTTON_4, gpios, {0});

static struct gpio_callback button_0_data;
static struct gpio_callback button_1_data;
static struct gpio_callback button_2_data;
static struct gpio_callback button_3_data;
static struct gpio_callback button_4_data;


// Main program
int main(void)
{
	init_led();
	init_button();

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
				//snprintf(buf->msg, 20, "%s", uart_msg);

				// You need to:
				// Put dispatcher data to FIFO buffer

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
		// Receive dispatcher data from uart_task fifo
		struct data_t *rec_item = k_fifo_get(&dispatcher_fifo, K_FOREVER);
		char sequence[20];
		memcpy(sequence,rec_item->msg,20);
		k_free(rec_item);

		printk("Dispatcher: %s\n", sequence);
        // You need to:
        // Parse color and time from the fifo data
        // Example
        //    char color = sequence[0];
        //    int time = atoi(sequence+2);
		//    printk("Data: %c %d\n", color, time);
        // Send the parsed color information to tasks using fifo
        // Use release signal to control sequence or k_yield
	}
}

// Task to handle red led
void red_led_task(void *, void *, void*) {
	
	printk("Red led thread started\n");
	while (true) {
        if(led_status == 0){
		// 1. set led on 
		gpio_pin_set_dt(&red,1);
		printk("Red on\n");
		// 2. sleep for 2 seconds
		k_sleep(K_SECONDS(1));
        if(led_status == 0){
		// 3. set led off
		gpio_pin_set_dt(&red,0);
		printk("Red off\n");
        if (led_status == 0) {
            led_status = 1;
        }
        }
        }
	}
}
void green_led_task(void *, void *, void*) {
	
	printk("Green led thread started\n");
	while (true) {
        if(led_status == 2){
		// 1. set led on 
		gpio_pin_set_dt(&green,1);
		printk("Green on\n");
		// 2. sleep for 2 seconds
		k_sleep(K_SECONDS(1));
        if(led_status == 2){
		// 3. set led off
		gpio_pin_set_dt(&green,0);
		printk("Green off\n");
        if (led_status == 2) {
            led_status = 0;
        }
        }
        }
	}
}

void yellow_led_task(void *, void *, void*) {
	
	printk("Yellow led thread started\n");
	while (true) {
        if(led_status == 1){
		// 1. set led on 
		gpio_pin_set_dt(&red,1);
        gpio_pin_set_dt(&green,1);
		printk("yellow on\n");
		// 2. sleep for 2 seconds
		k_sleep(K_SECONDS(1));
        if(led_status ==1){
		// 3. set led off
		gpio_pin_set_dt(&red,0);
        gpio_pin_set_dt(&green,0);
		printk("yellow off\n");
        if (led_status == 1) {
            led_status = 2;
        }
        }
        }
        if(led_status == 8){
        gpio_pin_set_dt(&red,1);
        gpio_pin_set_dt(&green,1);
        k_sleep(K_SECONDS(1));
        if(led_status == 8){
        gpio_pin_set_dt(&red,0);
        gpio_pin_set_dt(&green,0);
        k_sleep(K_SECONDS(1));
        }
        }
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


int init_button(void)
{
    int ret;

    // Button 0
    if (!gpio_is_ready_dt(&button_0)) {
        printk("Error: button 0 is not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_0, GPIO_INPUT);
    if (ret != 0) {
        printk("Error: failed to configure button 0 pin\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button_0, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error: failed to configure button 0 interrupt\n");
        return -1;
    }

    gpio_init_callback(&button_0_data, button_0_handler, BIT(button_0.pin));
    gpio_add_callback(button_0.port, &button_0_data);
    printk("Set up button 0 ok\n");


    // Button 1
    if (!gpio_is_ready_dt(&button_1)) {
        printk("Error: button 1 is not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_1, GPIO_INPUT);
    if (ret != 0) {
        printk("Error: failed to configure button 1 pin\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button_1, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error: failed to configure button 1 interrupt\n");
        return -1;
    }

    gpio_init_callback(&button_1_data, button_1_handler, BIT(button_1.pin));
    gpio_add_callback(button_1.port, &button_1_data);
    printk("Set up button 1 ok\n");


    // Button 2
    if (!gpio_is_ready_dt(&button_2)) {
        printk("Error: button 2 is not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_2, GPIO_INPUT);
    if (ret != 0) {
        printk("Error: failed to configure button 2 pin\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button_2, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error: failed to configure button 2 interrupt\n");
        return -1;
    }

    gpio_init_callback(&button_2_data, button_2_handler, BIT(button_2.pin));
    gpio_add_callback(button_2.port, &button_2_data);
    printk("Set up button 2 ok\n");


    // Button 3
    if (!gpio_is_ready_dt(&button_3)) {
        printk("Error: button 3 is not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_3, GPIO_INPUT);
    if (ret != 0) {
        printk("Error: failed to configure button 3 pin\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button_3, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error: failed to configure button 3 interrupt\n");
        return -1;
    }

    gpio_init_callback(&button_3_data, button_3_handler, BIT(button_3.pin));
    gpio_add_callback(button_3.port, &button_3_data);
    printk("Set up button 3 ok\n");


    // Button 4
    if (!gpio_is_ready_dt(&button_4)) {
        printk("Error: button 4 is not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_4, GPIO_INPUT);
    if (ret != 0) {
        printk("Error: failed to configure button 4 pin\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button_4, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error: failed to configure button 4 interrupt\n");
        return -1;
    }

    gpio_init_callback(&button_4_data, button_4_handler, BIT(button_4.pin));
    gpio_add_callback(button_4.port, &button_4_data);
    printk("Set up button 4 ok\n");

    return 0;
}

K_THREAD_DEFINE(dis_thread,STACKSIZE,dispatcher_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(uart_thread,STACKSIZE,uart_task,NULL,NULL,NULL,PRIORITY,0,0);