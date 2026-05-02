#include "pins.h"
#include <mediciones.h>
#include <Arduino.h>

#ifdef DEBUG
  #define DBG_PRINTLN(x)  Serial.println(x)
  #define DBG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DBG_PRINTLN(x)
  #define DBG_PRINTF(...)
#endif

// Output globals — escritas por EstadoPicoV3() al obtener lectura estable
float   PresionMedidaPromedio = 0.0f;
uint8_t PresionRedondeada     = 0;
float   g_Presion_PSI         = 0.0f;

// Constantes del sensor de presión
static constexpr float VOLTAJE_MIN  = 0.0066f; // V
static constexpr float VOLTAJE_MAX  = 3.3f;    // V
static constexpr float PRESION_MIN  = 1.0f;    // kPa
static constexpr float PRESION_MAX  = 500.0f;  // kPa
static constexpr long  T_ENTRE_LECT = 50;      // ms entre muestras

// Lee el ADC y actualiza g_Presion_PSI
void Lectura()
{
    int   mv      = analogReadMilliVolts(Pin_Sensor);
    float voltaje = (mv / 4095.0f) * 3.30f;
    float kpa     = ((voltaje - VOLTAJE_MIN) / (VOLTAJE_MAX - VOLTAJE_MIN))
                    * (PRESION_MAX - PRESION_MIN) + PRESION_MIN;
    g_Presion_PSI = kpa * 0.14503773773375f;
}

// Función no bloqueante — llamar desde loop() en cada iteración.
//
// Primera llamada: inicializa la medición.
// Llamadas siguientes: toma una muestra cada T_ENTRE_LECT ms.
// Tras retornar ESTABLE o INESTABLE, la siguiente llamada reinicia automáticamente.
//
// Retorna:
//   ResultadoMedicion::EN_CURSO   — medición en progreso, seguir llamando
//   ResultadoMedicion::ESTABLE    — lectura válida; PresionMedidaPromedio y
//                                   PresionRedondeada fueron actualizados
//   ResultadoMedicion::INESTABLE  — se agotaron los intentos sin lectura estable
ResultadoMedicion EstadoPicoV3()
{
    static constexpr int   VENTANA    = 8;
    static constexpr int   MAX_LECT   = 50;
    static constexpr float TOLERANCIA = 0.5f;

    static float    buf[VENTANA];
    static int      head     = 0;
    static int      count    = 0;
    static int      lecturas = 0;
    static uint32_t t_ultima = 0;
    static bool     activa   = false;

    uint32_t now = millis();

    // Inicialización en la primera llamada (o tras resultado terminal)
    if (!activa)
    {
        memset(buf, 0, sizeof(buf));
        head     = 0;
        count    = 0;
        lecturas = 0;
        t_ultima = now;
        activa   = true;
        digitalWrite(Valvula_Inflado,    LOW);
        digitalWrite(Valvula_Desinflado, LOW);
        DBG_PRINTLN("EstadoPicoV3: iniciando medicion");
        return ResultadoMedicion::EN_CURSO;
    }

    // Esperar el intervalo entre muestras
    if (now - t_ultima < (uint32_t)T_ENTRE_LECT)
        return ResultadoMedicion::EN_CURSO;

    t_ultima = now;
    Lectura();

    // Actualizar ring buffer
    buf[head] = g_Presion_PSI;
    head      = (head + 1) % VENTANA;
    if (count < VENTANA) count++;
    lecturas++;

    // Evaluar ventana completa
    if (count == VENTANA)
    {
        float sum = 0;
        for (int j = 0; j < VENTANA; j++) sum += buf[j];
        float media = sum / VENTANA;

        float var = 0;
        for (int j = 0; j < VENTANA; j++)
        {
            float d = buf[j] - media;
            var += d * d;
        }
        float sigma = sqrtf(var / VENTANA);

        if (sigma < TOLERANCIA && media >= 5.0f)
        {
            PresionMedidaPromedio = media;
            PresionRedondeada     = (uint8_t)roundf(media);
            activa                = false;
            DBG_PRINTF("Presion estable: %.2f PSI -> %u PSI\n",
                       PresionMedidaPromedio, PresionRedondeada);
            return ResultadoMedicion::ESTABLE;
        }
    }

    // Agotar intentos
    if (lecturas >= MAX_LECT)
    {
        activa = false;
        DBG_PRINTLN("Presion inestable o fuera de rango");
        return ResultadoMedicion::INESTABLE;
    }

    return ResultadoMedicion::EN_CURSO;
}
