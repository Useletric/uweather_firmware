#include "ota.h"
#include "structs.h"

extern struct system        struct_systemConfig;

void ota_prep(){
    OTADRIVE.setInfo(APIKEY, struct_systemConfig.frw_version);
    OTADRIVE.onUpdateFirmwareProgress(onUpdateProgress);
}

void fn_update(){
    // retrive firmware info from OTAdrive server
      updateInfo inf = OTADRIVE.updateFirmwareInfo();
      Serial.printf("\nfirmware info: %s ,%dBytes\n%s\n",
                    inf.version.c_str(), inf.size, inf.available ? "New version available" : "No newer version");
      // update firmware if newer available
      if (inf.available)
        OTADRIVE.updateFirmware();

      // sync local files with OTAdrive server
      OTADRIVE.syncResources();
      // list local files to serial port
      listDirSPIFFS(OTA_FILE_SYS, "/", 0);
}

void listDirSPIFFS(fs::FS &fs, const char *dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname, "r");
  if (!root)
  {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDirSPIFFS(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void onUpdateProgress(int progress, int totalt)
{
  static int last = 0;
  int progressPercent = (100 * progress) / totalt;
  Serial.print("*");
  if (last != progressPercent && progressPercent % 10 == 0)
  {
    // print every 10%
    Serial.printf("%d", progressPercent);
  }
  last = progressPercent;
}

