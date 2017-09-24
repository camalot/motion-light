#include "Timer.h"

Timer::Timer(void) {
    max_delay = 3600000UL; // default 1 hour
    running = false;
}

void Timer::every_n_milliseconds(unsigned long v) {
    max_delay = v;
    start();
}

void Timer::every_n_seconds(unsigned long v) {
    every_n_milliseconds(v * 1000UL);
}

void Timer::every_n_minutes(unsigned long v) {
    every_n_seconds(v * 60UL);
}


void Timer::start() {
    running = true;
    last_set = millis();
}

void Timer::check(CallbackFunction callback) {
    if(last_set == -1) {
        running = false;
        return;
    }

    unsigned long now = millis();
    if (now - last_set > max_delay) {
        last_set = now;
        callback();
    }
}

void Timer::stop() {
    last_set = -1;
    running = false;
}

bool Timer::isRunning() {
    return running;
}