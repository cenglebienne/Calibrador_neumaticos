#include <Arduino.h>
#include <audio.h>
#include <config.h>
#include <constants.h>
#include <connections.h>
#include <display.h>
#include <esp_system.h>
#include <esp_ota_ops.h>
#include <fs_manager.h>
#include <memory_utils.h>
#include <mensajes.h>
#include <menu.h>
#include <mqtt_utils.h>
#include <Pulsadores.h>
#include <server.h>
#include <servicio.h>
#include <update_manager.h>
#include <logger.h>

volatile unsigned long pulseInTimeBegin = micros();
volatile unsigned long pulseInTimeEnd = micros();
unsigned long pulseDuration;
unsigned long _millisUltimaVezReconnect_MQTT = 0;
unsigned long _millisUltimaVezReconnect_WiFi;
unsigned long Ultimo_intento_FTP;
unsigned long ultimoLogDesconexion = 0;
const unsigned long intervaloMinimoLog = 20000; // 10 segundos
bool _POSBrindandoServicio = false;
bool g_levantar_AP;
bool g_Servicio_Cortesia = false;
bool go_prog = false;
bool serv_pressed = false;
bool g_entro_pulso = false;
bool ServicioPago = false;
unsigned long Ultimo_sensado = 0;
float g_temperatura;
float temp_actual;
unsigned long g_Tiempo_Ultimo_Refresco = 0;
bool isDFPlayerAvailable = true;
unsigned long tiempoPresionado = 0; // Tiempo que el botón ha estado presionado
unsigned long tiempoInicio2 = 0;    // Tiempo de inicio cuando se presiona el botón
unsigned long g_TimerStby = 0;


/* const char *mqttHost = "66.97.47.118";
const int mqttPort = 1883;                  // El puerto estándar sin SSL para MQTT es 1883
const char *mqttUser = "farinamqtt";        // Si es necesario
const char *mqttPassword = "fv#297esp8266"; // Si es necesario */

void connectToMqtt();
void IncrementoMiscontadores();
bool timeoutSegundos(unsigned long millisUltimaVez, unsigned long segundosEsperar);
void ICACHE_RAM_ATTR buttonPinInterrupt();

void reiniciar();
void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

void setup()
{
    pinMode(Data_Pin_1, OUTPUT);
    pinMode(Clock_Pin_1, OUTPUT);

    pinMode(Data_Pin_2, OUTPUT);
    pinMode(Clock_Pin_2, OUTPUT);
    pinMode(Valvula_Inflado, OUTPUT);
    pinMode(Valvula_Desinflado, OUTPUT);
    pinMode(Pin_Buzzer, OUTPUT);
    pinMode(PIN_HABILITACION, OUTPUT);
    pinMode(BUSY_PIN, INPUT_PULLUP); // Configurar el pin BUSY como entrada
    pinMode(P_Prog, INPUT);
    pinMode(Pin_Sensor, INPUT);
    pinMode(pin_moneda, INPUT); // Le saqué la resistencia de pull up porque le pongo una externa

    pinMode(Pin_Pulsador_1, INPUT);
    pinMode(Pin_Pulsador_2, INPUT);

    //  digitalWrite(Pin_Pulsador_1, HIGH);
    //  digitalWrite(Pin_Pulsador_2, HIGH);
    digitalWrite(PIN_HABILITACION, LOW); // PONE EL PIN DEL HABILITACION DEL MOMEDERO POR DEFAULT EN BAJO

    attachInterrupt(digitalPinToInterrupt(pin_moneda), buttonPinInterrupt, CHANGE);

    Serial.begin(115200);
    Serial.println("Starting...");
    delay(500);
    digitalWrite(Pin_Buzzer, HIGH);
    delay(100);
    digitalWrite(Pin_Buzzer, LOW);
    delay(20);
    digitalWrite(Pin_Buzzer, HIGH);
    delay(100);
    digitalWrite(Pin_Buzzer, LOW);
    delay(20);
    
    g_Tiempo_Ultimo_Refresco = millis();
    g_timer_mensajes = millis();
    //// MANEJO DE MEMORIA////////////////////////
    Get_Flash_All();

    if ((Misvalores.Tiempo_Programado > 10) || (Misvalores.Tiempo_Programado < 1))
    {
        Misvalores.Tiempo_Programado = 5;
        Save_Flash_ValorServicio();
    }

    if (strcmp(Mientorno.EQUIPO_inicializado, "123456789") != 0)
    {
        Serial.println("Inicializando EEPROM");
        Inicializar_Flash();
    }
    delay(2000);
    Serial.println("Lectura de memoria Flash.....");
    Serial.println(String(Mientorno.CLIENTE_WIFI_ssid));
    Serial.println(String(Mientorno.CLIENTE_WIFI_clave));
    Serial.println("Cliente WIFI Hab: " + String(Mientorno.CLIENTE_WIFI_Habilitado));
    Serial.println("Segundos de espera reconexion:" + String(Mientorno.CLIENTE_WIFI_segundosEsperarConexion));
    Serial.println("Levantar AP: " + String(g_levantar_AP));
    Serial.println("Tiempo Programado: " + String(Misvalores.Tiempo_Programado));
    Serial.println("Compresor: " + String(Misvarios.Compresor));
    Serial.println("Audio Encendido: " + String(Misvarios.Audio_Encendido));
    Serial.println("Update Firmware pendiente: " + String(Miupdate.updateFirmwarePendiente));
    Serial.println("Update Fw al iniciar: " + String(Miupdate.updateFirmwareAlIniciar));
    Serial.println("Update Fw URL: " + String(Miupdate.urlFirmware[CAMPO_URL_LENGTH]));
    Serial.println("Update FS pendiente: " + String(Miupdate.updateFilesystemPendiente));
    Serial.println("Update FS al iniciar: " + String(Miupdate.updateFilesystemAlIniciar));
    Serial.println("Update FS URL: " + String(Miupdate.urlFilesystem[CAMPO_URL_LENGTH]));
    Serial.println("Servicio Gratis: " + String(Misvalores.Servicio_Gratis));
    Serial.println("Servidor MQTT: " + String(Mientorno.MQTT_servidor));
    Serial.println("Topic: " + String(Mientorno.MQTT_topic));
    Serial.println("Usuario: " + String(Mientorno.MQTT_usuario));
    Serial.println("Clave MQTT" + String(Mientorno.MQTT_clave));
    Serial.println(Misvalores.Version_YPF ? "Version YPF" : "Version Std");

    // String clientID = String(Mientorno.EQUIPO_serie[10]);
    // const char* client_id = clientID.c_str();

    switch (Misvarios.Compresor)
    {
    case 1:
        ratioStandardInflado = 0.5f;
        break;

    case 2:
        ratioStandardInflado = 1.5f; 
        break;

    case 3:
        ratioStandardInflado = 2.5f;
        break;

    default:
        // Valor por defecto
        ratioStandardInflado = 1.5f;
        break;
    }

    ////// ARRANQUE DE DISPLAY Y  LCD
    char versionStr[8];
    snprintf(versionStr, sizeof(versionStr), "%d.%02d",
         VERSION_FIRMWARE2 / 100,
         VERSION_FIRMWARE2 % 100);

    setupLCD();
    lcd.print("V");
    lcd.print(versionStr);
    lcd.print(" N:");
    lcd.print(Mientorno.EQUIPO_serie);

    // lcd.print("Ver" + (String)VERSION_FIRMWARE + " N:" + String(Mientorno.EQUIPO_serie));
    lcd.setCursor(0, 1);
    if (Misvalores.Version_YPF)
        displayCenteredMessage("VERSION YPF", 1);
    else
        displayCenteredMessage("VERSION STANDARD", 1);

    int x = 88;
    Mostrar_Presion(x);
    delay(1000);
    Mostrar_Rayita();
    delay(1000);

    Serial.println("Nro de serie: " + String(Mientorno.EQUIPO_serie));
    Serial.println("Version de Firmware: " + (String)versionStr);

    strcpy(Mientorno.MQTT_topic, "FarinaVending"); /// SACAR al encontrar error

    String _apAlQueEstoyConectado = "";

    //// File System////////////////////////////
    if (!LittleFS.begin())
        Serial.println("An error has occurred while mounting LittleFS");
    else
        Serial.println("LittleFS mounted successfully.");

    // Usar métodos directos para obtener información del sistema de archivos
    size_t totalBytes = LittleFS.totalBytes();
    size_t usedBytes = LittleFS.usedBytes();

    inicializarLogs();

    Serial.println("File System Info:");
    Serial.print("Total Bytes: ");
    Serial.println(totalBytes);
    Serial.print("Used Bytes: ");
    Serial.println(usedBytes);

    // Listar archivos en el directorio raíz
    File root = LittleFS.open("/");
    if (!root)
    {
        Serial.println("Failed to open directory");
    }

    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
    }

    File file = root.openNextFile();
    while (file)
    {
        Serial.print("FILE: ");
        Serial.print(file.name());
        Serial.print("\tSIZE: ");
        Serial.println(file.size());
        file = root.openNextFile();
    }
    g_archivo_lleno = logFileFull();
    //////////////////conexion wi y seteo reloj

    setupWiFi();
    Serial.print("Conectando a Wi-Fi");

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 7000; // Máx 7 segundos para conectar

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout)
    {
        delay(500);
        Serial.print(".");
    }
    String mensaje = "Power ON";
    logEvent(mensaje);

    if (WiFi.status() == WL_CONNECTED)
    {
        if (inicializarNTP())
        {
            mensaje = "✅ Wi-Fi OK | Hora sincronizada";
        }
        else
        {
            mensaje = "⚠️ Wi-Fi OK pero sin sincronizar hora";
        }

        /* Serial.println("Conectado a Wi-Fi");
         configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
         Serial.println("NTP configurado");
         */

        String ssid = WiFi.SSID();
        int32_t rssi = WiFi.RSSI();

        mensaje = "✅ Conectado a Wi-Fi SSID: " + ssid +
                  " | RSSI: " + String(rssi) + " dBm";

        logEvent(mensaje);
    }
    else
    {
        Serial.println("No se pudo conectar a Wi-Fi. El sistema continuará sin hora.");
    }

    WiFi.onEvent(onWiFiEvent); // ← Esto activa el "hook"

    ////// DFPlayer////////////////////////////////////
    mySerial.begin(9600, SERIAL_8N1, 16, 17);
    int retries = 2; // Número de intentos para iniciar DFPlayer
    while (!myDFPlayer.begin(mySerial) && retries > 0)
    {
        Serial.println("Failed to init DFPlayer, retrying...");
        delay(1000); // Espera entre intentos
        retries--;
    }

    if (retries == 0)
    {
        Serial.println("Failed to init DFPlayer, disabling device...");
        isDFPlayerAvailable = false; // Deshabilita DFPlayer si no se pudo iniciar
    }
    else
    {
        Serial.println("DFPlayer Mini is online.");
        myDFPlayer.volume(30); // Valor De Volumen: (0 ~ 30).
    }
    // testDeSonido();

    ////////////////////////////////////////////////////// Arranque

    if (Get_Flash_Bool("Levantar_AP"))
    {
        levantarAP();
        // justo antes de WiFi.softAPConfig(...)

        Serial.println("Iniciando Modo AP");
        clearLCD();
        displayCenteredMessage("MODO", 0);
        displayCenteredMessage("ACCESS POINT", 1);
        Set_Flash_Bool("Levantar_AP", false);
        g_APLevantado = true;
        /*
         IPAddress local_ip(192, 168, 0, 1);
         IPAddress gateway(192, 168, 0, 1); // Normalmente es la misma IP que el AP en modo AP
         IPAddress subnet(255, 255, 255, 0);
         WiFi.disconnect(true, true); // borra estado/keys, detiene STA
         WiFi.mode(WIFI_OFF);
         delay(100);
         WiFi.mode(WIFI_AP); // o WIFI_AP_STA si querés mantener STA
         bool ok = WiFi.softAPConfig(local_ip, gateway, subnet);
         ok &= WiFi.softAP("Farinavending", "expendedoras");
         Serial.println(ok ? "AP up" : "AP failed");
         Serial.printf("Mode=%d, AP IP=%s\n", WiFi.getMode(), WiFi.softAPIP().toString().c_str());
         // Iniciar el AP con la IP estática
         WiFi.softAPConfig(local_ip, gateway, subnet);

         const char *ssid = "Farinavending";
         const char *password = "expendedoras";
         WiFi.softAP(ssid, password);
         Serial.print("Access Point \"" + String(ssid) + "\" started\n");
         Serial.print("IP Address: ");
         Serial.println(WiFi.softAPIP()); */

        Miwebserver.on("/", HTTP_GET, []()
                       {
    if (!handleFileRead("/index.html")) {
      Miwebserver.send(404, "text/plain", "404: Not Found");
    } });

        Miwebserver.on("/js/script.js", HTTP_GET, []()
                       {
    if (!handleFileRead("/js/script.js")) {
      Miwebserver.send(404, "text/plain", "404: Not Found");
    } });
        Miwebserver.on("/css/w3.css", HTTP_GET, []()
                       {
    if(!handleFileRead("/css/w3.css")){
        Miwebserver.send(404, "text/plain", "404: Not Found");
    }; });
        /*   Miwebserver.on("/", HTTP_GET, []() {
            handleFileRead("/");
          });
          Miwebserver.on("/css/w3.css.gz", HTTP_GET, []() {
            handleFileRead("/css/w3.css.gz");
          });
          Miwebserver.on("/js/script.js.gz", HTTP_GET, []() {
            handleFileRead("/js/script.js.gz");
          }); */

        // Handle 404 Not Found
        Miwebserver.onNotFound([]()
                               {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += Miwebserver.uri();
    message += "\nMethod: ";
    message += (Miwebserver.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += Miwebserver.args();
    message += "\n";
    for (uint8_t i = 0; i < Miwebserver.args(); i++) {
      message += " " + Miwebserver.argName(i) + ": " + Miwebserver.arg(i) + "\n";
    }
    Serial.print(message);
    Miwebserver.send(404, "text/plain", message); });
        // Miwebserver.onNotFound(handleNotFound);
        Miwebserver.on("/estadoCliente.json", handleEnviarEstadoCliente);
        Miwebserver.on("/APdisponibles.json", handleEnviarAPDisponibles);
        Miwebserver.on("/grabarConfigCliente.php", handleGrabarConfigCliente);

        Miwebserver.on("/estadoGPRS.json", handleEnviarEstadoGPRS);
        Miwebserver.on("/grabarConfigGPRS.php", handleGrabarConfigGPRS);

        Miwebserver.on("/estadoMQTT.json", handleEnviarEstadoMQTT);
        Miwebserver.on("/grabarConfigMQTT.php", handleGrabarConfigMQTT);

        Miwebserver.on("/estadoContadores.json", handleEnviarEstadoContadores);
        Miwebserver.on("/grabarConfigContadores.php", handleGrabarConfigContadores);

        Miwebserver.on("/estadoEquipo.json", handleEnviarEstadoEquipo);
        Miwebserver.on("/grabarConfigEquipo.php", handleGrabarConfigEquipo);

        Miwebserver.on("/estadoValores.json", handleEnviarEstadoValores);
        Miwebserver.on("/grabarConfigValores.php", handleGrabarConfigValores);

        Miwebserver.on("/estadoAcerca.json", handleEnviarEstadoAcerca);

        Miwebserver.on("/reiniciar.php", handleReiniciar);

        Miwebserver.begin();
        g_webServerHabilitado = true;

        Serial.println("Servidor HTTP iniciado");
    }
    ////////////////////////FIN WEBSERVER///////////////////////////////////////////
    else
    // Arranque normal
    {

        // Setup MQTT
        mqttClient.onConnect(onMqttConnect);
        mqttClient.onDisconnect(onMqttDisconnect);
        mqttClient.onMessage(onClienteMQTT_WiFi_Message);
        mqttClient.setServer(mqttHost, mqttPort);
        mqttClient.setCredentials(mqttUser, mqttPassword);
        mqttClient.onSubscribe(onmqttClient_Subscribe);
        mqttClient.onUnsubscribe(onmqttClient_Unsubscribe);
        mqttClient.onPublish(onmqttClient_Publish);
        mqttClient.setKeepAlive(30); // se cambio para ver si mejora la estabilidad de la conexion con MQTT
        mqttClient.setClientId(Mientorno.EQUIPO_serie);
        Serial.println("  SSID: " + (String)Mientorno.CLIENTE_WIFI_ssid);
        Serial.println("  Clave: " + (String)Mientorno.CLIENTE_WIFI_clave);

        //  delay(1000);    //espera a que se conecte
        // checkWiFiConnection();

        if (Miupdate.updateFirmwareAlIniciar || Miupdate.updateFilesystemAlIniciar)
        {

            Serial.println("Se encontró un pedido de actualización de Firmware y/o Filesystem");
            bool Intentar = true;
            _millisUltimaVezReconnect_WiFi = millis();
            while (Intentar)
            {
                if (timeoutSegundos(_millisUltimaVezReconnect_WiFi, 120))
                {
                    Serial.println("Sin conexión WiFi por el momento. Se descartan los pedidos de actualización pendiente");
                    Miupdate.updateFirmwareAlIniciar = false;
                    Miupdate.updateFilesystemAlIniciar = false;
                    Save_Flash_Actualizar();
                    reiniciar();
                    Intentar = false;
                }

                if (WiFi.status() == WL_CONNECTED)
                {
                    delay(1000);
                    Serial.println("Conectado a Internet, se intentará actualizar el sistema");

                    if (Miupdate.updateFirmwareAlIniciar)
                    {
                        displayCenteredMessage("AGUARDE ...", 0);
                        displayCenteredMessage("ACT. FIRMWARE", 1);
                        Miupdate.updateFirmwareAlIniciar = false;
                        Save_Flash_Actualizar();
                        actualizarFirmwareOTA();
                        Intentar = false;
                    }
                    if (Miupdate.updateFilesystemAlIniciar)
                    {
                        Miupdate.updateFilesystemAlIniciar = false;
                        Save_Flash_Actualizar();
                        actualizarFilesystemOTA();
                        Intentar = false;
                    }
                }
            }
        }
    }
}
void loop()
{

    static bool PP;
    if (!g_APLevantado)
    {

        if (Miupdate.updateFirmwarePendiente || Miupdate.updateFilesystemPendiente)
        {
            if (WiFi.status() == WL_CONNECTED)
            {
                if (Miupdate.updateFirmwarePendiente)
                {
                    Serial.println("URL:" + String(Miupdate.urlFirmware));
                    Miupdate.updateFirmwarePendiente = false;
                    Miupdate.updateFirmwareAlIniciar = true;
                    Save_Flash_Actualizar();
                    delay(500);
                    Serial.println("Reiniciando para actualizar firmware");
                    reiniciar();
                }
                else
                { // updateFilesystemPendiente = true
                    Miupdate.updateFilesystemPendiente = false;
                    Miupdate.updateFilesystemAlIniciar = true;
                    Save_Flash_Actualizar();
                    reiniciar();
                }
            }
        }
        checkWiFiConnection();
        checkMqttConnection();

        static uint32_t Ultimo_intento_FTP = 0;

        if (WiFi.status() == WL_CONNECTED && (g_enviar_log_Por_FTP || g_archivo_lleno) &&
            elapsed_ms(Ultimo_intento_FTP, TIEMPO_ENTRE_ENVIOS))

        {
            Ultimo_intento_FTP = millis();
            String remoteName = generarNombreRemotoLog();

            // SUBIDA EN BLOQUES (cede CPU):

            bool automatico = g_archivo_lleno;

            if (enviarArchivoPorFTP(LOG_ACTUAL, remoteName, automatico, FTP_SUBDIR))
            {
                // Pequeña cesión antes de tocar FS
                vTaskDelay(1);

                File nuevo = LittleFS.open(LOG_ACTUAL, FILE_WRITE);
                if (nuevo)
                {
                    Serial.println("Archivo enviado por FTP.");
                    nuevo.println("⏱️ Log iniciado tras envío por FTP");
                    nuevo.close();
                    g_archivo_lleno = false;
                }
                g_enviar_log_Por_FTP = false;
                // Cede CPU después de FS también
                vTaskDelay(1);
            }
            else
            {
                Serial.println("⚠️ No se pudo enviar el archivo.");
                vTaskDelay(1);
            }
        }

        ///////// Acá comienza la parte particular del calibrador

        if (millis() >= g_timer_mensajes)
        {
            mensajes();
        }

        //******************************
        if ((Misvalores.Servicio_Gratis) && (!Misvalores.Version_YPF))
        {
            g_pago_qr_disponible = false; // en servicio gratis deshabilito el pago electronico
            digitalWrite(PIN_HABILITACION, HIGH);
            Blink();
        }
        else
        {
            g_pago_qr_disponible = true;
            digitalWrite(PIN_HABILITACION, LOW);
            if ((millis() - g_Tiempo_Ultimo_Refresco) > TIEMPO_REFRESCO_7SEG)
                Mostrar_Rayita();
        }

        if ((digitalRead(Pin_Pulsador_1) == LOW) || (digitalRead(Pin_Pulsador_2) == LOW))
        {
            if ((Misvalores.Servicio_Gratis) || ((Misvalores.Version_YPF) && (!g_conectadoMQTT)))
            {
                g_TimerStby = millis();
                ServicioPago = false;
                Servicio(ServicioPago);
            }
            else
            {
                if (Misvalores.Version_YPF)
                    reproducirMensaje(80, 1000);
                else
                    mensajeIndicacion();
            }
        }

        if ((g_cospel_in) || (g_pagoelectronico) || (g_Servicio_Cortesia))
        {
            Serial.println("Entró pago");
            buzzerCorto();
            if (g_cospel_in)
                logEvent("Ingresó ficha");
            // digitalWrite(PIN_HABILITACION, HIGH);

            // g_pago_qr_disponible = false; // deshabilito el pago online mientras está dando servicio
            IncrementoMiscontadores();
            lcd.clear();
            ServicioPago = true;
            Servicio(ServicioPago);
        }

        int estadoBoton = digitalRead(P_Prog);

        if (estadoBoton == LOW)
        { // Botón presionado (estado LOW con INPUT_PULLUP)
            if (tiempoInicio2 == 0)
            { // Registrar el tiempo de inicio una vez
                tiempoInicio2 = millis();
            }

            // Calcular el tiempo que el botón ha estado presionado
            tiempoPresionado = millis() - tiempoInicio2;
        }
        else
        { // Botón no presionado
            if (tiempoInicio2 != 0)
            { // Solo si el botón fue presionado anteriormente
                // Evaluar el tiempo que el botón estuvo presionado
                if (tiempoPresionado >= 100 && tiempoPresionado <= 5000)
                {
                    // Si el botón estuvo presionado entre 100 ms y 5000 ms
                    sub_prog();
                }
                else if (tiempoPresionado > 5000)
                {
                    // Si el botón estuvo presionado más de 5000 ms
                    Misvalores.Version_YPF = !Misvalores.Version_YPF; // Invertir el valor booleano
                    Set_Flash_Bool("Version_YPF", Misvalores.Version_YPF);
                    lcd.clear();
                    if (Misvalores.Version_YPF)
                        displayCenteredMessage("VERSION YPF", 0);
                    else
                        displayCenteredMessage("VERSION STANDARD", 0);
                    delay(2000);
                }
            }

            // Reiniciar los tiempos después de soltar el botón
            tiempoInicio2 = 0;
            tiempoPresionado = 0;
        }
    }
    Miwebserver.handleClient();
} // loop

void reiniciar()
{
    Serial.println("Reiniciando...");
    lcd.clear();
    displayCenteredMessage("Reiniciando...", 0);

    if (mqttClient.connected())
    {
        mqttClient.disconnect();
        Serial.println("Desconectando MQTT");
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFi.disconnect();
        Serial.println("Desconectando WIFI");
    }

    delay(2000);
    LittleFS.end();
    esp_restart(); // Llama a esta función para reiniciar el ESP32
    delay(2000);
}

void ICACHE_RAM_ATTR buttonPinInterrupt()

{
    if (digitalRead(pin_moneda) == LOW)
    {
        // start measuring
        pulseInTimeBegin = micros();
    }
    else
    {
        // stop measuring
        pulseInTimeEnd = micros();
        pulseDuration = pulseInTimeEnd - pulseInTimeBegin;
        if ((((pulseDuration > MinimoPulso1) && (pulseDuration < MinimoPulso1 + Tolerancia))) || (((pulseDuration > MinimoPulso2) && (pulseDuration < MinimoPulso2 + Tolerancia))))
        {
            g_cospel_in = true;
            g_entro_pulso = false;
        }
        else
            g_entro_pulso = true;
    }
}

bool timeoutSegundos(unsigned long millisUltimaVez, unsigned long segundosEsperar)
{
    yield();
    return ((unsigned long)(millis() - millisUltimaVez) > (segundosEsperar * 1000));
}

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_READY:
        Serial.println("WiFi ready");
        break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        Serial.println("WiFi scan done");
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("WiFi STA start");
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        Serial.println("WiFi STA stop");
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("Connected to AP");
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    {
        static int desconexionesWifi = 0;
        desconexionesWifi++;

        if (desconexionesWifi == 1 || desconexionesWifi % 20 == 0)
        {
            uint8_t reason = info.wifi_sta_disconnected.reason;
            String ssidIntentada = Mientorno.CLIENTE_WIFI_ssid;
            String passwordIntentada = Mientorno.CLIENTE_WIFI_clave;

            String mensaje = "Intento fallido de conexión a SSID: " + ssidIntentada +
                             " | Pass: " + passwordIntentada +
                             " | Razón: " + razonDesconexionWifi(reason);

            logEvent(mensaje);
            Serial.println(mensaje);
        }

        onWifiDisconnect();
        break;
    }

    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        Serial.println("Authentication mode of AP changed");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    {
        String ssid = WiFi.SSID();
        int32_t rssi = WiFi.RSSI();
        IPAddress ip = IPAddress(info.got_ip.ip_info.ip.addr);

        String mensaje = "✅ Conectado a Wi-Fi SSID: " + ssid +
                         " | IP: " + ip.toString() +
                         " | RSSI: " + String(rssi) + " dBm";

        logEvent(mensaje);
        Serial.println(mensaje);
        onWifiConnect();
        break;
    }
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Serial.println("Lost IP address and IP address is reset to 0");
        break;
    case ARDUINO_EVENT_WIFI_AP_START:
        Serial.println("AP started");
        break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
        Serial.println("AP stopped");
        break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        Serial.print("Client connected to AP. MAC: ");
        Serial.println(WiFi.macAddress());
        break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        Serial.println("Client disconnected from AP");
        break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
        Serial.println("AP assigned IP to client");
        break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
        Serial.println("AP received probe request");
        break;
    case ARDUINO_EVENT_WIFI_FTM_REPORT:
        Serial.println("FTM report received");
        break;
    case ARDUINO_EVENT_ETH_START:
        Serial.println("Ethernet started");
        break;
    case ARDUINO_EVENT_ETH_STOP:
        Serial.println("Ethernet stopped");
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        Serial.println("Ethernet connected");
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        Serial.println("Ethernet disconnected");
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        Serial.println("Ethernet got IP");
        break;
    default:
        Serial.print("Unknown event: ");
        Serial.println(event);
        break;
    }
}