#include "Comunicaciones.hpp"
#include "config.hpp"
#include "StatusNotifier.hpp"

#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <time.h>
#include <Arduino.h>

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

WiFiClient espClient;
PubSubClient client(espClient);

extern StatusNotifier statusNotifier;
extern CommsManager commsManager;

// ========================================================
// === CALLBACK MQTT (COMANDOS) ===
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

    if (String(topic) == TOPIC_SUB) {

        String current_time = commsManager.obtenerTiempoISO8601();
        statusNotifier.notificarRecepcion(current_time, mensaje);

        if (mensaje.equalsIgnoreCase("LED_OFF")) {
            digitalWrite(PIN_LED_RUIDO, LOW);
        } 
        else if (mensaje.equalsIgnoreCase("LED_ON")) {
            digitalWrite(PIN_LED_RUIDO, HIGH);
        }
    }
}

// ========================================================
// === CONEXIÓN WIFI ===
// ========================================================
void CommsManager::setup_wifi() {
    Serial.print("Conectando a ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n✅ WiFi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

// ========================================================
// === CONEXIÓN MQTT ===
// ========================================================
void CommsManager::reconnect_mqtt() {
    while (!client.connected()) {
        Serial.print("Intentando conexión MQTT...");
        if (client.connect("ESP32Client", MQTT_USER, MQTT_PASS)) {
            Serial.println("conectado");
            client.subscribe(TOPIC_SUB);
            Serial.print("📡 Suscrito a: ");
            Serial.println(TOPIC_SUB);
        } 
        else {
            Serial.print("Error: ");
            Serial.println(client.state());
            delay(5000);
        }
    }
}

// ========================================================
// === NTP ===
// ========================================================
void CommsManager::inicializarNTP() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10000)) {
        Serial.println("❌ No se pudo sincronizar hora NTP");
    }
    else {
        Serial.println("⏱ Hora NTP sincronizada");
    }
}

String CommsManager::obtenerTiempoISO8601() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return "";

    char buffer[25];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
    return String(buffer);
}

// ========================================================
// === NUEVO: ENVÍO HTTP POST (JSON) ===
// ========================================================
bool CommsManager::enviarHTTP(const String& payload) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ No hay WiFi, imposible enviar HTTP.");
        return false;
    }

    HTTPClient http;

    String url = "http://" + String(HTTP_SERVER) + ":" + String(HTTP_PORT) + String(HTTP_PATH);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    Serial.println("🌐 Enviando HTTP POST a: " + url);

    int codigo = http.POST(payload);

    if (codigo > 0) {
        Serial.print("📨 Respuesta HTTP: ");
        Serial.println(codigo);
    } else {
        Serial.println("❌ Error en POST: " + String(http.errorToString(codigo)));
    }

    http.end();
    return codigo == 200 || codigo == 201;
}

// ========================================================
// === INICIALIZACIÓN ===
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
        reconnect_mqtt();
    }
    client.loop();
}

// ========================================================
// === PUBLICAR MQTT ===
// ========================================================
bool CommsManager::publicar(const char* topic, const String& payload) {
    return client.publish(topic, payload.c_str());
}
