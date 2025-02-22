#include "main.h"

#define uS_TO_S_FACTOR 1000000UL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300UL        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int updateCount = 0;



void setup() {
  Serial.begin(115200);
  delay(1000);
  ++bootCount;
  ++updateCount;

  pinMode(ANEMO_PIN,INPUT);
  pinMode(RAIN_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RAIN_SENSOR_PIN), rainTrigger, CHANGE);
  getID();
  init_sd();
  loadConfiguration("/config.txt");
  initBME280();

  ota_prep();

  mqttInit();
  setClock(); 

  struct_systemConfig.datetime = getCurrentDateTime();
 
}

void loop() {
  unsigned long currentMillis = millis();
    
  if (currentMillis - lastSensorReadTime >= 60000) {
      lastSensorReadTime = currentMillis;
      
      Serial.println("Iniciando leitura dos sensores...");
      readSensors();
      
      struct_systemConfig.datetime = getCurrentDateTime();
      if (struct_systemConfig.sd_storage) {
          salvarDados();
      }
      
      mqttIsConected();
      streamingData();
      
      
      Serial.print("Eventos de chuva detectados: ");
      Serial.println(struct_pluviometro.count);
      
  }
}