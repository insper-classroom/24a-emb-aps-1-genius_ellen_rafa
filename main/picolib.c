#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "picolib.h"

void buzzer(double freq, double duration, int pin) {
    double t = 1000000/freq;
    for (int i=0; i<duration*freq; i++) {
        gpio_put(pin, 1);
        sleep_us(t/2);
        gpio_put(pin, 0);
        sleep_us(t/2);
    }
}

int set_rand_seed() {
    int seed = to_us_since_boot(get_absolute_time());
    srand(seed);
    return seed;
}