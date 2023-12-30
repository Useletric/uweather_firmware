#include "main.h"

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN,INPUT);
  getID();
  
  init_sd();
  loadConfiguration("/config.txt");
  initBME280();
  mqttInit();     //iniica comunicação com MQTT
  setClock();
  rtosInit();     //inicialização das tasks e estrutuas do RTOS
}

void loop() {
  vTaskDelete(NULL);  //deleta task loop
}