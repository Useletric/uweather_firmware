#ifndef _MAIN_H_
    #define _MAIN_H_
    #include "rtos.h"
    #include "sensor.h"
    #include "mqtt.h"
    #include "sdfile.h"
    #include "rtc.h"
    #include "ota.h"
    
    #define SENSOR_READ_INTERVAL 60000UL  /* Intervalo de leitura dos sensores em milissegundos */
    unsigned long lastSensorReadTime = 0; // Tempo da última leitura dos sensores

#endif //_MAIN_H_