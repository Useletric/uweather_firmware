// rtos.cpp
#include "structs.h"
#include "rtos.h"


extern struct anemometro    struct_anemometro;
extern struct bme280        struct_bme280;
extern struct tensaoPainel  struct_tensaoPainelSolar;
extern struct tensaoBateria struct_tensaoBateriaInterna;
extern struct system        struct_systemConfig;

#define MQTT_FLAG (1<<0)  //1
#define SD_FLAG (1<<1) // 10

SemaphoreHandle_t xSemaphore;
TimerHandle_t xTimer;
EventGroupHandle_t xEventos;

TaskHandle_t xTaskSensorHandle;
TaskHandle_t xTaskMQTTHandle;
TaskHandle_t xTaskSDHandle;

void vTaskMQTT(void *pvParameters); 
void vTaskSensor(void *pvParameters);
void vTaskSD(void *pvParameters);
void callBackTimer1(TimerHandle_t pxTimer);

void rtosInit() {
  xSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xSemaphore);
  xEventos = xEventGroupCreate();
  xTimer = xTimerCreate("TIMER1",pdMS_TO_TICKS(1000),pdTRUE,0,callBackTimer1);

  xTaskCreatePinnedToCore(vTaskSensor, "TaskSensor", configMINIMAL_STACK_SIZE + 4096, NULL, 1, &xTaskSensorHandle, APP_CPU_NUM);
  xTaskCreatePinnedToCore(vTaskMQTT,  "TaskMQTT",  configMINIMAL_STACK_SIZE + 2048,  NULL,  2,  &xTaskMQTTHandle,PRO_CPU_NUM);
  xTaskCreatePinnedToCore(vTaskSD,  "TaskSD",  configMINIMAL_STACK_SIZE + 2048,  NULL,  2,  &xTaskSDHandle,PRO_CPU_NUM);
  xTimerStart(xTimer,0);  
}

void vTaskSensor(void *pvParameters) {
  (void)pvParameters;
  
  while(1) {
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
    struct_systemConfig.datetime = getCurrentDateTime();
    Serial.println(struct_systemConfig.datetime);
    ShowData();

    struct_tensaoPainelSolar.voltage = ((voltageInput(VOLT_PIN) * 3.3)/4095)/ (330000.0 / (1000000.0 + 1000000.0));
    struct_tensaoBateriaInterna.voltage = ((voltageInput(VOLT_BAT)*4.2)/4095)*2;
    Serial.println(struct_tensaoPainelSolar.voltage);

    if(struct_systemConfig.sd_storage == true){
        xEventGroupSetBits(xEventos,SD_FLAG);
    }

    vTaskDelay(pdMS_TO_TICKS(struct_systemConfig.timer_ReadSensors));
  }
}

void vTaskSD(void *pvParameters){
  (void) pvParameters;
  EventBits_t xBits;

  while (1)
  {
    xBits = xEventGroupWaitBits(xEventos,SD_FLAG,pdTRUE,pdTRUE,portMAX_DELAY);
    StaticJsonDocument<200> doc;
          doc["datetime"] = struct_systemConfig.datetime;
          doc["temperature"] = struct_bme280.temp;
          doc["humidity"] = struct_bme280.umi;
          doc["pressure"] = struct_bme280.pressure;
          doc["altitude"] = struct_bme280.altitude;
          doc["windspeed"] = struct_anemometro.speedwind;
          doc["solarvolt"] = struct_tensaoPainelSolar.voltage;
          doc["batvolt"] = struct_tensaoBateriaInterna.voltage;

    store_sd("/uweather_data.txt",doc);
  }
  
}

/*Implementação da Task MQTT */
void vTaskMQTT(void *pvParameters){
  (void) pvParameters;
  char mensagem[254];
  EventBits_t xBits;

  while(1)
    {
      xBits = xEventGroupWaitBits(xEventos,MQTT_FLAG,pdTRUE,pdTRUE,portMAX_DELAY);  
          mqttInit();    
          mqttIsConected();
          // Criar um objeto JSON
          StaticJsonDocument<200> doc;
          doc["temperature"] = struct_bme280.temp;
          doc["humidity"] = struct_bme280.umi;
          doc["pressure"] = struct_bme280.pressure;
          doc["altitude"] = struct_bme280.altitude;
          doc["windspeed"] = struct_anemometro.speedwind;
          doc["solarvolt"] = struct_tensaoPainelSolar.voltage;
          doc["batvolt"] = struct_tensaoBateriaInterna.voltage;

          // Serializar o objeto JSON para a string
          serializeJson(doc, mensagem);   
            // Enviar a mensagem MQTT
          mqttSend(mensagem);    
          mqttDisconnect();
  }

}

void callBackTimer1(TimerHandle_t pxTimer)
{
    struct_systemConfig.timer_system++;
    if(struct_systemConfig.timer_system == struct_systemConfig.timer_MQTTConnection){
      struct_systemConfig.timer_system = 0;
      xEventGroupSetBits(xEventos,MQTT_FLAG);
    }
    
}