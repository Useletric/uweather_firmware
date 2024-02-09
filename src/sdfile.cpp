#include "sdfile.h"
#include "structs.h"

extern struct anemometro    struct_anemometro;
extern struct bme280        struct_bme280;
extern struct tensaoPainel  struct_tensaoPainelSolar;
extern struct tensaoBateria struct_tensaoBateriaInterna;
extern struct system        struct_systemConfig;


void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path){
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path){
  Serial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for(i=0; i<2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

void debug_sd(){

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  listDir(SD, "/", 0);
  createDir(SD, "/mydir");
  listDir(SD, "/", 0);
  removeDir(SD, "/mydir");
  listDir(SD, "/", 2);
  writeFile(SD, "/hello.txt", "Hello ");
  appendFile(SD, "/hello.txt", "World!\n");
  readFile(SD, "/hello.txt");
  deleteFile(SD, "/foo.txt");
  renameFile(SD, "/hello.txt", "/foo.txt");
  readFile(SD, "/foo.txt");
  testFileIO(SD, "/test.txt");
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}
void init_sd(){
    SPI.begin(SCK, MISO, MOSI);
    if(!SD.begin(CS)){
        Serial.println("Card Mount Failed");
        return;
    }

}

void getID(){
    for(int i = 0; i < 17; i = i + 8) {
	  struct_systemConfig.chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
    uint64_t resultado = struct_systemConfig.chipId * struct_systemConfig.multiplicador;
    Serial.print("id: ");
	Serial.println(resultado, HEX);
}

void store_sd(const char *nomeArquivo, const JsonDocument& jsonDoc) {
  // Certifique-se de que o cartão SD está inicializado antes de tentar armazenar dados
  if (!SD.begin(CS)) {
    Serial.println("Erro ao inicializar o cartão SD.");
    return;
  }

  // Abre ou cria o arquivo no cartão SD
  File file = SD.open(nomeArquivo, FILE_APPEND);
  if (!file) {
    // Se não conseguir abrir o arquivo, tenta criar um novo
    file = SD.open(nomeArquivo, FILE_WRITE);
  }

  if (file) {
    // Serializar o objeto JSON para a string e escrever no arquivo
    serializeJson(jsonDoc, file);
    file.println();  // Adicionar uma nova linha para separar entradas sucessivas
    file.close();
    Serial.println("Dados armazenados no cartão SD com sucesso.");
  } else {
    Serial.println("Erro ao abrir/criar o arquivo no cartão SD.");
  }
}

/* Função para carregar arquivo via json e salva dados */
void loadConfiguration(const char *filename){
    // Open file for reading
  File file = SD.open(filename);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<768> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  // Copy values from the JsonDocument to the Config
    String ssid = doc["ssid"];
    String password = doc["password"];
    String mqttServer = doc["mqttServer"];   
    String idStation = doc["idStation"];

    const int timer_ReadSensors = doc["timer_ReadSensors"];
    const int timer_MQTTConnection = doc["timer_MQTTConnection"];
    const int maxTentativasConexao = doc["maxTentativasConexao"];
    const int mqttPort = doc["mqttPort"];
    struct_systemConfig.idStation = idStation;
    struct_systemConfig.ssid = ssid;
    struct_systemConfig.password = password;
    struct_systemConfig.mqttServer = mqttServer;
    struct_systemConfig.mqttServer = mqttServer;
    struct_systemConfig.mqttPort = mqttPort;
    struct_systemConfig.timer_ReadSensors = timer_ReadSensors;
    struct_systemConfig.timer_MQTTConnection = timer_MQTTConnection;
    struct_systemConfig.maxTentativasConexao = maxTentativasConexao;
    
    Serial.println("\nParâmetros Salvos");
      Serial.println("SSID: "+String(struct_systemConfig.ssid));
      Serial.println("PASSWORD: "+String(struct_systemConfig.password));
      Serial.println("MQTT SERVER: "+String(struct_systemConfig.mqttServer));
      Serial.println("MQTT PORT: "+String(struct_systemConfig.mqttPort));
      Serial.println("READ SENSORS TIME: "+String(struct_systemConfig.timer_ReadSensors));
      Serial.println("TIMER_MQTTCONNECTION: "+String(struct_systemConfig.timer_MQTTConnection));
      Serial.println("MAX TENTATIVAS: "+String(struct_systemConfig.maxTentativasConexao));
      
  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
} 

void salvarDados(){
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

void salvarLeitura() {
  // Criar um objeto JSON
  StaticJsonDocument<200> doc;
  doc["temperature"] = struct_bme280.temp;
  doc["humidity"] = struct_bme280.umi;
  doc["pressure"] = struct_bme280.pressure;
  doc["altitude"] = struct_bme280.altitude;
  doc["windspeed"] = struct_anemometro.speedwind;
  doc["solarvolt"] = struct_tensaoPainelSolar.voltage;
  doc["batvolt"] = struct_tensaoBateriaInterna.voltage;

  // Abrir o arquivo para escrita
  File file = SD.open("/leituras.txt", FILE_APPEND);
  if (file) {
    // Serializar o objeto JSON e escrever no arquivo
    serializeJson(doc, file);
    file.println();  // Adicionar uma nova linha para separar entradas sucessivas
    file.close();
  } else {
    Serial.println("Erro ao abrir o arquivo para escrita no cartão SD.");
  }
}