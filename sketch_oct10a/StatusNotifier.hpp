#ifndef STATUS_NOTIFIER_HPP
#define STATUS_NOTIFIER_HPP

#include <Arduino.h>
#include <Adafruit_GFX.h>      // Core graphics library
#include <Adafruit_SSD1306.h>  // Hardware-specific library for OLED

// Definición para la pantalla I2C (cambia el pin RESET si es necesario)
#define SCREEN_WIDTH 128 // Ancho en píxeles
#define SCREEN_HEIGHT 64 // Alto en píxeles
#define OLED_RESET -1    // Pin de reset, -1 si se comparte el pin de reset de la ESP32

class StatusNotifier {
public:
    void inicializar(int ledPin); 
    void notificarRecepcion(); // Enciende LED y actualiza pantalla
    void manejarParpadeo();    // Apaga LED y limpia el estado de recepción en pantalla
    
private:
    int _ledPin;
    unsigned long _ledOnTime = 0;
    const unsigned long LED_DURATION_MS = 200; // Duración del parpadeo

    Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    void mostrarEstado(const char* estado);
};

#endif