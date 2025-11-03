#include "StatusNotifier.hpp"
#include "config.hpp" 

void StatusNotifier::inicializar(int ledPin) {
    _ledPin = ledPin;
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW); 

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println("❌ Error al inicializar la pantalla SSD1306.");
    } else {
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        mostrarEstadoPersistente(); // Muestra el estado inicial
    }
}

// Función para dibujar la hora y el contador
void StatusNotifier::mostrarEstadoPersistente() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("--- ESTADO MQTT ---");
    display.println();
    
    display.print("Ultimo Paq.: ");
    display.setTextSize(2);
    display.println(_packageCounter);
    
    display.setTextSize(1);
    display.println("Hora Recepcion:");
    
    // Muestra solo la parte de la hora (HH:MM:SS) del timestamp ISO8601
    // El formato es YYYY-MM-DDTHH:MM:SSZ (índice 11 al 18)
    if (_lastTimestamp.length() >= 19) {
        display.setTextSize(2);
        display.print(" ");
        display.println(_lastTimestamp.substring(11, 19)); 
    } else {
        display.setTextSize(2);
        display.println(" Sin datos");
    }
    
    display.display();
}

void StatusNotifier::notificarRecepcion(const String& timestamp) {
    digitalWrite(_ledPin, HIGH); // Enciende el LED
    _ledOnTime = millis();       // Registra el tiempo
    
    // Actualiza los datos persistentes
    _packageCounter++;
    _lastTimestamp = timestamp;
    
    // Actualiza la pantalla inmediatamente con el nuevo estado
    mostrarEstadoPersistente(); 
}

void StatusNotifier::manejarParpadeo() {
    // Si el LED está encendido y ha pasado el tiempo de duración (200ms)...
    if (_ledOnTime > 0 && (millis() - _ledOnTime >= LED_DURATION_MS)) {
        digitalWrite(_ledPin, LOW); // Solo apaga el LED
        _ledOnTime = 0;             // Reinicia el temporizador
        // **IMPORTANTE:** No llamamos a mostrarEstadoPersistente() aquí.
        // La información de la hora y el contador permanece en la pantalla.
    }
}