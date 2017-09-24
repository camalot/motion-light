#ifndef Timer_h
#define Timer_h

#include "CallbackFunction.h"

class Timer {
public:
    Timer();
    void every_n_milliseconds(unsigned long v);
    void every_n_seconds(unsigned long v);
    void every_n_minutes(unsigned long v);
    void start();
    void check(CallbackFunction callback);
    void stop();
    bool isRunning();
private:
    unsigned long max_delay;
    unsigned long last_set;
    bool running;
};

#endif