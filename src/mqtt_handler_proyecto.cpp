#include "mqtt_handlers.h"
#include "memory_api.h"
#include "app_state.h"
#include "config.h"
#include <LittleFS.h>
#include "wifi_compat.h"

extern AsyncMqttClient mqttClient;

bool onMqttComandoProyecto(char cmd, char *payload, size_t len,
                            char *buf, size_t cap, size_t *inOutUsed,
                            const char *topicPTS) {
  auto used   = [&]() -> size_t { return *inOutUsed; };
  auto room   = [&](size_t n) -> bool { return *inOutUsed + n <= cap; };
  auto ensure = [&](size_t n) -> bool {
    if (!room(n)) {
      Serial.printf("MQTT proyecto: buffer lleno (need %u, free %u)\n",
                    (unsigned)n, (unsigned)(cap - *inOutUsed));
      return false;
    }
    return true;
  };
  auto writePtr = [&]() -> char * { return buf + *inOutUsed; };
  auto advance  = [&](size_t n) { *inOutUsed += n; };

  switch (cmd) {
    case 'C':
      Serial.println("Comando C: Cambio de configuracion...");
      Misvalores.Servicio_Gratis   = (payload[5] == 'S');
      Misvalores.Tiempo_Programado = convertirLittleEndianAEntero(payload + 6);
      Misvalores.Version_Carrousel = (payload[10] == 'Y') ? 1 : 0;
      Save_Flash_ValorServicio();
      if (!ensure(1)) return true;
      *writePtr() = 'S';
      advance(1);
      return true;

    case 'D': {
      Serial.println("Comando D: Pedido de datos generales...");
      unsigned int versionFilesystem = 0;
      File archivoFS = LittleFS.open("/versionfilesystem.txt", "r");
      if (archivoFS) {
        versionFilesystem = (unsigned int)archivoFS.readString().toInt();
        archivoFS.close();
      }
      if (!ensure(2)) return true;
      memcpy(writePtr(), &VERSION_FIRMWARE, 2); advance(2);

      if (!ensure(2)) return true;
      memcpy(writePtr(), &versionFilesystem, 2); advance(2);

      if (!ensure(1)) return true;
      *writePtr() = (Misvalores.Servicio_Gratis ? 'S' : 'N'); advance(1);

      if (!ensure(sizeof(uint32_t))) return true;
      memcpy(writePtr(), &Misvalores.Tiempo_Programado, sizeof(uint32_t)); advance(sizeof(uint32_t));

      if (!ensure(1)) return true;
      *writePtr() = (uint8_t)Misvalores.Version_Carrousel; advance(1);

      String mac = WiFi.macAddress();
      mac.replace(":", "");
      mac.toLowerCase();
      size_t macLen = mac.length();
      if (!ensure(macLen + 2)) return true;
      memcpy(writePtr(), mac.c_str(), macLen); advance(macLen);
      buf[*inOutUsed + 1] = '\0';
      advance(2);
      return true;
    }

    case 'V':
      Serial.println("Comando V: Cambio de opciones de configuracion...");
      if (len >= 8) {
        // [5..6] flags (uint16, little-endian):
        // bit0=Servicio_Gratis, bit1-2=Version_Carrousel (0-3),
        // bit3=Sensor_Temp_Activado, bit4=reservado(WiFi-solo local),
        // bit5=Display_I2C, bit6=Opcion_Extra_1, bit7=Opcion_Extra_2, bit8=Opcion_Extra_3
        // [7] = Tiempo_Programado (uint8, 0-255 segundos)
        uint16_t flags;
        memcpy(&flags, payload + 5, 2);
        Misvalores.Servicio_Gratis    = (flags >> 0) & 1;
        Misvalores.Version_Carrousel  = (flags >> 1) & 0x03;
        Misvalores.Sensor_Temp_Activado = (flags >> 3) & 1;
        Misvalores.Display_I2C        = (flags >> 5) & 1;
        Misvalores.Opcion_Extra_1     = (flags >> 6) & 1;
        Misvalores.Opcion_Extra_2     = (flags >> 7) & 1;
        Misvalores.Opcion_Extra_3     = (flags >> 8) & 1;
        Misvalores.Tiempo_Programado  = (uint8_t)payload[7];
        Save_Flash_ValorServicio();
        if (!ensure(1)) return true;
        *writePtr() = 'S'; advance(1);
        if (g_enServicio) {
          Serial.println("Comando V diferido: esperando fin de servicio.");
          g_reinicio_pendiente = true;
        } else {
          mqttClient.publish(topicPTS, 2, false, buf, *inOutUsed);
          delay(300);
          ESP.restart();
        }
      } else {
        if (!ensure(1)) return true;
        *writePtr() = 'N'; advance(1);
      }
      return true;

    default:
      return false;
  }
}
