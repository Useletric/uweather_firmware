// rtos.cpp
#include "rtos.h"

SemaphoreHandle_t xSemaphore;

// --- Constantes ---
const float pi = 3.14159265;     //Número de pi
int period = 5000;               //Tempo de medida(miliseconds)
int delaytime = 2000;            //Invervalo entre as amostras (miliseconds)
int radius = 147;                //Raio do anemometro(mm)

// --- Variáveis Globais ---
unsigned int Sample  = 0;        //Armazena o número de amostras
unsigned int counter = 0;        //Contador para o sensor
unsigned int RPM = 0;            //Rotações por minuto
float speedwind = 0;             //Velocidade do vento (m/s)
float windspeed = 0;             //Velocidade do vento (km/h)

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
    Serial.print(counter);
    Serial.print(";  RPM: ");
    
    // Chama a função para calcular o RPM
    RPMCalc();
    Serial.print(RPM);
    Serial.print(";  Vel. Vento: ");

    // Chama a função para calcular a velocidade do vento em m/s
    WindSpeed();
    Serial.print(windspeed);
    Serial.print(" [m/s] ");

    // Chama a função para calcular a velocidade do vento em km/h
    SpeedWind();
    Serial.print(speedwind);
    Serial.print(" [km/h] ");
    Serial.println();
    Serial.print(" INPUT: ");
    Serial.print(digitalRead(SENSOR_PIN));
    Serial.println();
    float temp =  tempBME();

    float umi = umiBME();

    float pressure =  presureBME();

    float altitude = aproxAltBME();

    xQueueOverwrite(xFila, &speedwind);/* envia valor atual de count para fila*/
    vTaskDelay(pdMS_TO_TICKS(delaytime));
  }
}

void windvelocity() {
  speedwind = 0;
  windspeed = 0;
  counter = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), addcount, RISING);
    Serial.print(" INPUT: ");
    Serial.print(digitalRead(SENSOR_PIN));
  vTaskDelay(pdMS_TO_TICKS(period));
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


void RPMCalc() {
  if (period != 0) {
    RPM = ((counter) * 60) / (period / 1000);
  } else {
    // Tratar o caso de divisão por zero, se necessário
    RPM = 0;
  }
}

void WindSpeed() {
  windspeed = ((4 * pi * radius * RPM) / 60) / 1000;
}

void SpeedWind() {
  speedwind = (((4 * pi * radius * RPM) / 60) / 1000) * 3.6;
}

void addcount() {
  counter++;
}