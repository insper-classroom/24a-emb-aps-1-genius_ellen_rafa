/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const int BUZZER_PIN = 28;
const int BTN_START_PIN = 11;

const int BTN_RED_PIN = 21;
const int LED_RED_PIN = 20;
volatile int red_pressed = 0;

const int BTN_YELLOW_PIN = 19;
const int LED_YELLOW_PIN = 18;
volatile int yellow_pressed = 0;

const int BTN_BLUE_PIN = 17;
const int LED_BLUE_PIN = 16;
volatile int blue_pressed = 0;

const int BTN_GREEN_PIN = 27;
const int LED_GREEN_PIN = 26;
volatile int green_pressed = 0;

const double time = 0.5;
volatile int state = 0;
int led_index = 0;

const int LED_LIST[4] = { LED_RED_PIN, LED_YELLOW_PIN, LED_BLUE_PIN, LED_GREEN_PIN };
const int BTN_LIST[4] = { BTN_RED_PIN, BTN_YELLOW_PIN, BTN_BLUE_PIN, BTN_GREEN_PIN };

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_START_PIN) {
        if (state == 0) {
            // coloque a geração da lista aqui!
            state = 2;
        } else {
            state = 0;
        }
    } else if (gpio == BTN_RED_PIN){
        red_pressed = 1;
    } else if (gpio == BTN_YELLOW_PIN){
        yellow_pressed = 1;
    } else if (gpio == BTN_BLUE_PIN){
        blue_pressed = 1;
    } else if (gpio == BTN_GREEN_PIN){
        green_pressed = 1;
    }
};

void buzzer(double freq, double duration, int pin){
    double t = 1000000/freq;
    for (int i=0; i<duration*freq; i++){
        gpio_put(pin, 1);
        sleep_us(t/2);
        gpio_put(pin, 0);
        sleep_us(t/2);
    }
}

void idle_state() {
    gpio_put(LED_LIST[led_index%4], 1);
    sleep_ms(250);
    gpio_put(LED_LIST[led_index%4], 0);
    led_index++;
}

void main_state() {
    if (red_pressed){
        gpio_put(LED_RED_PIN,1);
        buzzer(250, time, BUZZER_PIN);
        gpio_put(LED_RED_PIN,0);

        red_pressed = 0;
    } 
    if (yellow_pressed) {
        gpio_put(LED_YELLOW_PIN,1);
        buzzer(500, time, BUZZER_PIN);
        gpio_put(LED_YELLOW_PIN,0);

        yellow_pressed = 0;
    }
    if (blue_pressed){
        gpio_put(LED_BLUE_PIN, 1);
        buzzer(750, time, BUZZER_PIN);
        gpio_put(LED_BLUE_PIN, 0);

        blue_pressed = 0;
    }
    if (green_pressed){
        gpio_put(LED_GREEN_PIN, 1);
        buzzer(1000, time, BUZZER_PIN);
        gpio_put(LED_GREEN_PIN, 0);

        green_pressed = 0;
    }
}

int main() {
    stdio_init_all();

    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    gpio_init(BTN_START_PIN);
    gpio_set_dir(BTN_START_PIN, GPIO_IN);
    gpio_pull_up(BTN_START_PIN);
    gpio_set_irq_enabled_with_callback(BTN_START_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    for (int i=0; i < 4; i++){
        gpio_init(BTN_LIST[i]);
        gpio_set_dir(BTN_LIST[i], GPIO_IN);
        gpio_pull_up(BTN_LIST[i]);

        gpio_set_irq_enabled(BTN_LIST[i], GPIO_IRQ_EDGE_FALL , true);

        gpio_init(LED_LIST[i]);
        gpio_set_dir(LED_LIST[i], GPIO_OUT);
    }

    while (true) {
        switch (state) {
            case 0:
                idle_state();
                break;
            case 1:
                main_state();
                break;
            default:
                idle_state();
                break;
        }
        
    }
}
