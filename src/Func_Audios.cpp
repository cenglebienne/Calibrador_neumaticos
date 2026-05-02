#include <audio.h>
#include <Arduino.h>
#include "app_state.h"

HardwareSerial mySerial(1); // Usar UART1
DFRobotDFPlayerMini myDFPlayer;

const unsigned long MAX_WAIT_TIME = 7000; // Tiempo máximo de espera en milisegundos

unsigned long g_Tiempo_Entre_Audios = 10000;
unsigned long g_Tiempo_Ultimo_Audio = 0;

/* void reproducirMensaje(uint8_t trackNumber, unsigned long Espera)
{
  // Verificar si se debe intentar la reproducción
  Serial.println(trackNumber);
  if (!isDFPlayerAvailable)
  {
    Serial.println("Se canceló por fallo el reproductor");
    return;
  }

  if (Misvalores.Audio_Encendido)
  {
    unsigned long startTime = millis();
    // Esperar a que el pin BUSY esté en alto
    while (digitalRead(BUSY_PIN) == LOW)
    {
      if (millis() - startTime > MAX_WAIT_TIME)
      {
        // Si la espera excede el tiempo máximo, deshabilitar Reintento_Reproduccion
        isDFPlayerAvailable = false;
        Serial.println("Tiempo de espera excedido. Deshabilitando reintento de reproducción.");
        return;
      }
      Serial.println("Aguardo a que termine el anterior");
      delay(10); // Pequeño retraso para evitar bloquear completamente el CPU
    }

    // Comenzar la reproducción si el pin BUSY está en alto
    myDFPlayer.play(trackNumber);
    //Serial.print("Reproduciendo pista número: ");
    //Serial.println(trackNumber);
   // delay(Espera);
   // Serial.println("Reproducción finalizada.");
    while(millis() - startTime < Espera){
      if (digitalRead(BUSY_PIN) == HIGH) {
        Serial.print("La reproducción ha terminado");
        break;
      }
      delay(10); // Pequeño retraso para evitar bloquear completamente el CPU
      //Serial.println("Aguardo a que termine la reproducción");
    }

  }
  else
  {
    Serial.println("El Audio no está habilitado.");
  }
} */

 void reproducirMensaje(uint8_t trackNumber, unsigned long Espera)
{
  // Verificar si se debe intentar la reproducción
  if (!isDFPlayerAvailable)
  {
    Serial.println("Se canceló por fallo el reproductor");
    return;
  }

  if (Misvalores.Audio_Encendido)
  {
    unsigned long startTime = millis();
    // Esperar a que el pin BUSY esté en alto
    while (digitalRead(BUSY_PIN) == LOW)
    {
      if (millis() - startTime > MAX_WAIT_TIME)
      {
        // Si la espera excede el tiempo máximo, deshabilitar Reintento_Reproduccion
        isDFPlayerAvailable = false;
        Serial.println("Tiempo de espera excedido. Deshabilitando reintento de reproducción.");
        return;
      }
     // Serial.println("Aguardo a que termine el anterior");
      delay(10); // Pequeño retraso para evitar bloquear completamente el CPU
    }

    // Comenzar la reproducción si el pin BUSY está en alto
    myDFPlayer.play(trackNumber);
    Serial.print("Reproduciendo pista número: ");
    Serial.println(trackNumber);
    delay(Espera);
   // Serial.println("Reproducción finalizada.");
  }
  else
  {
    Serial.println("El Audio no está habilitado.");
  }
}
 
void testDeSonido()
{
  uint8_t numAudio;
  numAudio = 50;
  while (numAudio < 81)
  {
    reproducirMensaje(numAudio,500);
    numAudio++;
    delay(5000);
  }
}

/*    printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
      delay(100);

int state = myDFPlayer.readState(); // Leer el estado del DFPlayer inicialmente
    while (state != trackNumber)
    {
      delay(100);
        Serial.print(F("DFPlayer State: "));
        Serial.println(state);  // Imprime el estado del DFPlayer
      state = myDFPlayer.readState(); // Leer el estado del DFPlayer inicialmente
      x++;
      if (x==30) break;
    }
    Serial.println("El mensaje ha dejado de reproducirse");

 Serial.print(F("DFPlayer State: "));
  Serial.println(state);  // Imprime el estado del DFPlayer
  delay (50);

  state = myDFPlayer.readState(); // Leer el estado del DFPlayer
  Serial.print(F("DFPlayer State: "));
  Serial.println(state);  // Imprime el estado del DFPlayer
  delay (50);

  state = myDFPlayer.readState(); // Leer el estado del DFPlayer
  Serial.print(F("DFPlayer State: "));
  Serial.println(state);  // Imprime el estado del DFPlayer
  delay (50);

  state = myDFPlayer.readState(); // Leer el estado del DFPlayer
  Serial.print(F("DFPlayer State: "));
  Serial.println(state);  // Imprime el estado del DFPlayer
  delay (50); */

/*   while (state == 513) {
    Serial.println("El mensaje está reproduciéndose");
    delay(50);
    state = myDFPlayer.readState(); // Leer el estado del DFPlayer nuevamente
  }
   Serial.println("El mensaje ha dejado de reproducirse"); */

/*
else      // Serial.println("Playback is disabled or DFPlayer is unavailable.");
  return; // Salir si el audio está desactivado o el DFPlayer no está disponible

int state = myDFPlayer.readState(); // Leer el estado del DFPlayer
Serial.print(F("DFPlayer State: "));
Serial.println(state);  // Imprime el estado del DFPlayer
// printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
delay(100);
state = myDFPlayer.readState(); // Leer el estado del DFPlayer
Serial.print(F("DFPlayer State: "));
Serial.println(state);  // Imprime el estado del DFPlayer
// printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
delay(100);
state = myDFPlayer.readState(); // Leer el estado del DFPlayer
Serial.print(F("DFPlayer State: "));
Serial.println(state);  // Imprime el estado del DFPlayer
// printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
delay(25);
/*  x=0;
while (x<50){
  state = myDFPlayer.readState(); // Leer el estado del DFPlayer
   Serial.print(F("DFPlayer State: "));
   Serial.println(state);  // Imprime el estado del DFPlayer
   printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
  delay(100);
  x++;
}
}*/

/*void handleDFPlayerError()
{
  int16_t error = myDFPlayer.readState();
  Serial.print("DFPlayer Error: ");
  Serial.println(error);

  // Deshabilita futuras operaciones en cualquier error significativo
  isDFPlayerAvailable = false;
}

void printDetail(uint8_t type, int value)
{
  switch (type)
  {
  case TimeOut:
    Serial.println(F("Time Out!"));
    break;
  case WrongStack:
    Serial.println(F("Stack Wrong!"));
    break;
  case DFPlayerCardInserted:
    Serial.println(F("Card Inserted!"));
    break;
  case DFPlayerCardRemoved:
    Serial.println(F("Card Removed!"));
    break;
  case DFPlayerCardOnline:
    Serial.println(F("Card Online!"));
    break;
  case DFPlayerUSBInserted:
    Serial.println("USB Inserted!");
    break;
  case DFPlayerUSBRemoved:
    Serial.println("USB Removed!");
    break;
  case DFPlayerPlayFinished:
    Serial.print(F("Number:"));
    Serial.print(value);
    Serial.println(F(" Play Finished!"));
    break;
  case DFPlayerError:
    Serial.print(F("DFPlayerError:"));
    switch (value)
    {
    case Busy:
      Serial.println(F("Card not found"));
      break;
    case Sleeping:
      Serial.println(F("Sleeping"));
      break;
    case SerialWrongStack:
      Serial.println(F("Get Wrong Stack"));
      break;
    case CheckSumNotMatch:
      Serial.println(F("Check Sum Not Match"));
      break;
    case FileIndexOut:
      Serial.println(F("File Index Out of Bound"));
      break;
    case FileMismatch:
      Serial.println(F("Cannot Find File"));
      break;
    case Advertise:
      Serial.println(F("In Advertise"));
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}
*/
// Para el buzzer.
void buzzerCorto()
{
  if (!Misvalores.Audio_Encendido)
  {
    digitalWrite(Pin_Buzzer, HIGH);
    delay(200);
    digitalWrite(Pin_Buzzer, LOW);
  }
  else
  {
    reproducirMensaje(72,300);
  }
}

void buzzerLargo()
{
  if (!Misvalores.Audio_Encendido)
  {
    digitalWrite(Pin_Buzzer, HIGH);
    delay(300);
    digitalWrite(Pin_Buzzer, LOW);
  }
  else
  {
    reproducirMensaje(74,500);
  }
}

void doblePitidoCorto()
{
  if (Misvalores.Audio_Encendido)
  {
    reproducirMensaje(78,500);
 
  }
  else
  {
    digitalWrite(Pin_Buzzer, HIGH);
    delay(200);
    digitalWrite(Pin_Buzzer, LOW);
    delay(50);
    digitalWrite(Pin_Buzzer, HIGH);
    delay(200);
    digitalWrite(Pin_Buzzer, LOW);
    delay(50);
  }
}

void triplePitidoCorto()
{
  if (Misvalores.Audio_Encendido)
  {
    reproducirMensaje(79,1000);
  }
  else
  {
    digitalWrite(Pin_Buzzer, HIGH);
    delay(200);
    digitalWrite(Pin_Buzzer, LOW);
    delay(50);
    digitalWrite(Pin_Buzzer, HIGH);
    delay(200);
    digitalWrite(Pin_Buzzer, LOW);
    delay(50);
    digitalWrite(Pin_Buzzer, HIGH);
    delay(200);
    digitalWrite(Pin_Buzzer, LOW);
    delay(50);
  }
}

void mensajeIndicacion()
{
  if (g_conectadoMQTT)
    reproducirMensaje(52,3000);
  else
    reproducirMensaje(53,3000);
}

/* void Audio_Error_Pico()
{
  if ((millis() - g_Tiempo_Ultimo_Audio) > g_Tiempo_Entre_Audios)
  {
    reproducirMensaje(56);
    delay(1500);
    g_Tiempo_Ultimo_Audio = millis();
  }
} */
