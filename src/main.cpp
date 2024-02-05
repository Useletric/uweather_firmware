#include "main.h"

#define uS_TO_S_FACTOR 1000000UL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300UL        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;



void setup() {
  Serial.begin(115200);
  delay(1000);
  ++bootCount;
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  pinMode(SENSOR_PIN,INPUT);
  getID();
  init_sd();
  loadConfiguration("/config.txt");
  initBME280();

  ota_prep();

  struct_systemConfig.datetime = getCurrentDateTime();
  Serial.println(struct_systemConfig.datetime);
  readSensors();
  if (bootCount  >= 2) {
    bootCount = 0;  // Reinicia a contagem
      mqttInit();     //iniica comunicação com MQTT
      mqttIsConected();
      setClock();
    streamingData();
    fn_update();
  }
  if(struct_systemConfig.sd_storage == true){
    salvarDados();
  }
  esp_deep_sleep_start();
}

void loop() {

}