#include "main.h"

#define uS_TO_S_FACTOR 1000000UL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 300UL      /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int updateCount = 0;

WiFiManager wm;
bool portalStarted = false;
AsyncWebServer server(80);

void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("Entrou em modo AP");
}

void saveConfigCallback() {
  Serial.println("Configuração salva");
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  ++bootCount;
  ++updateCount;

  pinMode(ANEMO_PIN, INPUT);
  pinMode(RAIN_SENSOR_PIN, INPUT_PULLUP);
  initRainPCNT();

  getID();
  init_sd();
  loadConfiguration("/config.txt");
  initBME280();

  ota_prep();

  bool res;
  wm.setConfigPortalBlocking(false); // Não bloqueia o loop
  wm.setConfigPortalTimeout(0); // AP nunca fecha automaticamente

  
  if (!wm.autoConnect((struct_systemConfig.idStation).c_str(), "123456789"))
  {
    Serial.println("Failed to connect initially");
    portalStarted = wm.startConfigPortal((struct_systemConfig.idStation).c_str());
  }
  else
  {

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "Hi! This is ElegantOTA AsyncDemo."); });

    // Inicia o servidor
    ElegantOTA.begin(&server); // Start ElegantOTA
    ElegantOTA.setAutoReboot(true);
    server.begin();
    if (MDNS.begin((struct_systemConfig.idStation).c_str())) {
        Serial.println("mDNS responder started");
        MDNS.addService("http", "tcp", 80);
    }
    wm.setAPCallback(configModeCallback); // Callback para quando entra em modo AP
wm.setSaveConfigCallback(saveConfigCallback); // Callback para quando salva configuração

// Mantém o AP ativo mesmo após a conexão
wm.setAPStaticIPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
wm.setSTAStaticIPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));

    mqttInit();
    setClock();

    struct_systemConfig.datetime = getCurrentDateTime();
  }
}

void loop()
{
    // Processa o WiFiManager para manter o AP ativo
    wm.process();
    unsigned long currentMillis = millis();


    if (currentMillis - lastOtaCheck >= otaInterval)
    {
      lastOtaCheck = currentMillis;
      Serial.println("🔄 Verificando OTA...");
      fn_update(); // Checa atualização no servidor
    }

    if (currentMillis - lastSensorReadTime >= 60000)
    {
      lastSensorReadTime = currentMillis;

      Serial.println("Iniciando leitura dos sensores...");
      readSensors();

      struct_systemConfig.datetime = getCurrentDateTime();
      resetRainDaily();

      salvarDados();
      mqttInit();
      mqttIsConected();
      streamingData();

      Serial.print("Eventos de chuva detectados: ");
      Serial.println(struct_pluviometro.count);
    }
  }
