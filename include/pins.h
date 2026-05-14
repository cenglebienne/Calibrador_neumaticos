#pragma once


const int pin_moneda=35;
const int Puls_prog=4;
const int Pin_Pulsador_1= 23;
const int Pin_Pulsador_2= 14;
const int PIN_HABILITACION= 15 ;
const int Pin_Sensor= 34;
const int Pin_Buzzer= 5 ;
const int Valvula_Inflado = 33;
const int Valvula_Desinflado = 32;

const int Data_Pin_1 = 27;
const int Clock_Pin_1 = 26;

const int Data_Pin_2 = 25;
const int Clock_Pin_2 =13;
const int BUSY_PIN = 18;


#ifndef I2C_SDA_PIN
  #define I2C_SDA_PIN  21
#endif
#ifndef I2C_SCL_PIN
  #define I2C_SCL_PIN  22
#endif

#ifndef LCD_I2C_ADDR
  #define LCD_I2C_ADDR 0x27
#endif



#ifndef LCD_COLS
#define LCD_COLS 16
#endif

#ifndef LCD_ROWS
#define LCD_ROWS 2
#endif





