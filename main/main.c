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
volatile bool timer_fired = false;

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

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    timer_fired = true;
    return 0;
};

// Estados <=========================================================================

void main_state(int  *round_max, int *current_round, int *total_round, const int sequence[MAX_SEQUENCE], alarm_id_t *alarm_ptr, bool *active_alarm) {
    int pressed_color = 4;
    
    if (red_pressed){
        if (*active_alarm){
            cancel_alarm(*alarm_ptr);
            *active_alarm = false;
        }
        use_color(RED);
        red_pressed = 0;
        pressed_color = RED;
    } 
    if (yellow_pressed) {
        if (*active_alarm){
            cancel_alarm(*alarm_ptr);
            *active_alarm = false;
        }
        use_color(YELLOW);
        yellow_pressed = 0;
        pressed_color = YELLOW;
    }
    if (blue_pressed){
        if (*active_alarm){
            cancel_alarm(*alarm_ptr);
            *active_alarm = false;
        }
        use_color(BLUE);
        blue_pressed = 0;
        pressed_color = BLUE;
    }
    if (green_pressed){
        if (*active_alarm){
            cancel_alarm(*alarm_ptr);
            *active_alarm = false;
        }
        use_color(GREEN);
        green_pressed = 0;
        pressed_color = GREEN;
    }

    if (pressed_color == sequence[*current_round]) {
        *current_round += 1;
        if (*current_round == *round_max) {
            right_choice();
            *total_round += 1;
            *round_max += 1;
            *current_round = 0;
            state = 2;
        }
    } else if (pressed_color != 4 || timer_fired){
        wrong_choice(alarm_ptr, active_alarm);
        show_score(total_round);
        timer_fired = 0;
        state = 0;
    }
}

void instruction_state(int round_max, const int sequence[MAX_SEQUENCE], alarm_id_t *alarm_ptr, bool *active_alarm) {
    sleep_ms(500);
    for (int i=0; i < round_max; i++) {
        use_color(sequence[i]);
        sleep_ms(250);
    }
    *alarm_ptr = add_alarm_in_ms(60000, alarm_callback, NULL, false);
    *active_alarm = true;
    state = 1;
}

void init_state(int *round_max, int *current_round, int *total_round, int sequence[MAX_SEQUENCE]) {
    *round_max = 3;
    *current_round = 0;
    *total_round = 0;

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
    int total_round = 0;
    int current_round = 0;
    int sequence[MAX_SEQUENCE];
    bool active_alarm = false;
    alarm_id_t alarm;

    while (true) {
        switch (state) {
            case 0:
                idle_state();
                break;
            case 1:
                main_state(&round_max, &current_round, &total_round, sequence, &alarm, &active_alarm);
                break;
            case 2:
                instruction_state(round_max, sequence, &alarm, &active_alarm);
                break;
            case 3:
                init_state(&round_max, &current_round, &total_round, sequence);
                break;
            default:
                idle_state();
                break;
        }
    }
}
