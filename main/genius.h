// genius.h
#ifndef GENIUS_H
#define GENIUS_H

#define MAX_SEQUENCE 100
#define TIME 0.5

#define BUZZER_PIN 28
#define BTN_START_PIN 15

#define RED 0
#define BTN_RED_PIN 13
#define LED_RED_PIN 12

#define YELLOW 1
#define BTN_YELLOW_PIN 11
#define LED_YELLOW_PIN 10

#define GREEN 2
#define BTN_GREEN_PIN 21
#define LED_GREEN_PIN 20

#define BLUE 3
#define BTN_BLUE_PIN 17
#define LED_BLUE_PIN 16

void gpio_init_all(gpio_irq_callback_t gpio_callback);

void idle_state();

void use_color(int color_id);

void wrong_choice();

void show_score(int *pontuation);

void right_choice();

#endif