#include "StatusNotifier.hpp"
#include "config.hpp" 

void StatusNotifier::inicializar(int ledPin) {
    _ledPin = ledPin;
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW); 

    // Aquí se asume que Wire ya fue inicializado en Sensores.cpp (GPIO 26/27)
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println("❌ Error al inicializar la pantalla SSD1306.");
    } else {
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        mostrarEstadoPersistente(); // Muestra el estado inicial
    }
}

// Función para dibujar la hora y el contenido del paquete
void StatusNotifier::mostrarEstadoPersistente() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("--- ESTADO MQTT ---");
    
    // Línea 2: Contador de Paquetes
    display.print("Paquete N:");
    display.setTextSize(2);
    display.println(_packageCounter);
    
    // Línea 3: Hora de Recepción
    display.setTextSize(1);
    display.print("Hora RX:");
    // Muestra solo la parte de la hora (HH:MM:SS)
    if (_lastTimestamp.length() >= 19) {
        display.println(_lastTimestamp.substring(11, 19)); 
    } else {
        display.println("N/A");
    }
    
    // Líneas 4-8: Contenido del Mensaje
    display.println("Contenido:");
    display.setTextSize(1);
    // Usa un cursor más pequeño para imprimir el contenido
    display.setCursor(0, 48);
    display.println(_lastMessage.substring(0, 20) + (_lastMessage.length() > 20 ? "..." : ""));

    display.display();
}

void StatusNotifier::notificarRecepcion(const String& timestamp, const String& message) {
    digitalWrite(_ledPin, HIGH); // Enciende el LED
    _ledOnTime = millis();       // Registra el tiempo
    
    // Actualiza los datos persistentes
    _packageCounter++;
    _lastTimestamp = timestamp;
    _lastMessage = message; // <-- Almacena el mensaje recibido
    
    // Actualiza la pantalla inmediatamente con el nuevo estado
    mostrarEstadoPersistente(); 
}

void StatusNotifier::manejarParpadeo() {
    // Apaga el LED después de 200ms, pero MANTIENE la información en la pantalla
    if (_ledOnTime > 0 && (millis() - _ledOnTime >= LED_DURATION_MS)) {
        digitalWrite(_ledPin, LOW); 
        _ledOnTime = 0;             
    }
}