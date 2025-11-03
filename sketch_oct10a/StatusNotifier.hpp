#ifndef STATUS_NOTIFIER_HPP
#define STATUS_NOTIFIER_HPP

#include <Arduino.h>
#include <Adafruit_GFX.h>      
#include <Adafruit_SSD1306.h>  

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1    

class StatusNotifier {
public:
    void inicializar(int ledPin); 
    // Modificado para aceptar la hora Y el mensaje completo
    void notificarRecepcion(const String& timestamp, const String& message); 
    void manejarParpadeo();    
    
private:
    int _ledPin;
    unsigned long _ledOnTime = 0;
    const unsigned long LED_DURATION_MS = 200; 
    long _packageCounter = 0; 
    String _lastTimestamp = "N/A"; 
    String _lastMessage = "Esperando..."; // <-- Nuevo: Almacena el contenido del mensaje

    Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    void mostrarEstadoPersistente(); 
};

#endif