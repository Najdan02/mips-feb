#include "keypad.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "gpio.h"

char keys[2];
unsigned temp_granica = 30;
int keysChanged = 1;
