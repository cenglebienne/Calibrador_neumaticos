#include <calibracion.h>
#include <audio.h>
#include <display.h>
#include <mediciones.h>
#include <constants.h>
#include <mensajes.h>
#include "memory_api.h"
#include <config.h>
#include <Arduino.h>

// ==================
float g_RatioCliente;
int   g_SetPoint;
bool  g_Pico_Mal_Conectado;

// Estos ratios estándar se usan como fallback y cerca del setpoint
float ratioStandardInflado ;
const float ratioStandardDesinflado = 0.8f;

// Límites de seguridad para válvulas
const float TIEMPO_MAXIMO_VALVULA_DESINFLADO = 15000.0f; // ms
const float TIEMPO_MAXIMO_VALVULA_INFLADO    =  8000.0f; // ms
const float PULSO_MAX_FINO_MS = 2000.0f;
// Pulsos mínimos sugeridos

const float PULSO_MIN_MS   = 100.0f;  // cerca del setpoint


// Banda de aceptación (deadband) y extra si hubo cruce recién (histeresis)
const float DEADBAND_OK_PSI       = 0.30f;
const float DEADBAND_POST_CRUCE   = 0.20f;


extern float PresionMedidaPromedio; // lectura filtrada/estable
//extern int   PresionRedondeada;     // lectura redondeada para UI/voz

/* extern bool  EstadoPicoV3();        // true si hay problema de lectura / pico mal conectado
extern void  Lectura();             // refresca PresionMedidaPromedio/Redondeada
extern void  Mostrar_Presion(int v);
extern void  Mostrar_Presion(float v);
extern bool  mostrarTiempoRestante(bool ServicioPago); */
//extern void  triplePitidoCorto();
//extern void  reproducirMensaje(int id, int ms);

// Viene de tu lógica de servicio/tiempo restante
//extern volatile int g_segundosRestantes;

// =========================
// Helpers internos
// =========================


// Prueba corta para medir ratio real de inflado o desinflado.
// Si algo sale mal, vuelve al ratio estándar correspondiente.
static float calculoRatio(bool inflado)
{
  Serial.println(inflado ? F("🧪 Calculando ratio INFLADO") : F("🧪 Calculando ratio DESINFLADO"));

  // Lectura base
  Lectura();
  if (EstadoPicoV3()) {
    Serial.println(F("⚠️ Lectura inválida antes de la prueba. Uso ratio estándar."));
    return inflado ? ratioStandardInflado : ratioStandardDesinflado;
  }

  float presionAntes = PresionMedidaPromedio;

  // Pulso de prueba
  const unsigned long tiempoPruebaMs = 2000;
  digitalWrite(inflado ? Valvula_Inflado : Valvula_Desinflado, HIGH);
  delay(tiempoPruebaMs);
  digitalWrite(inflado ? Valvula_Inflado : Valvula_Desinflado, LOW);
  //Mostrar_Presion(PresionRedondeada); ver si agrego
  // Estabilización breve y nueva lectura
  delay(500);
  Lectura();
  if (EstadoPicoV3()) {
    Serial.println(F("⚠️ Lectura inválida post prueba. Uso ratio estándar."));
    return inflado ? ratioStandardInflado : ratioStandardDesinflado;
  }

  float presionDespues = PresionMedidaPromedio;
  float delta          = fabsf(presionDespues - presionAntes);
  if (delta < 0.30f) {
    Serial.println(F("⚠️ Delta muy bajo en prueba. Uso ratio estándar."));
    return inflado ? ratioStandardInflado : ratioStandardDesinflado;
  }

  // Ratio bruto (PSI/s)
  float ratio = delta / (tiempoPruebaMs / 1000.0f);

  // Suavizado por distancia al setpoint (evita ratios inflados si probaste muy cerca)
  float presionPromedioPrueba = 0.5f * (presionAntes + presionDespues);
  float distanciaAlSetpoint   = fabsf(presionPromedioPrueba - g_SetPoint);
  float factor                = 1.0f - distanciaAlSetpoint * 0.025f; // -2.5% por PSI de distancia
  factor                      = constrain(factor, 0.85f, 1.30f);
  ratio                      *= factor;

  Serial.printf("📈 Ratio %s calculado: %.2f PSI/s (factor=%.2f)\n",
                inflado ? "INFLADO" : "DESINFLADO", ratio, factor);
  return ratio;
}

// Pulso largo de emergencia/servicio (se mantiene del diseño original)
void InfladoLargo()
{
  digitalWrite(Valvula_Inflado, HIGH);
  delay(7000);
  digitalWrite(Valvula_Inflado, LOW);
  g_segundosRestantes = g_segundosRestantes - 7;
}

// =========================
// Calibración principal
// =========================

void Calibracion(bool ServicioPago, bool &calcularRatio)
{
  // Flags por sentido (persisten entre llamadas)
  static bool ratioInfladoCalculado    = false;
  static bool ratioDesinfladoCalculado = false;
  static float ratioInfladoaUsar;
  static float ratioDesinfladoaUsar;

  int contadorCruces = 0;
  // Si desde Servicio() nos piden recalcular, resetear flags acá
  if (calcularRatio) {
    ratioInfladoCalculado    = false;
    ratioDesinfladoCalculado = false;
    calcularRatio            = false; // lo consumimos
    Serial.println(F("🔄 Forzando recálculo de ratios por pedido del servicio."));
    ratioInfladoaUsar     = ratioStandardInflado;
    ratioDesinfladoaUsar  = ratioStandardDesinflado;
  }

  Serial.println(F("🔧 Iniciando Calibracion()..."));

  // Lectura inicial robusta
  Lectura();
  if ((EstadoPicoV3)() ) {
    Serial.println(F("❌ Error de lectura inicial (pico mal conectado). Salgo de Calibracion()."));  
    return;
  }

  // Delta inicial
  float delta = PresionMedidaPromedio - g_SetPoint;

  // ¿Ya estamos dentro de la banda?
  if (fabsf(delta) < DEADBAND_OK_PSI) {
    triplePitidoCorto();
    Mostrar_Presion(PresionRedondeada);
    reproducirMensaje(PresionRedondeada, 2500);
    return;
  }

  // Sentido de acción (SE RECALCULA EN CADA ITERACIÓN)
  bool inflado            = (delta < 0);
  bool huboCrucePrevio    = false;   // para histeresis en la banda fina



  // Si estamos lejos del setpoint, y aún no tenemos ratio medido para ese sentido, medir


  if ((delta > +3.0f) && (!ratioDesinfladoCalculado)) {
    float r = calculoRatio(false);
    ratioDesinfladoaUsar    = r;
    ratioDesinfladoCalculado = true;
    
  } else if ((delta < -3.0f) && (!ratioInfladoCalculado)) {
    float r = calculoRatio(true);
    ratioInfladoaUsar       = r;
    ratioInfladoCalculado    = true;
    
  }
  // Ratio base para empezar
  float ratioBase = inflado ? ratioInfladoaUsar : ratioDesinfladoaUsar;
  // Bucle principal: termina cuando se acaba el tiempo o logramos la banda
  ///******************************************************************* */

  while (!mostrarTiempoRestante(ServicioPago)) {
    // Reevaluar delta actual
    Lectura();
    Serial.println("Primer lectura");
    if (EstadoPicoV3()) {
     Serial.println(F("⚠️ Lectura inválida durante calibración. Intento siguiente ciclo..."));
      delay(250);
      break;
    }
    
    delta = PresionMedidaPromedio - g_SetPoint;
      Serial.print("Presion medida Promedio: ");
      Serial.println(PresionMedidaPromedio);
      Serial.print("Delta: ");
      Serial.println(delta);

    // ¿Listo?
    float banda = huboCrucePrevio ? DEADBAND_POST_CRUCE : DEADBAND_OK_PSI;
    if (fabsf(delta) < banda) {
      triplePitidoCorto();
      Mostrar_Presion(PresionRedondeada);
      reproducirMensaje(PresionRedondeada, 2500);
      break;
    }
 
    // Recalcular sentido CADA vuelta con delta actual
    bool sentidoPrevio = inflado;
    inflado = (delta < 0);

    // Si cambió el sentido -> cruzamos el setpoint
    if (inflado != sentidoPrevio) {
      Serial.println(F("⚠️ Cruce de setpoint detectado. Cambio de sentido + micropulso + ratio estándar."));
      huboCrucePrevio = true;
      contadorCruces++;

    }
    else huboCrucePrevio = false;

    // Cálculo del tiempo de activación (proporcional a |delta|/ratio)
    
    float tiempoActivacion = 1000.0f * fabsf(delta) /  (ratioBase); 

    // Si acabamos de cruzar, hacemos un micropulso adicional (más seguro)
    if (huboCrucePrevio) {
      tiempoActivacion = tiempoActivacion/contadorCruces;
      Serial.println("Rebotando");
    }
    float tiempoMax        = inflado ? TIEMPO_MAXIMO_VALVULA_INFLADO : TIEMPO_MAXIMO_VALVULA_DESINFLADO;

    tiempoActivacion       = constrain(tiempoActivacion, PULSO_MIN_MS, tiempoMax);

    // Modo fino si estamos a <= 1.0 PSI
    if (fabsf(delta) <= 1.0f) {
     // float proporcional = 1000.0f * fabsf(delta) / max(0.60f, ratioBase);
      tiempoActivacion   = constrain(tiempoActivacion, PULSO_MIN_MS, PULSO_MAX_FINO_MS);
      Serial.println("Sintonia fina (<1 psi): ");
      
    }


  // Log de estado

    Serial.printf("%s Δ=%.2f PSI | ratio=%.2f | pulso=%.0f ms | P=%.2f PSI | SP=%d\n",
                  inflado ? "🔺Inflar" : "🔻Desinflar",
                  delta, ratioBase, tiempoActivacion, PresionMedidaPromedio, g_SetPoint);
  Serial.printf(contadorCruces > 0 ? " (Cruces= %d)" : "\n", contadorCruces);
   
  // Ejecutar pulso
    int pin = inflado ? Valvula_Inflado : Valvula_Desinflado;
    digitalWrite(pin, HIGH);
    delay((unsigned long)tiempoActivacion);
    digitalWrite(pin, LOW);

    // Estabilización y muestro de presion parcial de inflado/desinflado
    delay(500);
    Mostrar_Presion(PresionRedondeada);

  }
}


