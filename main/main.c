/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*/

#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "picolib.h"
#include "genius.h"

volatile int red_pressed = 0;
volatile int yellow_pressed = 0;
volatile int green_pressed = 0;
volatile int blue_pressed = 0;

volatile int state = 0;

// Funções <==========================================================================

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

// Estados <=========================================================================

void main_state(int  *round_max, int *current_round, const int sequence[MAX_SEQUENCE]) {
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

void instruction_state(int round_max, const int sequence[MAX_SEQUENCE]) {
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

    set_rand_seed();
    for (int i=0; i < MAX_SEQUENCE; i++){
        sequence[i] = rand() % 4; // números aleatórios entre 0 e 3
    }
    state = 2;
}

int main() {
    stdio_init_all();
    gpio_init_all(&gpio_callback);

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
