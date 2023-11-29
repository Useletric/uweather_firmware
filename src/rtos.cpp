// rtos.cpp
#include "rtos.h"

        extern struct anemometro    struct_anemometro;
        extern struct bme280        struct_bme280;
        extern struct tensaoPainel  struct_tensaoPainelSolar;
        extern struct tensaoBateria struct_tensaoBateriaInterna;
        extern struct system        struct_systemConfig;

SemaphoreHandle_t xSemaphore;

TaskHandle_t xTaskSensorHandle;
TaskHandle_t xTaskMQTTHandle;
QueueHandle_t xFila; 

void vTaskMQTT(void *pvParameters); 
void vTaskSensor(void *pvParameters);

void rtosInit() {
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemaphore);
    xFila = xQueueCreate(1, sizeof(int));
    xTaskCreatePinnedToCore(vTaskSensor, "TaskSensor", configMINIMAL_STACK_SIZE + 4096, NULL, 1, &xTaskSensorHandle, APP_CPU_NUM);
    xTaskCreatePinnedToCore(vTaskMQTT,  "TaskMQTT",  configMINIMAL_STACK_SIZE + 2048,  NULL,  2,  &xTaskMQTTHandle,PRO_CPU_NUM);  
}

void vTaskSensor(void *pvParameters) {
  (void)pvParameters;
  
  while(1) {
    Serial.println("Entrou na tarefa Sensor");  // Adicione mensagens de depuração
    unsigned int Sample = 0;
    Serial.print(Sample);
    Serial.print(": Inicia Leitura...");

    // Chama a função para medir a velocidade do vento
    windvelocity();

    Serial.println("   Finalizado.");
    Serial.print("Contador: ");
    Serial.print(struct_anemometro.counter);
    Serial.print(";  RPM: ");
    
    // Chama a função para calcular o RPM
    RPMCalc();
    Serial.print(struct_anemometro.RPM);
    Serial.print(";  Vel. Vento: ");

    // Chama a função para calcular a velocidade do vento em m/s
    WindSpeed();
    Serial.print(struct_anemometro.windspeed);
    Serial.print(" [m/s] ");

    // Chama a função para calcular a velocidade do vento em km/h
    SpeedWind();
    Serial.print(struct_anemometro.speedwind);
    Serial.print(" [km/h] ");
    Serial.println();
    Serial.print(" INPUT: ");
    Serial.print(digitalRead(SENSOR_PIN));
    Serial.println();



    vTaskDelay(pdMS_TO_TICKS(struct_systemConfig.timer_ReadSensors));
  }
}

/*Implementação da Task MQTT */
void vTaskMQTT(void *pvParameters){
  (void) pvParameters;
  char mensagem[254];
  float valor_recebido = 0;

  while(1)
  {
    
      if(xQueueReceive(xFila, &valor_recebido, portMAX_DELAY) == pdTRUE) //verifica se há valor na fila para ser lido. Espera 1 segundo
      {
        mqttIsConected();
// Criar um objeto JSON
      StaticJsonDocument<200> doc;
      doc["temperature"] = tempBME();
      doc["humidity"] = umiBME();
      doc["pressure"] = presureBME();
      doc["altitude"] = aproxAltBME();
      doc["windspeed"] = valor_recebido;

      // Serializar o objeto JSON para a string
      serializeJson(doc, mensagem);   
        // Enviar a mensagem MQTT
      mqttSend(mensagem);    
        vTaskDelay(pdMS_TO_TICKS(30000));
      }

  }

}


