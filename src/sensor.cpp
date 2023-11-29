#include "sensor.h"
Adafruit_BME280 bme; // I2C
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

float aproxAltBME(){
    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    return bme.readAltitude(SEALEVELPRESSURE_HPA);
}

void getDataBME(){

}