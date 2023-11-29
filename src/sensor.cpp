#include "sensor.h"

Adafruit_BME280 bme; // I2C

    extern struct anemometro    struct_anemometro;
    extern struct bme280        struct_bme280;
    extern struct tensaoPainel  struct_tensaoPainelSolar;
    extern struct tensaoBateria struct_tensaoBateriaInterna;

void initBME280(){
    Serial.println(F("BME280 test"));
    bool status;

    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin(0x76);  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }

    Serial.println("-- Default Test --");
}

float tempBME(){
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    return bme.readTemperature();
}
float umiBME(){
    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    return bme.readHumidity();
}
float presureBME(){
    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    return bme.readPressure()/100.0F;

}
void getDataBME280(){
    struct_bme280.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    struct_bme280.pressure = bme.readPressure()/100.0F;
    struct_bme280.temp = bme.readTemperature();
    struct_bme280.umi = bme.readHumidity();

}


void windvelocity() {
  struct_anemometro.speedwind = 0;
  struct_anemometro.windspeed = 0;
  struct_anemometro.counter = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), addcount, RISING);
    Serial.print(" INPUT: ");
    Serial.print(digitalRead(SENSOR_PIN));
  vTaskDelay(pdMS_TO_TICKS(struct_anemometro.period));
}

void RPMCalc() {
  if (struct_anemometro.period != 0) {
    struct_anemometro.RPM = ((struct_anemometro.counter) * 60) / (struct_anemometro.period / 1000);
  } else {
    // Tratar o caso de divisão por zero, se necessário
    struct_anemometro.RPM = 0;
  }
}

void WindSpeed() {
  struct_anemometro.windspeed = ((4 * struct_anemometro.pi * struct_anemometro.radius * struct_anemometro.RPM) / 60) / 1000;
}

void SpeedWind() {
  struct_anemometro.speedwind = (((4 * struct_anemometro.pi * struct_anemometro.radius * struct_anemometro.RPM) / 60) / 1000) * 3.6;
}

void addcount() {
  struct_anemometro.counter++;
}

void ShowData(){
    sprintf(struct_systemConfig.Buffer, "Contador: %d ; RPM: %d ; Vel. Vento: %.4f [km/h] ; Temperatura: %.4f [°C] ; Umidade: %.4f [%] ; Pressão %.4f [hPa] ; Altitude: %.4f [m]",
                                            struct_anemometro.counter,
                                            struct_anemometro.RPM,
                                            struct_anemometro.speedwind,
                                            struct_bme280.temp,
                                            struct_bme280.umi,
                                            struct_bme280.pressure,
                                            struct_bme280.altitude
                                            );
    struct_systemConfig.infoSensor = String(struct_systemConfig.Buffer);  
    Serial.println(struct_systemConfig.infoSensor);
}
