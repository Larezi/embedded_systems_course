#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

// Led pin configurations
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
// static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
volatile int led_status = 0; // 0: red, 1: green, 2: yellow
// Red led thread initialization
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