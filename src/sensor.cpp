//sensor.cpp
#include "structs.h"
#include "sensor.h"

Adafruit_BME280 bme; // I2C

extern struct anemometro    struct_anemometro;
extern struct biruta        struct_biruta;
extern struct bme280        struct_bme280;
extern struct tensaoPainel  struct_tensaoPainelSolar;
extern struct tensaoBateria struct_tensaoBateriaInterna;
extern struct system        struct_systemConfig;

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
    //Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    return 0;

}
void getDataBME280(){
  struct_bme280.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  struct_bme280.pressure = bme.readPressure()/100.0F;
  struct_bme280.temp = bme.readTemperature();
  struct_bme280.umi = bme.readHumidity();

}

void readWindDirection(int &wind_dir, String &direction) {
  int adcValue = analogRead(BIRUT_PIN); // Leitura do ADC

  Serial.print("Leitura do sensor: ");
  Serial.println(adcValue);

  if (adcValue <= 248) {
      wind_dir = 315;
      direction = "Noroeste";
  } else if (adcValue <= 745) { 
      wind_dir = 270;
      direction = "Oeste";
  } else if (adcValue <= 1241) { 
      wind_dir = 225;
      direction = "Sudoeste";
  } else if (adcValue <= 1738) { 
      wind_dir = 180;
      direction = "Sul";
  } else if (adcValue <= 2235) { 
      wind_dir = 135;
      direction = "Sudeste";
  } else if (adcValue <= 2732) { 
      wind_dir = 90;
      direction = "Leste";
  } else if (adcValue <= 3335) {  
      wind_dir = 45;
      direction = "Nordeste";
  } else {  
      wind_dir = 0;
      direction = "Norte";
  }
}

void windvelocity() {
  struct_anemometro.speedwind = 0;
  struct_anemometro.windspeed = 0;
  struct_anemometro.counter = 0;
  attachInterrupt(digitalPinToInterrupt(ANEMO_PIN), addcount, RISING);

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
    sprintf(struct_systemConfig.Buffer, "Volume: %d ; Direcao °: %d ; %s ; Vel. Vento: %.4f [km/h] ; Temperatura: %.4f [°C] ; Umidade: %.4f [%] ; Pressão %.4f [hPa] ; Altitude: %.4f [m] ; Tensão Painel: %.4f [V] ; Tensão Bat: %.4f [V]",
                                            struct_pluviometro.count,
                                            struct_biruta.wind_dir,
                                            struct_biruta.direction,
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

void IRAM_ATTR rainTrigger(){
  bool currentRainState = digitalRead(RAIN_SENSOR_PIN);
  if (currentRainState != struct_pluviometro.lastState) {
      struct_pluviometro.count++;
      struct_pluviometro.lastState = currentRainState;
  }
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
    

    struct_tensaoPainelSolar.voltage = ((voltageInput(VOLT_PIN) * 3.3)/4095)/ (1000000.0 / (1000000.0 + 1000000.0));
    struct_tensaoBateriaInterna.voltage = ((voltageInput(VOLT_BAT)*4.2)/4095)*2;
    Serial.println(struct_tensaoPainelSolar.voltage);
    readWindDirection(struct_biruta.wind_dir, struct_biruta.direction);

    salvarLeitura();
    ShowData();
}

