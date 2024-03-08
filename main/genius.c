#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "picolib.h"
#include "genius.h"

const int LED_LIST[4] = { LED_RED_PIN, LED_YELLOW_PIN, LED_GREEN_PIN, LED_BLUE_PIN };
const int BTN_LIST[4] = { BTN_RED_PIN, BTN_YELLOW_PIN, BTN_GREEN_PIN, BTN_BLUE_PIN };


void gpio_init_all(gpio_irq_callback_t gpio_callback) {
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    gpio_init(BTN_START_PIN);
    gpio_set_dir(BTN_START_PIN, GPIO_IN);
    gpio_pull_up(BTN_START_PIN);
    gpio_set_irq_enabled_with_callback(BTN_START_PIN, GPIO_IRQ_EDGE_FALL, true, gpio_callback);

    for (int i=0; i < 4; i++){
        gpio_init(BTN_LIST[i]);
        gpio_set_dir(BTN_LIST[i], GPIO_IN);
        gpio_pull_up(BTN_LIST[i]);

        gpio_set_irq_enabled(BTN_LIST[i], GPIO_IRQ_EDGE_FALL , true);

        gpio_init(LED_LIST[i]);
        gpio_set_dir(LED_LIST[i], GPIO_OUT);
    }
}

void idle_state() {
    for (int i=0; i<4; i++) {
        gpio_put(LED_LIST[i], 1);
        sleep_ms(250);
        gpio_put(LED_LIST[i], 0);
    }
}

void use_color(int color_id) {
    const int FREQ_LIST[4] = { 250, 500, 750, 1000 };

    gpio_put(LED_LIST[color_id], 1);
    buzzer(FREQ_LIST[color_id], TIME, BUZZER_PIN);
    gpio_put(LED_LIST[color_id], 0);
}

void wrong_choice() {
    const int WRONG_LIST[4] = { 100, 200, 300, 400 };

    sleep_ms(500);
    for (int i=3; i >= 0; i--) {
        gpio_put(LED_LIST[i], 1);
        buzzer(WRONG_LIST[i], TIME, BUZZER_PIN);
        gpio_put(LED_LIST[i], 0);
    }
};

void right_choice() {
    const int RIGHT_LIST[4] = { 750, 1000, 1250, 1500 };
    
    sleep_ms(500);
    for (int i=0; i < 4; i++) {
        gpio_put(LED_LIST[i], 1);
    }
    for (int i=0; i<4; i++){
        buzzer(RIGHT_LIST[i], TIME, BUZZER_PIN);
    }
    for (int i=0; i < 4; i++) {
        gpio_put(LED_LIST[i], 0);
    }
}