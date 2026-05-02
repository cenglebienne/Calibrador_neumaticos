#pragma once

// Identidad / branding
#ifndef EMPRESA
  #define EMPRESA            "FARINA VENDING"
#endif

#ifndef TELEFONO
  #define TELEFONO           "223-5218985"
#endif
#ifndef VERSION_FIRMWARE
  #define VERSION_FIRMWARE   FW_VERSION      // viene del .ini
#endif

// Web / FS
#ifndef WEB_INDEX
  #define WEB_INDEX          "/index.html"
#endif

// LCD
#ifndef LCD_COLS
  #define LCD_COLS           16
#endif
#ifndef LCD_ROWS
  #define LCD_ROWS           2
#endif

// AP por defecto (cuando no hay WiFi cliente válido)
#ifndef AP_SSID_DEFAULT
  #define AP_SSID_DEFAULT    "FarinaVending"
#endif
#ifndef AP_PASS_DEFAULT
  #define AP_PASS_DEFAULT    "expendedoras"
#endif
#ifndef RED_WIFI
    #define RED_WIFI "Farina"
#endif
#ifndef CLAVE_WIFI
    #define CLAVE_WIFI "Lacar2025"
#endif
// Máxima cantidad de APs a detectar en el scan
const int MAX_CANTIDAD_AP_DETECTAR = 20;

