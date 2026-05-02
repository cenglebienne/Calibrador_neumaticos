#include "pulsadores.h"
#include "pins.h"
#include <Arduino.h>

bool PP = false;
bool P1 = false;
bool P2 = false;

bool P_Prog_On = false;
bool resultado = false;
bool buttonState = false;

long lastDebounceTime_P;
long lastDebounceTime_P1;
long lastDebounceTime_P2;

long debounceDelay = 50;

bool PushProg()
{
  static bool ProgAnterior;
  static bool buttonState;
  bool resultado = false;
  buttonState = digitalRead(Puls_prog);
  if ((millis() - lastDebounceTime_P) > debounceDelay)
  {
    if (!buttonState)
    {
      ProgAnterior = true;
      lastDebounceTime_P = millis(); // set the current time
    }
    else if ((buttonState) && (ProgAnterior))
    {
     
      resultado = true;
      ProgAnterior = false;
      lastDebounceTime_P = millis(); // set the current time
    }
  }
  return resultado;
}

bool Push_P1() {
  static bool P1_Anterior;
  bool resultado = false;
  bool buttonState = digitalRead(Pin_Pulsador_1);

  if ((millis() - lastDebounceTime_P1) > debounceDelay) {
    if (buttonState == LOW) {
      P1_Anterior = true;
      lastDebounceTime_P1 = millis();
    } else if ((buttonState == HIGH) && (P1_Anterior)) {
      resultado = true;
      P1_Anterior = false;
      lastDebounceTime_P1 = millis();
    }
  }
  return resultado;
}

bool Push_P2() {

  static bool P2_Anterior;
  bool resultado = false;
  bool buttonState = digitalRead(Pin_Pulsador_2);

  if ((millis() - lastDebounceTime_P2) > debounceDelay) {
    if (buttonState == LOW) {
      P2_Anterior = true;
      lastDebounceTime_P2 = millis();
    } else if ((buttonState == HIGH) && (P2_Anterior)) {
      resultado = true;
      P2_Anterior = false;
      lastDebounceTime_P2 = millis();
    }
  }
  return resultado;
}



// ── Variante acelerada (para seteo de presión) ───────────────────────────────
// Dispara al presionar y repite mientras se mantiene con velocidad creciente:
//   0–1 s sostenido  →  cada 400 ms  (~2,5 pasos/s)
//   1–3 s sostenido  →  cada 150 ms  (~6,5 pasos/s)
//   > 3 s sostenido  →  cada  50 ms  (~20  pasos/s)

static bool pushAccel(int pin,
                      bool          &held,
                      unsigned long &t_press,
                      unsigned long &t_last_repeat)
{
  bool pressed = (digitalRead(pin) == LOW);
  unsigned long now = millis();

  if (pressed) {
    if (!held) {
      held = true;
      t_press       = now;
      t_last_repeat = now;
      return true;          // disparo inmediato al primer contacto
    }
    unsigned long hold   = now - t_press;
    unsigned long intervalo = hold < 1000UL ? 400UL :
                              hold < 3000UL ? 150UL : 50UL;
    if (now - t_last_repeat >= intervalo) {
      t_last_repeat = now;
      return true;
    }
  } else {
    held = false;
  }
  return false;
}

bool Push_P1_accel() {
  static bool held = false;
  static unsigned long t_press = 0, t_last_repeat = 0;
  return pushAccel(Pin_Pulsador_1, held, t_press, t_last_repeat);
}

bool Push_P2_accel() {
  static bool held = false;
  static unsigned long t_press = 0, t_last_repeat = 0;
  return pushAccel(Pin_Pulsador_2, held, t_press, t_last_repeat);
}

