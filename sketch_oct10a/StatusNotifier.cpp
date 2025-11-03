#include "StatusNotifier.hpp"
#include "config.hpp" 

void StatusNotifier::inicializar(int ledPin) {
    _ledPin = ledPin;
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW); 

    // Inicialización de la pantalla
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println("❌ Error al inicializar la pantalla SSD1306.");
    } else {
        display.display(); // Muestra el buffer inicial
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        mostrarEstado("Estacion Iniciada...");
    }
}

void StatusNotifier::mostrarEstado(const char* estado) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("--- ESTADO ---");
    display.println();
    display.setTextSize(2);
    display.println(estado);
    display.display();
}

void StatusNotifier::notificarRecepcion() {
    digitalWrite(_ledPin, HIGH); // Enciende el LED
    _ledOnTime = millis();       // Registra el tiempo
    
    // Actualiza el estado en la pantalla
    mostrarEstado("RX MSG!"); 
}

void StatusNotifier::manejarParpadeo() {
    // Si el LED está encendido y ha pasado el tiempo de duración (200ms)...
    if (_ledOnTime > 0 && (millis() - _ledOnTime >= LED_DURATION_MS)) {
        digitalWrite(_ledPin, LOW); // Apaga el LED
        _ledOnTime = 0;             // Reinicia el temporizador
        
        // Vuelve a mostrar el estado "Iniciada" o "En Espera"
        mostrarEstado("En Espera"); 
    }
}