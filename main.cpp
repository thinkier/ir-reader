#include <stdio.h>
#include <iostream>
#include <string>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


using namespace std;

#define PWR 18
#define DATA 17

#define FREQ (38000)

int main() {
    stdio_init_all();
    gpio_init(DATA);
    gpio_set_dir(DATA, GPIO_IN);
    gpio_pull_up(DATA);
#ifdef PWR
    gpio_init(PWR);
    gpio_set_dir(PWR, GPIO_OUT);
    gpio_put(PWR, 1);
#endif

    while (!stdio_usb_connected()) {
    }
    cout << "--- Ready ---" << endl;

    for (;;) {
        string buf;
        absolute_time_t start = get_absolute_time();
        absolute_time_t first_t;
        double tick = 0;
        uint16_t z = 0;

        while (buf.length() < 16384 && z < 2048) {
            sleep_until(delayed_by_us(start, (uint64_t) ((++tick * 1e6) / FREQ)));

            bool v = gpio_get(DATA) == 0;
            bool e = buf.empty();
            if (v || !e) {
                if (e) {
                    first_t = get_absolute_time();
                }
                buf.push_back(v ? '1' : '0');
            }

            if (!e) {
                if (v) {
                    z = 0;
                } else {
                    z++;
                }
            }
        }
        cout << "--- New IR Transmission ---" << endl;
        cout << buf << endl;
        cout << "--- End IR Transmission ---" << endl;
    }
}
