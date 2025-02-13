//sensor.cpp
#include "structs.h"
#include "sensor.h"

Adafruit_AHTX0 aht; // I2C

extern struct anemometro    struct_anemometro;
extern struct bme280        struct_bme280;
extern struct tensaoPainel  struct_tensaoPainelSolar;
extern struct tensaoBateria struct_tensaoBateriaInterna;
extern struct system        struct_systemConfig;

void initBME280(){
    Serial.println(F("BME280 test"));
    bool status;


    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = aht.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }

    Serial.println("-- Default Test --");
}

float tempBME(){
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    Serial.print("Temperature = ");
    Serial.print(temp.temperature);
    Serial.println(" *C");

    return temp.temperature;
}
float umiBME(){
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    Serial.print("Humidity = ");
    Serial.print(humidity.relative_humidity);
    Serial.println(" %");

    return humidity.relative_humidity;
}
float presureBME(){
    Serial.print("Pressure = ");
    //Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    return 0;

}
void getDataBME280(){
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    //.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    struct_bme280.pressure = 0;
    struct_bme280.temp = temp.temperature;
    struct_bme280.umi = humidity.relative_humidity;

}


void windvelocity() {
  struct_anemometro.speedwind = 0;
  struct_anemometro.windspeed = 0;
  struct_anemometro.counter = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), addcount, RISING);

  unsigned long millis();       
  long startTime = millis();
  while(millis() < startTime + struct_anemometro.period) {
  }
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
  struct_anemometro.windspeed = (((4 * struct_anemometro.pi * struct_anemometro.radius * struct_anemometro.RPM) / 60) / 1000)*0.3636;
}

void SpeedWind() {
  struct_anemometro.speedwind = ((((4 * struct_anemometro.pi * struct_anemometro.radius * struct_anemometro.RPM) / 60) / 1000) * 3.6)*0.3636;
}

void addcount() {
  struct_anemometro.counter++;
}

void ShowData(){
    sprintf(struct_systemConfig.Buffer, "Contador: %d ; RPM: %d ; Vel. Vento: %.4f [km/h] ; Temperatura: %.4f [°C] ; Umidade: %.4f [%] ; Pressão %.4f [hPa] ; Altitude: %.4f [m] ; Tensão Painel: %.4f [V] ; Tensão Bat: %.4f [V]",
                                            struct_anemometro.counter,
                                            struct_anemometro.RPM,
                                            struct_anemometro.speedwind,
                                            struct_bme280.temp,
                                            struct_bme280.umi,
                                            struct_bme280.pressure,
                                            struct_bme280.altitude,
                                            struct_tensaoPainelSolar.voltage,
                                            struct_tensaoBateriaInterna.voltage
                                            );
    struct_systemConfig.infoSensor = String(struct_systemConfig.Buffer);  
    Serial.println(struct_systemConfig.infoSensor);
}

float voltageInput(uint8_t portaAnalogica){
    float total=0;  
    for (int i=0; i<AMOSTRAS; i++) {
        total += 1.0 * analogRead(portaAnalogica);
        delay(5);
    }
  return total / (float)AMOSTRAS;
}

void readSensors(){
      Serial.println("Entrou na tarefa Sensor");  // Adicione mensagens de depuração

    Serial.print(": Inicia Leitura Anemometro...");

    // Chama a função para medir a velocidade do vento
    windvelocity();

    Serial.println("   Finalizado.");

    
    // Chama a função para calcular o RPM
    RPMCalc();

    // Chama a função para calcular a velocidade do vento em m/s
    WindSpeed();

    // Chama a função para calcular a velocidade do vento em km/h
    SpeedWind();

    // Coleta dados do BME280
    getDataBME280();
    // Chama a função para mostrar os dados
    

    struct_tensaoPainelSolar.voltage = ((voltageInput(VOLT_PIN) * 3.3)/4095)/ (330000.0 / (1000000.0 + 1000000.0));
    struct_tensaoBateriaInterna.voltage = ((voltageInput(VOLT_BAT)*4.2)/4095)*2;
    Serial.println(struct_tensaoPainelSolar.voltage);

    salvarLeitura();
    ShowData();
}

