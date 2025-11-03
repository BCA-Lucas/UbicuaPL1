#ifndef LED_NOTIFIER_HPP
#define LED_NOTIFIER_HPP

#include <Arduino.h>

class LedNotifier {
public:
    // Asumiendo que PIN_LED_RUIDO es el pin del LED integrado (GPIO 2)
    void inicializar(int ledPin); 
    void notificarRecepcion(); // Función para encender el LED
    void manejarParpadeo();    // Función para apagar el LED después de un tiempo

private:
    int _ledPin;
    unsigned long _ledOnTime = 0;
    const unsigned long LED_DURATION_MS = 200; // Duración del parpadeo (200ms)
};

#endif