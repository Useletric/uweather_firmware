#include "mqtt.h"


/* instacias para WIFI e client*/
WiFiClient espClient;
PubSubClient client(espClient);

void mqttReconect();

/* configuraçãoes da REDE e broker MQTT*/
const char* ssid     = "CAFOFO";
const char* password = "Bc270299";


/* configuraçãoes do broker MQTT*/
const char* mqttServer = "192.168.10.2";
const int mqttPort = 1883;

// Set time via NTP, as required for x.509 validation
void setClock () {
    configTime (0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print ("Waiting for NTP time sync: ");
    time_t now = time (nullptr);
    while (now < 8 * 3600 * 2) {
        delay (500);
        Serial.print (".");
        now = time (nullptr);
    }
    struct tm timeinfo;
    gmtime_r (&now, &timeinfo);
    Serial.print ("\n");
    Serial.print ("Current time: ");
    Serial.print (asctime (&timeinfo));
}

void mqttInit(){
   
   WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao WiFi..");
   }
  Serial.println("Conectado na rede WiFi");

  client.setServer(mqttServer, mqttPort);


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
  while (!client.connected()) {

    if (client.connect("ESP32Client"))
    {
      Serial.println("Conectado ao broker!");
    }
    else
    {
      Serial.print("Falha na conexao ao broker - Estado: ");
      Serial.print(client.state());
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}

