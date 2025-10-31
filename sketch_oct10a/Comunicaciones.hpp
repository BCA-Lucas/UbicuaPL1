#ifndef COMUNICACIONES_HPP
#define COMUNICACIONES_HPP

#include <Arduino.h>

class CommsManager {
public:
	void inicializarComunicaciones();
	void mantenerConexion();
	bool publicar(const char* topic, const String& payload);
	
	String obtenerTiempoISO8601(); 

private:
	void setup_wifi();
	void reconnect_mqtt();
	void inicializarNTP();
	static void callback(char* topic, byte* message, unsigned int length);
};

#endif