#include "config.h"
#include "config_common.h"
#include "constants.h"
#include "app_state.h"
#include <Preferences.h>



bool boot_load_config(Config& out) {
  Preferences prefs;
  if (!prefs.begin("My_Flash", true)) return false;
  out.display_use_i2c = prefs.getBool("Display_I2C", true);
  out.wifi_mode_ap    = prefs.getBool("Levantar_AP",  false); 
  out.validador       = prefs.getInt("Validador", 3);
  prefs.end();
  const int idx = constrain(out.validador, 1, 3) - 1;  // Aseguramos índice 0..2
  out.min_pulse = PULSE_MIN[idx];
  out.max_pulse = PULSE_MAX[idx];
  minimoPulso = out.min_pulse;
  maximoPulso = out.max_pulse;
  return true; 
}