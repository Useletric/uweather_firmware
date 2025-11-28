#include "main.h"

#define uS_TO_S_FACTOR 1000000UL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300UL        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int updateCount = 0;
String btName = "EMAP" + String(struct_systemConfig.idStation);  
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);
  //bluetoothInit(btName.c_str());  // nome que vai aparecer no Bluetooth
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

  WiFiManager wm;
  bool res;
  // showConnectingWiFi(); // Mostra mensagem no display
  //  res = wm.autoConnect(); // auto generated AP name from chipid
  //  res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect((struct_systemConfig.idStation).c_str(), "123456789"); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    { request->send(200, "text/plain", "Hi! This is ElegantOTA AsyncDemo."); });


    // Inicia o servidor
    ElegantOTA.begin(&server); // Start ElegantOTA
    ElegantOTA.setAutoReboot(true);
    server.begin();
    mqttInit();
    setClock(); 

    struct_systemConfig.datetime = getCurrentDateTime();
  }
  
   
}

void loop() {
  unsigned long currentMillis = millis();
 // bluetoothLoop();  // verifica se chegou algum comando BT

  if (currentMillis - lastOtaCheck >= otaInterval) {
    lastOtaCheck = currentMillis;
    Serial.println("🔄 Verificando OTA...");
    fn_update();  // Checa atualização no servidor
  }
    
  if (currentMillis - lastSensorReadTime >= 60000) {
      lastSensorReadTime = currentMillis;
      
      Serial.println("Iniciando leitura dos sensores...");
      readSensors();
      
      struct_systemConfig.datetime = getCurrentDateTime();

          salvarDados();
          mqttInit();
          mqttIsConected();
          streamingData();

      Serial.print("Eventos de chuva detectados: ");
      Serial.println(struct_pluviometro.count);
      
  }
}