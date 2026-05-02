#include "display_hal.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "pins.h"

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

void display_begin(bool /*useI2C*/) {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void display_clear() { lcd.clear(); }

void display_clearRow(uint8_t row) {
    lcd.setCursor(0, row);
    lcd.print("                ");
}

void display_setCursor(uint8_t c, uint8_t r) { lcd.setCursor(c, r); }
void display_print(const char* s)            { if (s) lcd.print(s); }
void display_print_int(int v)                { lcd.print(v); }

void display_center(const char* s, uint8_t row) {
    if (!s) return;
    int len = (int)strlen(s);
    int start = (LCD_COLS - len) / 2;
    if (start < 0) start = 0;
    lcd.setCursor(0, row);
    lcd.print("                ");
    lcd.setCursor(start, row);
    lcd.print(s);
}

void display_warn_other(bool, const char*, const char*) {}
