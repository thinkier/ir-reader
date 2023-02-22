#include <stdio.h>
#include <iostream>
#include <string>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"


using namespace std;

#define PWR 17
#define DATA 18

#define PWM 13

#define SYS_CLK_FREQ (125000000)
#define FREQ (2500)

int main() {
    stdio_init_all();
#ifdef PWM
    gpio_set_function(PWM, GPIO_FUNC_PWM);

    pwm_config config = pwm_get_default_config();
    pwm_set_clkdiv_mode(6, PWM_DIV_FREE_RUNNING);
    pwm_set_clkdiv_int(6, SYS_CLK_FREQ / FREQ);

    pwm_init(6, &config, false);
    pwm_set_wrap(6, 1);
    pwm_set_chan_level(6, PWM_CHAN_B, 1);
    pwm_set_enabled(6, true);

#endif

    gpio_init(DATA);
    gpio_set_dir(DATA, GPIO_IN);
    gpio_pull_up(DATA);
#ifdef PWR
    gpio_init(PWR);
    gpio_set_dir(PWR, GPIO_OUT);
    gpio_put(PWR, true);
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

        while (buf.length() < 1024 && z < 128) {
            busy_wait_until(delayed_by_us(start, (uint64_t) ((++tick * 1e6) / FREQ)));

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
