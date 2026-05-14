#include "vending_machine.h"
#include "app_state.h"
#include "lcd_manager.h"
#include "display_7seg.h"
#include "pulsadores.h"
#include "logger.h"
#include "pins.h"
#include "constants.h"
#include "memory_api.h"
#include "sensado.h"
#include "config_common.h"
#include "app_core.h"
#include "calibrador.h"
#include <mediciones.h>

extern bool g_conectado_wifi;


// Sub-estados para el carrousel del display en reposo
enum MensajeReposo
{
  MSG_PAGO,
  MSG_WIFI,
  MSG_PUB
};

static EstadoCalibrador estado_actual = REPOSO;
static MensajeReposo msj_reposo_actual = MSG_PAGO;

// Variables de control de tiempo
static uint32_t t_ultimo_segundo = 0;
static uint32_t t_pausa_mensaje = 0;
static uint32_t t_ultimo_cambio_msj = 0; // Temporizador del carrousel
static int segundos_restantes = 0;

static bool primer_lazo_servicio = true;
static bool primer_lazo_error    = true;
static bool forzar_refresco_msj  = true;
static int  setpoint = 28;

static void displayTiempo(int seg, int row)
{
  char buf[8];
  snprintf(buf, sizeof(buf), "%02d:%02d", seg / 60, seg % 60);
  displayCenteredMessage(buf, row);
}

// =========================================================
// Calibración — constantes y helpers
// =========================================================

static constexpr float DEADBAND_PSI       = 0.30f;
static constexpr float DEADBAND_POSTCRUCE = 0.20f;
static constexpr float RATIO_STD_INFLADO  = 2.0f;   // PSI/s
static constexpr float RATIO_STD_DESINFL  = 0.8f;
static constexpr float T_MAX_INFLADO_MS   = 8000.0f;
static constexpr float T_MAX_DESINFL_MS   = 15000.0f;
static constexpr float T_MIN_PULSO_MS     = 100.0f;
static constexpr float T_MAX_FINO_MS      = 2000.0f;

// 8 lecturas espaciadas 50ms → promedio (bloqueante ~400ms)
static float medirPresionSimple()
{
  float sum = 0;
  for (int i = 0; i < 8; i++) { delay(50); Lectura(); sum += g_Presion_PSI; }
  return sum / 8.0f;
}

// Estima ratio PSI/s para un sentido (bloqueante ~2.5s, se llama una vez por sentido)
static float calculoRatioCalib(bool inflado, int sp)
{
  float antes = medirPresionSimple();
  int   pin   = inflado ? Valvula_Inflado : Valvula_Desinflado;
  digitalWrite(pin, HIGH); delay(2000); digitalWrite(pin, LOW);
  delay(500);
  float despues = medirPresionSimple();
  float delta   = fabsf(despues - antes);
  if (delta < 0.30f) return inflado ? RATIO_STD_INFLADO : RATIO_STD_DESINFL;
  float ratio  = delta / 2.0f;
  float distSP = fabsf(0.5f * (antes + despues) - (float)sp);
  float factor = constrain(1.0f - distSP * 0.025f, 0.85f, 1.30f);
  return ratio * factor;
}

void maquina_vending_loop()
{
  uint32_t now = millis();

  switch (estado_actual)
  {

  // =========================================================
  case REPOSO:
  {

    if (Miupdate.updateFirmwarePendiente || Miupdate.updateFilesystemPendiente)
    {
      ejecutar_OTA_Remoto();
    }

    if (Misvalores.Servicio_Gratis)
    {
      g_pagoQrDisponible = false;
      digitalWrite(PIN_HABILITACION, HIGH);

      if (forzar_refresco_msj)
      {
        clearLCD();
        displayCenteredMessage("SERVICIO GRATIS", 0);
        displayCenteredMessage("PRESIONE BOTON", 1);
        forzar_refresco_msj = false;
      }

      estado_actual = REPOSO_GRATIS;
      break;
    }

    //  MODO NORMAL: Habilitar monedero
    g_pagoQrDisponible = true;
    digitalWrite(PIN_HABILITACION, LOW);

    // --- INICIO CARROUSEL DE MENSAJES ---
    uint32_t tiempo_mostrar = 0;

    if (msj_reposo_actual == MSG_PAGO)
      tiempo_mostrar = 10000;
    else if (msj_reposo_actual == MSG_WIFI)
      tiempo_mostrar = 3000;
    else if (msj_reposo_actual == MSG_PUB)
      tiempo_mostrar = 3000;

    if (forzar_refresco_msj || (now - t_ultimo_cambio_msj >= tiempo_mostrar))
    {

      if (msj_reposo_actual == MSG_PAGO)
      {

        switch (Misvalores.Version_Carrousel)
        {

        case 1:
          displayCenteredMessage("INSERTE", 0);
          displayCenteredMessage("FICHA", 1);
          break;

        case 2:
          if (g_conectado_wifi)
          {
            displayCenteredMessage("ESCANEE", 0);
            displayCenteredMessage("CODIGO QR", 1);
          }
          else
          {
            displayCenteredMessage("MAQUINA FUERA", 0);
            displayCenteredMessage("DE SERVICIO", 1);
          }
          break;

        case 0:
        case 3:
        default:
          displayCenteredMessage("INSERTE FICHA", 0);
          if (g_conectado_wifi)
            displayCenteredMessage("O ESCANEE QR", 1);
          else
            clearRow(1);
          break;
        }

        msj_reposo_actual = Mientorno.CLIENTE_WIFI_Habilitado ? MSG_WIFI : MSG_PUB;
      }

      else if (msj_reposo_actual == MSG_WIFI)
      {
        displayCenteredMessage("WIFI", 0);
        if (g_conectado_wifi)
          displayCenteredMessage("CONECTADO", 1);
        else
          displayCenteredMessage("SIN RED", 1);
        msj_reposo_actual = MSG_PUB;
      }

      else if (msj_reposo_actual == MSG_PUB)
      {
        displayCenteredMessage(Misvalores.Empresa, 0);
        displayCenteredMessage(Misvalores.Telefono, 1);

        msj_reposo_actual = MSG_PAGO;
      }

      t_ultimo_cambio_msj = now;
      forzar_refresco_msj = false;
    }
    // --- FIN CARROUSEL ---

    //  LECTURA DE PAGOS
  if (!Misvalores.Servicio_Gratis && (g_cospelIn || g_pagoelectronico || g_servicioCortesia))
  {
    Incremento_contadores();
    primer_lazo_servicio = true;
    estado_actual = SELECCIONANDO_PRESION;
   
  }
   
   
    break;
  }

// =========================================================
case REPOSO_GRATIS:
{

  if (Miupdate.updateFirmwarePendiente || Miupdate.updateFilesystemPendiente)
  {
    ejecutar_OTA_Remoto();
  }

  if (forzar_refresco_msj)
  {
    clearLCD();
    displayCenteredMessage("SERVICIO GRATIS", 0);
    displayCenteredMessage("SELECCIONE PRESION", 1);
    forzar_refresco_msj = false;
  }

  if ((digitalRead(Pin_Pulsador_2) == LOW)||(digitalRead(Pin_Pulsador_1) == LOW))
  {
    primer_lazo_servicio = true;
    estado_actual = SELECCIONANDO_PRESION;
  }

  break;
}

// =========================================================
case SELECCIONANDO_PRESION:
{
  static bool     pre_inflando  = false;
  static uint32_t t_pre_inflado = 0;

  if (primer_lazo_servicio)
  {
    setpoint = 28;
    segundos_restantes = Misvalores.Servicio_Gratis ? 600 : Misvalores.Tiempo_Programado * 60;
    t_ultimo_segundo = now;
    primer_lazo_servicio = false;
    pre_inflando = false;
    g_enServicio = true;
    clearLCD();
    displayCenteredMessage("TIEMPO RESTANTE", 0);
    displayTiempo(segundos_restantes, 1);
    Mostrar_Presion(setpoint);
  }

  // Countdown tick
  if (now - t_ultimo_segundo >= 1000)
  {
    t_ultimo_segundo = now;
    segundos_restantes--;
    displayTiempo(segundos_restantes, 1);
  }

  // Timer expired
  if (segundos_restantes <= 0)
  {
    g_enServicio = false;
    g_cospelIn        = false;
    g_pagoelectronico  = false;
    g_servicioCortesia = false;
    if (g_reinicio_pendiente)
    {
      g_reinicio_pendiente = false;
      Serial.println("Ejecutando reinicio diferido.");
      delay(300);
      ESP.restart();
    }
    clearLCD();
    displayCenteredMessage("MUCHAS GRACIAS", 0);
    t_pausa_mensaje = now;
    estado_actual = MOSTRANDO_GRACIAS;
    break;
  }

  // New payment during service: reset timer
  if (!Misvalores.Servicio_Gratis && (g_cospelIn || g_pagoelectronico || g_servicioCortesia))
  {
    Incremento_contadores();
    g_cospelIn        = false;
    g_pagoelectronico  = false;
    g_servicioCortesia = false;
    segundos_restantes = Misvalores.Tiempo_Programado * 60;
    t_ultimo_segundo   = now;
    displayTiempo(segundos_restantes, 1);
  }

  // Pre-inflado de emergencia: ambos pulsadores → abre válvula 5 s para neumático sin presión
  bool ambos_presionados = (digitalRead(Pin_Pulsador_1) == LOW) && (digitalRead(Pin_Pulsador_2) == LOW);

  if (!pre_inflando && ambos_presionados) {
    pre_inflando  = true;
    t_pre_inflado = now;
    digitalWrite(Valvula_Inflado, HIGH);
    clearLCD();
    displayCenteredMessage("PRE-INFLANDO...", 0);
    displayTiempo(segundos_restantes, 1);
    break;
  }

  if (pre_inflando) {
    if (now - t_pre_inflado < 5000) {
      break;  // Sigue inflando, omitir el resto del estado
    }
    // Fin del pre-inflado: cerrar válvula y restaurar display
    digitalWrite(Valvula_Inflado, LOW);
    pre_inflando = false;
    clearLCD();
    displayCenteredMessage("TIEMPO RESTANTE", 0);
    displayTiempo(segundos_restantes, 1);
    Mostrar_Presion(setpoint);
    break;
  }

  // Pressure setpoint: P1 sube, P2 baja (acelerado al mantener)
  if (Push_P1_accel() && setpoint < 50) { setpoint++; Mostrar_Presion(setpoint); }
  if (Push_P2_accel() && setpoint > 5)  { setpoint--; Mostrar_Presion(setpoint); }

  // Lectura de presión en el pico (no bloqueante)
  {
    ResultadoMedicion medicion = EstadoPicoV3();
    if (medicion == ResultadoMedicion::ESTABLE)
    {
      estado_actual = CALIBRANDO;
      break;
    }
    if (medicion == ResultadoMedicion::INESTABLE)
    {
      primer_lazo_error = true;
      estado_actual = ERROR_PICO;
      break;
    }
  }

  break;
}
case ERROR_PICO:
{
  if (primer_lazo_error)
  {
    Mostrar_Error();
    clearLCD();
    displayCenteredMessage("ERROR DE LECTURA", 0);
    displayCenteredMessage("CONECTE EL PICO", 1);
    primer_lazo_error = false;
  }

  // Detectar desconexión: 3 lecturas consecutivas < 5 PSI
  static uint32_t t_lect_error = 0;
  static int      cont_bajas   = 0;

  if (now - t_lect_error >= 200)
  {
    t_lect_error = now;
    Lectura();
    if (g_Presion_PSI < 5.0f)
    {
      if (++cont_bajas >= 3)
      {
        cont_bajas           = 0;
        primer_lazo_servicio = true;
        estado_actual        = SELECCIONANDO_PRESION;
      }
    }
    else
    {
      cont_bajas = 0;
    }
  }

  break;
}
case CALIBRANDO:
{
  enum SubCalib : uint8_t { INICIO, EVALUANDO, PULSANDO, ESTABILIZANDO, MIDIENDO, LISTO };
  static SubCalib sub              = INICIO;
  static bool     ratioInflCalc    = false;
  static bool     ratioDesinflCalc = false;
  static float    ratioInfl        = RATIO_STD_INFLADO;
  static float    ratioDesInfl     = RATIO_STD_DESINFL;
  static bool     inflado          = false;
  static bool     huboCruce        = false;
  static int      contCruces       = 0;
  static float    tActivacion      = 0;
  static uint32_t t_pulso          = 0;
  static uint32_t t_estab          = 0;

  // Countdown — igual que en SELECCIONANDO_PRESION
  if (now - t_ultimo_segundo >= 1000)
  {
    t_ultimo_segundo = now;
    segundos_restantes--;
    displayTiempo(segundos_restantes, 1);
  }
  if (segundos_restantes <= 0)
  {
    digitalWrite(Valvula_Inflado,    LOW);
    digitalWrite(Valvula_Desinflado, LOW);
    sub          = INICIO;
    g_enServicio = false;
    clearLCD();
    displayCenteredMessage("MUCHAS GRACIAS", 0);
    t_pausa_mensaje = now;
    estado_actual   = MOSTRANDO_GRACIAS;
    break;
  }

  switch (sub)
  {
  // ── Inicio de sesión de calibración ──────────────────────
  case INICIO:
  {
    huboCruce        = false;
    contCruces       = 0;
    ratioInflCalc    = false;
    ratioDesinflCalc = false;
    ratioInfl        = RATIO_STD_INFLADO;
    ratioDesInfl     = RATIO_STD_DESINFL;

    clearLCD();
    displayCenteredMessage("CALIBRANDO", 0);
    Mostrar_Presion(PresionRedondeada);

    float delta = PresionMedidaPromedio - (float)setpoint;
    if (fabsf(delta) < DEADBAND_PSI) { sub = LISTO; break; }

    inflado = (delta < 0);

    // Cálculo de ratio solo si estamos lejos del setpoint (>3 PSI)
    if (inflado && !ratioInflCalc && fabsf(delta) > 3.0f)
    {
      ratioInfl     = calculoRatioCalib(true, setpoint);
      ratioInflCalc = true;
    }
    else if (!inflado && !ratioDesinflCalc && fabsf(delta) > 3.0f)
    {
      ratioDesInfl     = calculoRatioCalib(false, setpoint);
      ratioDesinflCalc = true;
    }

    sub = EVALUANDO;
    break;
  }

  // ── Evaluar delta y calcular próximo pulso ─────────────────
  case EVALUANDO:
  {
    float delta = PresionMedidaPromedio - (float)setpoint;
    float banda = huboCruce ? DEADBAND_POSTCRUCE : DEADBAND_PSI;

    if (fabsf(delta) < banda) { sub = LISTO; break; }

    bool prevInflado = inflado;
    inflado = (delta < 0);

    if (inflado != prevInflado)
    {
      huboCruce = true;
      contCruces++;
    }
    else
    {
      huboCruce = false;
    }

    float ratioBase  = inflado ? ratioInfl : ratioDesInfl;
    tActivacion      = 1000.0f * fabsf(delta) / ratioBase;

    if (huboCruce)
      tActivacion /= (float)contCruces;

    float tMax = inflado ? T_MAX_INFLADO_MS : T_MAX_DESINFL_MS;
    if (fabsf(delta) <= 1.0f) tMax = T_MAX_FINO_MS;

    tActivacion = constrain(tActivacion, T_MIN_PULSO_MS, tMax);

    // Actualizar LCD con dirección
    clearLCD();
    displayCenteredMessage(inflado ? "INFLANDO" : "DESINFLANDO", 0);
    displayTiempo(segundos_restantes, 1);

    // Abrir válvula y pasar a PULSANDO
    digitalWrite(inflado ? Valvula_Inflado : Valvula_Desinflado, HIGH);
    t_pulso = now;
    sub     = PULSANDO;
    break;
  }

  // ── Mantener válvula abierta el tiempo calculado ───────────
  case PULSANDO:
  {
    if (now - t_pulso >= (uint32_t)tActivacion)
    {
      digitalWrite(Valvula_Inflado,    LOW);
      digitalWrite(Valvula_Desinflado, LOW);
      t_estab = now;
      sub     = ESTABILIZANDO;
    }
    break;
  }

  // ── Esperar estabilización de presión (500ms) ──────────────
  case ESTABILIZANDO:
  {
    if (now - t_estab >= 500)
    {
      Mostrar_Presion(PresionRedondeada);
      sub = MIDIENDO;
    }
    break;
  }

  // ── Nueva lectura no bloqueante con EstadoPicoV3 ───────────
  case MIDIENDO:
  {
    ResultadoMedicion r = EstadoPicoV3();
    if (r == ResultadoMedicion::ESTABLE)
    {
      sub = EVALUANDO;
    }
    else if (r == ResultadoMedicion::INESTABLE)
    {
      sub               = INICIO;
      primer_lazo_error = true;
      estado_actual     = ERROR_PICO;
    }
    break;
  }

  // ── Presión dentro de la banda — calibración exitosa ──────
  case LISTO:
  {
    Mostrar_Presion(PresionRedondeada);
    clearLCD();
    displayCenteredMessage("PRESION ALCANZADA", 0);
    displayTiempo(segundos_restantes, 1);
    sub          = INICIO;
    g_enServicio = false;
    t_pausa_mensaje = now;
    estado_actual   = MOSTRANDO_GRACIAS;
    break;
  }
  } // switch sub

  break;
}
// =========================================================
case MOSTRANDO_GRACIAS:
{

  if (now - t_pausa_mensaje >= 2000)
  {
    forzar_refresco_msj = true;
    estado_actual = REPOSO;
  }

  break;
}
}
}

// =========================================================

void vending_forzar_refresco()
{
  forzar_refresco_msj = true;
}

// =========================================================

void Incremento_contadores()
{

  if (g_inTest)
  {
    g_inTest = false;
    g_servicioCortesia = false;
    return;
  }

  String tipoServicio = "";

  if (g_pagoelectronico)
  {
    Miscontadores.Cont_Parcial_OnLine++;
    Miscontadores.Cont_Total_Online++;
    tipoServicio = "Online";
  }
  else if (g_servicioCortesia)
  {
    Miscontadores.Cont_Parcial_Cortesia++;
    Miscontadores.Cont_Total_Cortesia++;
    tipoServicio = "Cortesia";
  }
  else if (g_cospelIn)
  {
    Miscontadores.Cont_Parcial_Cospeles++;
    Miscontadores.Cont_Total_Cospeles++;
    tipoServicio = "Fichas";
  }

  Save_Flash_Contadores();

   g_cospelIn        = false;
   g_pagoelectronico  = false;
   g_servicioCortesia = false;
  if (tipoServicio != "")
  {
    char logMsg[100];
    snprintf(logMsg, sizeof(logMsg),
             "Servicio OK: %s | TF:%lu TO:%lu TC:%lu",
             tipoServicio.c_str(),
             (unsigned long)Miscontadores.Cont_Total_Cospeles,
             (unsigned long)Miscontadores.Cont_Total_Online,
             (unsigned long)Miscontadores.Cont_Total_Cortesia);

    logEvent(logMsg);
  }


}