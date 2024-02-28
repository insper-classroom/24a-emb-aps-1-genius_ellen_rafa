/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const int BUZZER_PIN = 28;
const int BOTAO_VERMELHO = 21;
const int BOTAO_VERDE = 27;
const int BOTAO_AZUL = 17;
const int BOTAO_AMARELO = 19;

const int LED_VERMELHO = 20;
const int LED_VERDE = 26;
const int LED_AZUL = 16;
const int LED_AMARELO = 18;

volatile int vermelho = 0;
volatile int verde = 0;
volatile int azul = 0;
volatile int amarelo = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (gpio == BOTAO_VERMELHO){
        vermelho = 1;
    } else if (gpio == BOTAO_AMARELO){
        amarelo = 1;
    } else if (gpio==BOTAO_AZUL){
        azul = 1;
    } else if (gpio==BOTAO_VERDE){
        verde = 1;
    }
};

void onda(double frequencia, double tempo, double pino){
    double t = (1/frequencia)*1000000;
    for (int i=0; i<tempo*frequencia; i++){
        gpio_put(pino, 1);
        sleep_us(t/2);
        gpio_put(pino, 0);
        sleep_us(t/2);
    }
}

int main() {
    // mudar seed sempre que o jogo se inicia. o seed pode ser ao ligar a plaquinha
    // pico sdk
    stdio_init_all();
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    int leds[4] = {LED_VERMELHO, LED_VERDE, LED_AZUL, LED_AMARELO};
    int botoes[4] = {BOTAO_VERMELHO, BOTAO_VERDE, BOTAO_AZUL, BOTAO_AMARELO};
    for (int i=0; i<4; i++){
        gpio_init(botoes[i]);
        gpio_set_dir(botoes[i], GPIO_IN);
        gpio_pull_up(botoes[i]);

        if (i==0){
            gpio_set_irq_enabled_with_callback(botoes[i], GPIO_IRQ_EDGE_FALL, true, &btn_callback);
        } else {
            gpio_set_irq_enabled(botoes[i], GPIO_IRQ_EDGE_FALL , true);
        }

        gpio_init(leds[i]);
        gpio_set_dir(leds[i], GPIO_OUT);
    }

    while (true) {
        if (vermelho){
            gpio_put(LED_VERMELHO,1);
            onda(250,1,BUZZER_PIN);
            gpio_put(LED_VERMELHO,0);

            vermelho = 0;
        } else if(amarelo) {
            gpio_put(LED_AMARELO,1);
            onda(500,1,BUZZER_PIN);
            gpio_put(LED_AMARELO,0);
            amarelo = 0;
        } else if(azul){
            gpio_put(LED_AZUL,1);
            onda(750,1,BUZZER_PIN);
            gpio_put(LED_AZUL,0);
            azul = 0;
        } else if(verde){
            gpio_put(LED_VERDE,1);
            onda(1000,1,BUZZER_PIN);
            gpio_put(LED_VERDE,0);
            verde = 0;
        }
        
    }
}
