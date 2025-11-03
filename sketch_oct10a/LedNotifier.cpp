#include "LedNotifier.hpp"
#include "config.hpp" // Necesario para acceder a PIN_LED_RUIDO

void LedNotifier::inicializar(int ledPin) {
    _ledPin = ledPin;
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW); // Asegura que esté apagado al inicio
}

void LedNotifier::notificarRecepcion() {
    digitalWrite(_ledPin, HIGH); // Enciende el LED
    _ledOnTime = millis();       // Registra el tiempo de encendido
}

void LedNotifier::manejarParpadeo() {
    // Si el LED está encendido (_ledOnTime > 0) y ha pasado el tiempo de duración...
    if (_ledOnTime > 0 && (millis() - _ledOnTime >= LED_DURATION_MS)) {
        digitalWrite(_ledPin, LOW); // Apaga el LED
        _ledOnTime = 0;             // Reinicia el temporizador
    }
}