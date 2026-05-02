#pragma once
#include <Arduino.h>

// Cross-platform task delay function
inline void task_delay(uint32_t ms) {
  delay(ms);
}
