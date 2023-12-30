#include "mqtt.h"
#include "structs.h"

extern struct system        struct_systemConfig;

/* instacias para WIFI e client*/
WiFiClient espClient;
PubSubClient client(espClient);

void mqttReconect();

/* configuraçãoes da REDE e broker MQTT*/
const char* ssid     = struct_systemConfig.ssid.c_str();
const char* password = struct_systemConfig.password.c_str();


/* configuraçãoes do broker MQTT*/
const char* mqttServer = "192.168.10.2";
const int mqttPort = 1883;


void mqttInit(){
   
   WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED &&  struct_systemConfig.tentativasConexao < struct_systemConfig.maxTentativasConexao) {
    delay(500);
    Serial.println("Conectando ao WiFi..");
    struct_systemConfig.tentativasConexao++;
   }
  if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Conectado na rede WiFi");
        struct_systemConfig.tentativasConexao = 0;
        client.setServer(struct_systemConfig.mqttServer.c_str(), struct_systemConfig.mqttPort);
    } else {
        Serial.println("Falha ao conectar na rede WiFi. Armazenando dados no cartão SD.");
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
         vTaskDelay(pdMS_TO_TICKS(100));
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

