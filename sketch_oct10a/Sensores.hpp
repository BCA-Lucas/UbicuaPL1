#ifndef SENSORES_HPP
#define SENSORES_HPP

#include <Arduino.h>

struct SensorData {
	float temperature_celsius;
	float humidity_percent;
	int air_quality_index;
	float wind_speed_kmh;
	int wind_direction_degrees;
	float atmospheric_pressure_hpa;
	float uv_index;
	float luminosity_lux;
	int noise_analog;
	int rain_detected;
};

class SensorManager {
public:
	void inicializarSensores();
	void leerTodos(SensorData& data);
	String generarJSON(const SensorData& data);

private:
	int promedioAnalogico(int pin);
	float getUVIndex(float uvVoltage);
};

#endif