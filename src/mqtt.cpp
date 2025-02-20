#include "mqtt.h"
#include "structs.h"

extern struct system        struct_systemConfig;

/* instacias para WIFI e client*/
WiFiClient espClient;
PubSubClient client(espClient);

void mqttReconect();

/* configuraçãoes da REDE e broker MQTT*/
// Declare uma matriz de caracteres para armazenar o SSID
char ssid[32]; // Assumindo que o SSID tem no máximo 31 caracteres

// Copie o conteúdo da String para a matriz de caracteres

const char* password = struct_systemConfig.password.c_str();


/* configuraçãoes do broker MQTT*/
const char* mqttServer = "192.168.10.2";
const int mqttPort = 1883;


void mqttInit(){
   // Declare uma matriz de caracteres para armazenar o SSID
    char ssid[32]; // Assumindo que o SSID tem no máximo 31 caracteres

    // Copie o conteúdo da String para a matriz de caracteres
    strncpy(ssid, struct_systemConfig.ssid.c_str(), sizeof(ssid) - 1);
    ssid[sizeof(ssid) - 1] = '\0'; // Certifique-se de terminar a string com null
   WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED &&  struct_systemConfig.tentativasConexao < struct_systemConfig.maxTentativasConexao) {
    delay(500);
    Serial.println("Conectando ao WiFi..");
    struct_systemConfig.tentativasConexao++;
   }
  if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Conectado na rede WiFi");
        struct_systemConfig.tentativasConexao = 0;
        struct_systemConfig.sd_storage = false;
        client.setServer(struct_systemConfig.mqttServer.c_str(), struct_systemConfig.mqttPort);
    } else {
        Serial.println("Falha ao conectar na rede WiFi. Armazenando dados no cartão SD.");
        struct_systemConfig.tentativasConexao = 0;
        struct_systemConfig.sd_storage = true;
    }


}

void mqttIsConected(){
    if(!client.connected()){
          mqttReconect();
    }
}

void mqttSend(char mensagem[]){
    //Envia a mensagem ao broker
    client.publish("weather/sensor/windspeed", mensagem);
    Serial.print("Valor enviado: ");
    Serial.println(mensagem);
}

//função pra reconectar ao servido MQTT
void mqttReconect() {
  //Enquanto estiver desconectado
  while (!client.connected() && struct_systemConfig.tentativasConexao < struct_systemConfig.maxTentativasConexao) {
      if (client.connect("ESP32Client")) {
         Serial.println("Conectado ao broker!");
         struct_systemConfig.tentativasConexao = 0;
      } else {
         Serial.print("Falha na conexao ao broker - Estado: ");
         Serial.print(client.state());
         delay(100);
         struct_systemConfig.tentativasConexao++;
      }
   }

   if (!client.connected() && struct_systemConfig.tentativasConexao >= struct_systemConfig.maxTentativasConexao) {
      Serial.println("Número máximo de tentativas atingido. Armazenando dados no cartão SD.");
      // Chame uma função para armazenar os dados no cartão SD aqui
      struct_systemConfig.sd_storage = true;
   }
}

//Função para desconectar da rede
void mqttDisconnect(){
  WiFi.disconnect();
}

void streamingData(){
  // Abrir o arquivo para leitura
  File file = SD.open("/leituras.txt");
  if (!file) {
    Serial.println("Erro ao abrir o arquivo para leitura.");
    return;
  }

  // Inicializar variáveis para calcular a média
  float tempSum = 0;
  float umiSum = 0;
  float pressureSum = 0;
  float altitudeSum = 0;
  float windspeedSum = 0;
  float solarvoltSum = 0;
  float batvoltSum = 0;
  int count = 0;

  // Ler cada linha do arquivo e somar os valores
  while (file.available()) {
    String line = file.readStringUntil('\n');
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, line);
    if (!error) {
      tempSum += doc["temperature"].as<float>();
      umiSum += doc["humidity"].as<float>();
      pressureSum += doc["pressure"].as<float>();
      altitudeSum += doc["altitude"].as<float>();
      windspeedSum += doc["windspeed"].as<float>();
      solarvoltSum += doc["solarvolt"].as<float>();
      batvoltSum += doc["batvolt"].as<float>();
      count++;
    }
  }

  file.close();

  // Calcular a média
  float tempAvg = tempSum / count;
  float umiAvg = umiSum / count;
  float pressureAvg = pressureSum / count;
  float altitudeAvg = altitudeSum / count;
  float windspeedAvg = windspeedSum / count;
  float solarvoltAvg = solarvoltSum / count;
  float batvoltAvg = batvoltSum / count;

  // Criar um objeto JSON para a média
  StaticJsonDocument<200> avgDoc;
  avgDoc["temperature"] = tempAvg;
  avgDoc["humidity"] = umiAvg;
  avgDoc["pressure"] = pressureAvg;
  avgDoc["altitude"] = altitudeAvg;
  avgDoc["windspeed"] = windspeedAvg;
  avgDoc["solarvolt"] = solarvoltAvg;
  avgDoc["batvolt"] = batvoltAvg;
  avgDoc["winddirection"] = struct_biruta.wind_dir;
  avgDoc["index_rain"] = struct_pluviometro.count;
  avgDoc["id"] = struct_systemConfig.idStation;

  // Converter o objeto JSON para uma string
  char avgMessage[508];
  serializeJson(avgDoc, avgMessage);

  // Enviar a mensagem MQTT com a média dos dados
  mqttSend(avgMessage);

  // Limpar o arquivo após a transmissão
  SD.remove("/leituras.txt");

  Serial.println("Dados transmitidos via MQTT.");

  struct_pluviometro.count = 0;
}

bool shouldEnterDeepSleep() {
  return true;  // Sempre entra em Deep Sleep após cada ciclo
}

void enterDeepSleep() {
  // Desativa o Wi-Fi antes de entrar em Deep Sleep
  WiFi.disconnect(true);

  // Imprime uma mensagem antes de entrar em Deep Sleep
  Serial.println("Entrando em Deep Sleep...");

  // Adormece por 15 minutos (900 segundos)
  esp_sleep_enable_timer_wakeup(900ULL * 1000000ULL);
  esp_deep_sleep_start();
}

bool shouldTransmit() {
  static unsigned int transmitCounter = 0;
  const unsigned int TRANSMIT_INTERVAL = 4;  // Transmitir a cada 4 leituras

  transmitCounter++;
  if (transmitCounter == TRANSMIT_INTERVAL) {
    transmitCounter = 0;
    return true;
  }

  return false;
}