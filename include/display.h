#ifndef DISPLAY_H
#define DISPLAY_H

// 7-segment display (bit-bang shift register via Clock/Data pins)
#include "display_7seg.h"

// LCD I2C display HAL
#include "display_hal.h"

// Legacy: old-architecture files access the lcd object directly
#include <LiquidCrystal_I2C.h>
extern LiquidCrystal_I2C lcd;

#endif // DISPLAY_H
