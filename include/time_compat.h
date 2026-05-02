#pragma once
#include <Arduino.h>
#include <time.h>

#if defined(ARDUINO_ARCH_ESP8266)

// ESP8266: Implement getLocalTimeCompat using standard C time functions
inline bool getLocalTimeCompat(struct tm* timeinfo, uint32_t timeout) {
  time_t now = time(nullptr);
  if (now < 1609459200) {  // 2021-01-01
    return false;
  }
  localtime_r(&now, timeinfo);
  return true;
}

#else

// ESP32: Use built-in getLocalTime()
inline bool getLocalTimeCompat(struct tm* timeinfo, uint32_t timeout) {
  return getLocalTime(timeinfo, timeout);
}

#endif
