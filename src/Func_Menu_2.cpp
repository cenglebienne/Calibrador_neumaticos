#include <menu.h>
#include <pulsadores.h>
#include <display.h>
#include <audio.h>
#include <mediciones.h>
#include "memory_api.h"
#include <calibracion.h>
#include "app_state.h"
//-------------------------// Para Menus De Programacion //-------------------------//

int numprograma;
int num_mensaje;
int Minutos = 3;
int Segundos = 0;
int Min = 1;
int Seg = 0;
unsigned long g_Tiempo_Ahora;

bool TiempoFuera = false;

bool salir_prog = false;
bool readyforMP = false;
bool Primera_Vez = true;
bool Primer_Loop = true;
bool Primer_Loop2 = true;
bool Primer_Loop3 = true;
bool Primer_Loop4 = true;
bool Primer_Loop5 = true;

bool Primer_LoopOut = true;
int Presion_Actual = 0;
unsigned long tiempoInicio = 0;

//-----// Audio //-----//
bool Menu_Audio = false;
bool Cambio = false;

//-----// Estadistica //-----//

bool Menu_Estadistica = false;
int Cont_Sub_1;

//-----// Precio Y Tiempo //-----//

bool Menu_Precio_Y_TMPO = false;

//-----// Prueba //-----//

bool Menu_Prueba = false;

//-----// Calibracion //-----//

bool Menu_Calibracion = false;

//-----// Wifi //-----//

bool Menu_AP = false;

//-----// Detectar Fallas //-----//

bool Menu_Detectar_Fallas = false;
bool Msj_AA = true;
bool Msj_Menu_AA = false;
bool Auto_Analisis = false;
bool Paso1 = false;
bool Paso2 = false;
bool Paso3 = false;
bool Falla_Sensor = false;
bool Falla_Inflado = false;
bool Falla_Desinflado = false;
bool g_Timeout=false;

int Aux3 = 0;
int Cont_Sub_2 = 1;
unsigned long Tiempo_Ultimo_Clock;

void subprog1();
void subprog2();
void subprog3();
void subprog4();
void subprog5();
void subprog6();
void subprog7();


void sub_prog()
{

  Minutos = 3;
  Segundos = 0;
  bool PP;
  int numprograma_old = 99;
  int tpo_nopress;
  Serial.println(millis());

  Primer_Loop = true;
  salir_prog = false;
  readyforMP = false;
  numprograma = 1;

  while (!salir_prog)
  {

    yield();
    PP = PushProg();
    if (PP)
    {
      Minutos = 3;
      Segundos = 0;
      Primer_Loop = true;
      numprograma++;
      if (numprograma >= 8)
        numprograma = 1;
      if (Cambio)
      {
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("GUARDANDO");
        lcd.setCursor(5, 1);
        lcd.print("DATOS");
        reproducirMensaje(62, 100);
        Save_Flash_ValorServicio();
        Save_Flash_ValorServicio();
        delay(1500);
        Cambio = false;
      }
    }

    if ((digitalRead(Pin_Pulsador_1) == LOW) || (digitalRead(Pin_Pulsador_2) == LOW))
    {
      Minutos = 3;
      Segundos = 0;
    }

    if ((millis() - Tiempo_Ultimo_Clock) >= 1000)
    {                                 // Actualiza cada segundo (1000 ms)
      Tiempo_Ultimo_Clock = millis(); // Guarda el tiempo actual como el anterior
      Segundos--;
      if (Segundos < 0)
      {
        Segundos = 59;
        Minutos--;
      }
      if ((Minutos == 0) && (Segundos <= 0))
      {
        TiempoFuera = true;
        subprog6();
      }
    }

    if (numprograma != 2)
      Mostrar_Programa(numprograma);

    switch (numprograma)
    {
    case 1:
      subprog1();
      break;
    case 2:
      subprog2();
      break;
    case 3:
      subprog3();
      break;
    case 4:
      subprog4();
      break;
    case 5:
      subprog5();
      break;
    case 6:
      subprog6();
      break;
    case 7:
      subprog7();
      break;
    }
  }

  Serial.println("Programa ");
  Serial.println(numprograma);
  Serial.println(millis());
  g_timer_mensajes = 0;
  readyforMP = true; // OJO ACA  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

//--------------------// Sub Menu nº 1: ESTADISTICA //--------------------//

void subprog1()
{

  Primer_LoopOut = true;
  digitalWrite(PIN_HABILITACION, HIGH);

  if (Primer_Loop)
  {
    Cont_Sub_1 = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("---PROGRAMA 1---");
    lcd.setCursor(1, 1);
    lcd.print("(ESTADISTICAS)");
    Primer_Loop = false;
  }

  if (!Menu_Estadistica)
  {
    static bool P1, P2;

    P1 = Push_P1();
    P2 = Push_P2();
    if (P1 || P2)
    {
      Menu_Estadistica = true;
      lcd.clear();
      Primer_Loop2 = true;
    }
  }

  if (Menu_Estadistica)
  {
    static bool P1, P2;
    P1 = Push_P1();
    P2 = Push_P2();

    if (P1)
    {
      Cont_Sub_1++;
      if (Cont_Sub_1 == 8)
        Cont_Sub_1 = 1;
      lcd.clear();
    }
    if (P2)
    {
      Cont_Sub_1--;
      if (Cont_Sub_1 == 0)
        Cont_Sub_1 = 7;
      lcd.clear();
    }

    switch (Cont_Sub_1)
    {

    case 1:
    {
      displayMessage("PARCIAL FICHAS",1, 0,false);
      displayValor(Miscontadores.Cont_Parcial_Cospeles, 6, 1);
      break;
    }

    case 2:
    {
      displayMessage("TOTAL FICHAS", 1, 0,false);
      displayValor(Miscontadores.Cont_Total_Cospeles, 6, 1);
      break;
    }
    case 3:
    {
      displayMessage("PARCIAL ONLINE",1, 0,false);
      displayValor(Miscontadores.Cont_Parcial_OnLine, 6, 1);
      break;
    }
    case 4:
    {
      displayMessage("TOTAL ONLINE",1, 0,false);
      displayValor(Miscontadores.Cont_Total_Online, 6, 1);
      break;
    }
    case 5:
    {
      displayMessage("PARCIAL CORTESIA", 0, 0,false);
      displayValor(Miscontadores.Cont_Parcial_Cortesia, 6, 1);
      break;
    }
    case 6:
    {
      displayMessage("TOTAL CORTESIA", 1, 0,false);
      displayValor(Miscontadores.Cont_Total_Cortesia, 6, 1);
      break;
    }
    case 7:
    { // RESETEAR CONTADORES PARCIALES
      displayMessage("RESET CONTADORES?",0, 0,false);
      displayMessage("+ Y - PARA SI",2,1,false);

      if (!(digitalRead(Pin_Pulsador_1)) && !(digitalRead(Pin_Pulsador_2)))
      {
        reproducirMensaje(61, 100);
        clearLCD();
        displayCenteredMessage("BORRANDO", 0);
        displayCenteredMessage("CONTADORES", 1);
        Miscontadores.Cont_Parcial_Cortesia = 0;
        Miscontadores.Cont_Parcial_OnLine = 0;
        Miscontadores.Cont_Parcial_Cospeles = 0;
        Save_Flash_Contadores();
        delay(1000);
        lcd.clear();
        Cont_Sub_1 = 1;
      }
      break;
    }
    }
  }
}

//--------------------// Sub Menu nº 2: PRUEBA //--------------------//

void subprog2()
{
  static bool P1, P2;
  bool Primer_LoopBZ = true;

  if ((digitalRead(Pin_Pulsador_1) == HIGH) && (digitalRead(Pin_Pulsador_2) == HIGH))
    Mostrar_Programa(numprograma);
  digitalWrite(PIN_HABILITACION, LOW);

  if (Primer_Loop)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("---PROGRAMA 2---");
    lcd.setCursor(4, 1);
    lcd.print("(PRUEBA)");
    Primer_Loop = false;
  }

  if (!Menu_Prueba)
  {
    P1 = Push_P1();
    P2 = Push_P2();
    if (P1 || P2)
    {
      g_cospel_in = false;
      Primer_Loop2 = true;
      Primer_Loop3 = true;
      Menu_Prueba = true;
    }
  }
  if (Menu_Prueba)
  {

    Lectura();

    if (Primer_Loop2)
    {

      g_Tiempo_Ahora = millis();
      if (Primer_Loop3)
        lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PRESION:");
      lcd.setCursor(11, 0);
      if (g_Presion_PSI > 5)
        lcd.print(g_Presion_PSI);
      else
      {
        lcd.setCursor(11, 0);
        lcd.print("0.00  ");
      }
     
     if (WiFi.status() != WL_CONNECTED)
          displayMessage(" SIN CONEXION  ", 1,1, false);
      else if (mqttClient.connected())
          displayMessage("CONECTADO SERV", 1,1, false);
      else
          displayMessage("CONECTADO WIFI", 1,1, false);
     
      Primer_Loop2 = false;
      Primer_Loop3 = false;
    }

    if ((digitalRead(Pin_Pulsador_1) == LOW) && ((digitalRead(Pin_Pulsador_2) == LOW)))
    {
      digitalWrite(Pin_Buzzer, HIGH);
      delay(300);
      digitalWrite(Pin_Buzzer, LOW);
      digitalWrite(Valvula_Inflado, LOW);
      digitalWrite(Valvula_Desinflado, LOW);
      if (Primer_LoopBZ)
        lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PRESION:");
      lcd.setCursor(11, 0);
      if (g_Presion_PSI > 5)
        lcd.print(g_Presion_PSI);
      else
      {
        lcd.setCursor(11, 0);
        lcd.print("0.00  ");
      }
      lcd.setCursor(5, 1);
      lcd.print("BUZZER");
      Primer_LoopBZ = false;
    }
    else
    {

      Primer_LoopBZ = true;

      if (digitalRead(Pin_Pulsador_1) == LOW)
      {

        g_Tiempo_Ahora = millis();
        if ((millis() - g_Tiempo_Ahora) > 100)
          lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("PRESION:");
        lcd.setCursor(11, 0);
        if (g_Presion_PSI > 5)
          lcd.print(g_Presion_PSI);
        else
        {
          lcd.setCursor(11, 0);
          lcd.print("0.00  ");
        }
        Mostrar_UP();
        digitalWrite(Valvula_Inflado, HIGH);
      }
      else
      {
        digitalWrite(Valvula_Inflado, LOW);
        Primer_Loop2 = true;
      }

      if (digitalRead(Pin_Pulsador_2) == LOW)
      {

        g_Tiempo_Ahora = millis();
        if ((millis() - g_Tiempo_Ahora) > 100)
          lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("PRESION:");
        lcd.setCursor(11, 0);
        if (g_Presion_PSI > 5)
          lcd.print(g_Presion_PSI);
        else
        {
          lcd.setCursor(11, 0);
          lcd.print("0.00  ");
        }
        Mostrar_DN();
        digitalWrite(Valvula_Desinflado, HIGH);
      }
      else
      {
        digitalWrite(Valvula_Desinflado, LOW);
        Primer_Loop2 = true;
      }

      /*       if ((digitalRead(Pin_Pulsador_1) == HIGH) && ((digitalRead(Pin_Pulsador_2) == HIGH)))
            {
              digitalWrite(Pin_Buzzer, LOW);
              g_Tiempo_Ahora = millis();
              if ((millis() - g_Tiempo_Ahora) > 100)
                lcd.clear();
              lcd.setCursor(6, 1);
              lcd.print("          ");
            } */
    }

    // monedero();

    if (g_cospel_in)
    {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("INGRESO");
      lcd.setCursor(4, 1);
      lcd.print("1 FICHA");
      g_Tiempo_Ahora = millis();
      // reproducirMensaje(65);
      delay(2000);
      g_cospel_in = false;
      Primer_Loop2 = true;
      Primer_Loop3 = true;
    }
    if (g_entro_pulso)
    {
      g_entro_pulso = false;
      pulseDuration = pulseDuration / 1000;
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("PULSO");
      lcd.setCursor(4, 1);
      lcd.print(String(pulseDuration));
      g_Tiempo_Ahora = millis();
      delay(2000);
      pulseDuration = 0;
      Primer_Loop2 = true;
      Primer_Loop3 = true;
    }
  }
}

//--------------------// Sub Menu nº 3: AUDIO //--------------------//

void subprog3()
{
  static bool P1, P2;
  digitalWrite(PIN_HABILITACION, HIGH);

  if (Primer_Loop)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("---PROGRAMA 3---");
    lcd.setCursor(4, 1);
    lcd.print("(AUDIO)");
    Primer_Loop = false;
  }

  P1 = Push_P1();
  P2 = Push_P2();
  if (P1 || P2)
  {
    Primer_Loop2 = true;
    Menu_Audio = true;
  }

  if (Menu_Audio)
  {
    if (Primer_Loop2)
    {
      if (Misvalores.Audio_Encendido)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("AUDIO: ENCENDIDO");
        lcd.setCursor(1, 1);
        lcd.print("- PARA APAGAR");
      }
      else
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("AUDIO: APAGADO");
        lcd.setCursor(0, 1);
        lcd.print("+ PARA ENCENDER");
      }
      Primer_Loop2 = false;
    }
    if ((digitalRead(Pin_Pulsador_1) == LOW) && (digitalRead(Pin_Pulsador_2) == LOW))
    {
      if (tiempoInicio == 0)
      {
        tiempoInicio = millis();
      }
      if (millis() - tiempoInicio > 7500)
      {
        lcd.setCursor(0, 0);
        lcd.print("BORRANDO TOTALES");
        lcd.setCursor(1, 1);
        lcd.print("DE LA MEMORIA");
        reproducirMensaje(61, 100);
        delay(2500);
        Miscontadores = {0, 0, 0, 0, 0, 0, 0, 0};
        Save_Flash_Contadores();

        doblePitidoCorto();
      }
    }
    else
    {
      tiempoInicio = 0;
      if ((Misvalores.Audio_Encendido) && (!digitalRead(Pin_Pulsador_2)))
      {
        myDFPlayer.volume(0);
        Misvalores.Audio_Encendido = false;
        Save_Flash_ValorServicio();

        Cambio = true;
        Primer_Loop2 = true;
      }
      if ((!Misvalores.Audio_Encendido) && (!digitalRead(Pin_Pulsador_1)))
      {
        myDFPlayer.volume(30);
        Misvalores.Audio_Encendido = true;
        Save_Flash_ValorServicio();
        reproducirMensaje(66, 100);
        delay(500);
        Cambio = true;
        Primer_Loop2 = true;
      }
    }
  }
}
//--------------------// Sub Menu nº 4: PRECIO Y TMPO //--------------------//

void subprog4()
{
  static bool P1, P2;

    digitalWrite(PIN_HABILITACION, LOW);

    if (Primer_Loop)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("---PROGRAMA 4---");
      lcd.setCursor(0, 1);
      lcd.print("(PRECIO Y TMPO)");
      Primer_Loop = false;
    }

    if (!Menu_Precio_Y_TMPO)
    {
      P1 = Push_P1();
      P2 = Push_P2();
      if (P1 || P2)
      {
        Primer_Loop2 = true;
        Menu_Precio_Y_TMPO = true;
      }
    }

    if (Menu_Precio_Y_TMPO)
    {

      if (Primer_Loop2)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TIEMPO:");
        lcd.setCursor(9, 0);
        lcd.print(Misvalores.Tiempo_Programado);
        lcd.setCursor(12, 0);
        lcd.print("MIN");
        if (Misvalores.Servicio_Gratis)
        {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("SERV GRATUITO");
        }
        if (!Misvalores.Servicio_Gratis)
        {
          lcd.setCursor(0, 1);
          lcd.print("PRECIO: 1 FICHA");
        }
        Primer_Loop2 = false;
      }

      P1 = Push_P1();
      P2 = Push_P2();

      if (P1)
      {
        Misvalores.Tiempo_Programado++;
        if (Misvalores.Tiempo_Programado == 11)
          Misvalores.Tiempo_Programado = 1;

        Cambio = true;
        Primer_Loop2 = true;
      }

      if (P2)
      {
        Misvalores.Tiempo_Programado--;
        if (Misvalores.Tiempo_Programado == 0)
          Misvalores.Tiempo_Programado = 1;

        Cambio = true;
        Primer_Loop2 = true;
      }

      g_Tiempo_Servicio = Misvalores.Tiempo_Programado;

      if (!(digitalRead(Pin_Pulsador_1)) && !(digitalRead(Pin_Pulsador_2)))
      {
        delay(350);
        Misvalores.Servicio_Gratis = true;

        Cambio = true;
        Primer_Loop2 = true;
      }

      // monedero();

      if (g_cospel_in == true)
      {
        Misvalores.Servicio_Gratis = false;

        g_cospel_in = false;
        Cambio = true;
        Primer_Loop2 = true;
      }
    }
  }
}

//--------------------// Sub Menu nº 5: WIFI //--------------------//

void subprog5()
{
  static bool P1, P2;
  digitalWrite(PIN_HABILITACION, HIGH);

  if (Primer_Loop)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("---PROGRAMA 5---");
    lcd.setCursor(1, 1);
    lcd.print("(ACCESS POINT)");
    Primer_Loop = false;
    Primer_Loop2 = true;
  }

  P1 = Push_P1();
  P2 = Push_P2();
  if (P1 || P2)
  {
    Menu_AP = true;
  }

  if (Menu_AP)
  {
    if (Primer_Loop2)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" PRESIONE +");
      lcd.setCursor(1, 1);
      lcd.print("PARA MODO AP");
      Primer_Loop2 = false;
      P1 = false;
    }

    if (P1)
    {
      setupLCD(); // Inicializa el LCD
      Set_Flash_Bool("Levantar_AP", true);
      delay(1000);
      reiniciar();
    }
  }
}

//--------------------// Sub Menu nº 6: CALIBRACIÓN //--------------------//

void subprog6()
{
  static bool P1, P2;
  static bool primeraEntrada = true;

  digitalWrite(PIN_HABILITACION, HIGH);

  if (Primer_Loop)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("---PROGRAMA 6---");
    lcd.setCursor(1, 1);
    lcd.print("(CALIBRACION)");
    Primer_Loop = false;

    Menu_Calibracion = false;
    Primer_Loop2 = true;
    primeraEntrada = false;
  }

  if (!Menu_Calibracion)
  {
    P1 = Push_P1();
    P2 = Push_P2();

    if (P1 || P2)
    {
      Menu_Calibracion = true;
      Primer_Loop2 = true;
    }
    return; 
  }

  // --- Menú de calibración ---
  if (Menu_Calibracion)
  {
    
    if (Primer_Loop2)
    {
      lcd.clear();
      displayCenteredMessage("FLUJO COMPRESOR", 0);
      lcd.setCursor(7, 1);
      lcd.print(Misvalores.Validador);
      Primer_Loop2 = false;
    }

    // Lectura de botones
    P1 = Push_P1();
    P2 = Push_P2();

    // Incrementar valor
    if (P1)
    {
      if (Misvalores.Validador < 3)
      {
        Misvalores.Validador++;
        Cambio = true;
        Primer_Loop2 = true;
      }
    }

    // Decrementar valor
    if (P2)
    {
      if (Misvalores.Validador > 1)
      {
        Misvalores.Validador--;
        Cambio = true;
        Primer_Loop2 = true;
      }
    }

  }
}

//--------------------// Sub Menu nº 7: SALIDA //--------------------//

void subprog7()
{
  static bool P1, P2;
  digitalWrite(PIN_HABILITACION, HIGH);

  if (Primer_LoopOut)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("---- SALIDA ----");
    lcd.setCursor(0, 1);
    lcd.print(" (+) PARA SALIR");
    Primer_LoopOut = false;
  }

  Menu_Estadistica = false;
  Menu_Prueba = false;
  Menu_Audio = false;
  Menu_Precio_Y_TMPO = false;
  //Menu_Calibracion = false;
  Menu_AP = false;
  Paso1 = false;
  Paso2 = false;
  Paso3 = false;
  Falla_Sensor = false;
  Falla_Inflado = false;
  Falla_Desinflado = false;
  Msj_Menu_AA = false;
  Cont_Sub_2 = 1;
  Msj_AA = true;
  Aux3 = 0;
  Auto_Analisis = false;
  Menu_Detectar_Fallas = false;
  Primer_Loop = true;
  Primer_Loop2 = true;
  Primer_Loop3 = true;
  Primer_Loop4 = true;
  Primer_Loop5 = true;

  P1 = Push_P1();

  if (P1 || TiempoFuera)
  {
    Primer_LoopOut = true;
    TiempoFuera = false;
    salir_prog = true;
    Mostrar_Rayita();
    lcd.clear();
    if (Misvalores.Servicio_Gratis)
    {
      lcd.setCursor(0, 0);
      lcd.print("SERVICIO GRATIS");
    }
    if (!Misvalores.Servicio_Gratis)
    {
      g_Timeout = true;
      lcd.setCursor(1, 0);
      lcd.print("UTILICE FICHA");
    }
  }
}
