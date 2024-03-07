/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdlib.h> // necessário p/ função rand e srand

#define MAX_SEQUENCE 100
#define TIME 0.5

const int BUZZER_PIN = 28;
const int BTN_START_PIN = 15;

const int RED = 0;
const int BTN_RED_PIN = 13;
const int LED_RED_PIN = 12;
volatile int red_pressed = 0;

const int YELLOW = 1;
const int BTN_YELLOW_PIN = 11;
const int LED_YELLOW_PIN = 10;
volatile int yellow_pressed = 0;

const int GREEN = 2;
const int BTN_GREEN_PIN = 21;
const int LED_GREEN_PIN = 20;
volatile int green_pressed = 0;

const int BLUE = 3;
const int BTN_BLUE_PIN = 17;
const int LED_BLUE_PIN = 16;
volatile int blue_pressed = 0;

const int LED_LIST[4] = { LED_RED_PIN, LED_YELLOW_PIN, LED_GREEN_PIN, LED_BLUE_PIN };
const int BTN_LIST[4] = { BTN_RED_PIN, BTN_YELLOW_PIN, BTN_GREEN_PIN, BTN_BLUE_PIN, };

volatile int state = 0;

// Funções <==========================================================================

uint64_t get_seed() {
     return to_us_since_boot(get_absolute_time());
}

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_START_PIN) {
        state = (state == 0) ? 3 : 0;
    } else if (state == 1) {
        if (gpio == BTN_RED_PIN){
            red_pressed = 1;
        } else if (gpio == BTN_YELLOW_PIN){
            yellow_pressed = 1;
        } else if (gpio == BTN_BLUE_PIN){
            blue_pressed = 1;
        } else if (gpio == BTN_GREEN_PIN){
            green_pressed = 1;
        }
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

void use_color(int color_id) {
    const int FREQ_LIST[4] = { 250, 500, 750, 1000 };

    gpio_put(LED_LIST[color_id], 1);
    buzzer(FREQ_LIST[color_id], TIME, BUZZER_PIN);
    gpio_put(LED_LIST[color_id], 0);
}

void wrong_choice(){
    const int WRONG_LIST[4] = { 100, 200, 300, 400 };

    sleep_ms(500);
    for (int i=3; i >= 0; i--) {
        gpio_put(LED_LIST[i], 1);
        buzzer(WRONG_LIST[i], TIME, BUZZER_PIN);
        gpio_put(LED_LIST[i], 0);
    }
};

void right_choice(){
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

// Estados <=========================================================================

void idle_state() {
    for (int i=0; i<4; i++) {
        gpio_put(LED_LIST[i], 1);
        sleep_ms(250);
        gpio_put(LED_LIST[i], 0);
    }
}

void main_state(int  *round_max, int *current_round, int sequence[MAX_SEQUENCE]) {
    int pressed_color = 4;
    if (red_pressed){
        use_color(RED);
        red_pressed = 0;
        pressed_color = RED;
    } 
    if (yellow_pressed) {
        use_color(YELLOW);
        yellow_pressed = 0;
        pressed_color = YELLOW;
    }
    if (blue_pressed){
        use_color(BLUE);
        blue_pressed = 0;
        pressed_color = BLUE;
    }
    if (green_pressed){
        use_color(GREEN);
        green_pressed = 0;
        pressed_color = GREEN;
    }

    if (pressed_color == sequence[*current_round]) {
        *current_round += 1;
        if (*current_round == *round_max) {
            right_choice();
            *round_max += 1;
            *current_round = 0;
            state = 2;
        }
    } else if (pressed_color != 4){
        wrong_choice();
        state = 0;
    }
}

void instruction_state(int round_max, int sequence[MAX_SEQUENCE]) {
    sleep_ms(500);
    for (int i=0; i < round_max; i++) {
        use_color(sequence[i]);
        sleep_ms(250);
    }
    state = 1;
}

void init_state(int *round_max, int *current_round, int sequence[MAX_SEQUENCE]) {
    *round_max = 3;
    *current_round = 0;

    red_pressed = 0;
    yellow_pressed = 0;
    blue_pressed = 0;
    green_pressed = 0;

    srand(get_seed());
    for (int i=0; i < MAX_SEQUENCE; i++){
        sequence[i] = rand() % 4; // números aleatórios entre 0 e 3
    }
    state = 2;
}

int main() {
    stdio_init_all();
    get_seed();

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

    int round_max = 3;
    int current_round = 0;
    int sequence[MAX_SEQUENCE];

    while (true) {
        switch (state) {
            case 0:
                idle_state();
                break;
            case 1:
                main_state(&round_max, &current_round, sequence);
                break;
            case 2:
                instruction_state(round_max, sequence);
                break;
            case 3:
                init_state(&round_max, &current_round, sequence);
                break;
            default:
                idle_state();
                break;
        }
    }
}
