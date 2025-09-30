#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "App.hpp"
#include "Constants.hpp"

int main() {
    App::instance().init();

    // a. Welcome
    {
        const char* s = "Welcome to CS527: Computer Systems \nLet's have fun\n";
        char buf[128];
        std::snprintf(buf, sizeof(buf), "%s", s);
        App::instance().write(DISPLAY_BASE, buf, static_cast<unsigned>(std::strlen(buf) + 1));
    }

    for (;;) {
        // b. Ask user to write a random number
        {
            const char* s = "Write a random number";
            char buf[64];
            std::snprintf(buf, sizeof(buf), "%s", s);
            App::instance().write(DISPLAY_BASE, buf, static_cast<unsigned>(std::strlen(buf) + 1));
        }

        // c. Read number from keyboard
        char in[KEYBOARD_MEM_BYTES] = {0};
        App::instance().read(KEYBOARD_BASE, in, sizeof(in));
        int value = std::atoi(in);

        if (value < 24) {
            // d. Read push button
            char btnbuf[2] = {0};
            App::instance().read(BUTTON_ADDR, btnbuf, sizeof(btnbuf));
            unsigned char btn = static_cast<unsigned char>(btnbuf[0]);
            if (btn == 1) {
                // i. write to alarm with number received
                char out[1];
                out[0] = static_cast<char>(value & 0xFF);
                App::instance().write(ALARM_ADDR, out, 1);
                break; // done
            } else {
                // ii. back to 'b'
                continue;
            }
        } else {
            // e. back to 'b'
            continue;
        }
    }

    return 0;
}
