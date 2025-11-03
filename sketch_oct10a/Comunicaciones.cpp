#include "Comunicaciones.hpp"
#include "config.hpp"
#include "LedNotifier.hpp" // <-- INCLUSIÓN DE LA NUEVA CLASE

#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include <Arduino.h>

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

WiFiClient espClient;
PubSubClient client(espClient);

extern LedNotifier ledNotifier; // <-- DECLARACIÓN EXTERNA DE LA INSTANCIA

// ========================================================
// === CALLBACK DE MQTT ===
// ========================================================
void CommsManager::callback(char* topic, byte* message, unsigned int length) {
    Serial.print("📩 Mensaje recibido en [");
    Serial.print(topic);
    Serial.print("]: ");

    String mensaje = "";
    for (unsigned int i = 0; i < length; i++) {
        mensaje += (char)message[i];
    }
    Serial.println(mensaje);

    // === Control del LED integrado (GPIO 2) ===
    if (String(topic) == TOPIC_SUB) {
        
        // ** NUEVA LÍNEA: ENCIENDE EL LED E INICIA EL TEMPORIZADOR DE PARPADEO **
        ledNotifier.notificarRecepcion(); 
        Serial.println("💡 LED (GPIO 2) ACTIVADO por paquete recibido.");

        if (mensaje.equalsIgnoreCase("LED_OFF")) {
            // Si el comando es APAGAR, lo apagamos inmediatamente
            digitalWrite(PIN_LED_RUIDO, LOW); 
            Serial.println("💡 LED (GPIO 2) APAGADO por comando 'LED_OFF'.");
        } else if (mensaje.equalsIgnoreCase("LED_ON")) {
            // Mantiene el LED encendido (la función notificarRecepcion() ya lo encendió)
            Serial.println("Comando 'LED_ON' recibido.");
        } else {
            Serial.println("⚠️ Comando desconocido, se activó el parpadeo de recepción.");
        }
    }
}

// ========================================================
// === CONEXIÓN WIFI ===
// ========================================================
void CommsManager::setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n✅ WiFi conectado");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

// ========================================================
// === CONEXIÓN MQTT ===
// ========================================================
void CommsManager::reconnect_mqtt() {
    while (!client.connected()) {
        Serial.print("Intentando conexión MQTT...");
        if (client.connect("ESP32Client", MQTT_USER, MQTT_PASS)) {
            Serial.println("conectado ✅");
            client.subscribe(TOPIC_SUB);
            Serial.print("📡 Suscrito a: ");
            Serial.println(TOPIC_SUB);
        } else {
            Serial.print("falló, rc=");
            Serial.print(client.state());
            Serial.println(" intentando de nuevo en 5 segundos");
            delay(5000);
        }
    }
}

// ========================================================
// === NTP ===
// ========================================================
void CommsManager::inicializarNTP() {
    Serial.println("-> PASO 1.1: Configurando NTP...");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10000)) {
        Serial.println("-> PASO 1.2: ❌ No se pudo obtener la hora del servidor NTP.");
    } else {
        Serial.println("-> PASO 1.2: ✅ Hora sincronizada.");
    }
}

String CommsManager::obtenerTiempoISO8601() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "";
    }
    char timeString[25];
    strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
    return String(timeString);
}

// ========================================================
// === INICIALIZACIÓN Y PUBLICACIÓN ===
// ========================================================
void CommsManager::inicializarComunicaciones() {
    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);

    if (WiFi.status() == WL_CONNECTED) {
        inicializarNTP();
    }
}

void CommsManager::mantenerConexion() {
    if (!client.connected()) {
        Serial.println("🔴 Cliente MQTT desconectado. Intentando reconexión...");
        reconnect_mqtt();
    }
    client.loop();
}

bool CommsManager::publicar(const char* topic, const String& payload) {
    return client.publish(topic, payload.c_str());
}