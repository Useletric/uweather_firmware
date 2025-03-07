#ifndef _STRUCTS_H_
    #define _STRUCTS_H_

    /* Importação do framework Arduino */
    #include <Arduino.h>

        struct anemometro{ /* Struct referente aos dados da rede e do A7670SA */
            // --- Constantes ---
            const float pi = 3.14159265;     //Número de pi
            int period = 5000;               //Tempo de medida(miliseconds)
            int delaytime = 2000;            //Invervalo entre as amostras (miliseconds)
            int radius = 105;                //Raio do anemometro(mm)

            // --- Variáveis Globais ---
            unsigned int Sample  = 0;        //Armazena o número de amostras
            unsigned int counter = 0;        //Contador para o sensor
            unsigned int RPM = 0;            //Rotações por minuto
            float speedwind = 0;             //Velocidade do vento (m/s)
            float windspeed = 0;             //Velocidade do vento (km/h)
        };

        struct biruta{
            int wind_dir = 0;
            String direction = "";
        };

        struct pluviometro{
            volatile int count = 0;
            bool lastState = false;
            
        };

        struct bme280{
            float temp = 0;
            float umi = 0;
            float pressure =  0;
            float altitude = 0;
        };

        struct tensaoPainel{
            float voltage = 0;

        };

        struct tensaoBateria{
            float voltage = 0;

        };

        struct system{
            int timer_ReadSensors = 2000;
            int timer_MQTTConnection = 30;
            int timer_system = 0;
            char Buffer[254] = "";
            String infoSensor = "";
            String datetime = "";
            uint32_t chipId = 0;
            uint64_t multiplicador = 0xA93;
            int maxTentativasConexao = 15;
            int tentativasConexao = 0;
            boolean sd_storage = false;
            /* configuraçãoes da REDE e broker MQTT*/
            String ssid     = "";
            String password = "";
            /* configuraçãoes do broker MQTT*/
            String mqttServer = "";
            String idStation = "";
            int mqttPort = 1883;
            String frw_version = "v@1.2.8";

            
        };

        extern struct anemometro    struct_anemometro;
        extern struct biruta        struct_biruta;
        extern struct pluviometro   struct_pluviometro;
        extern struct bme280        struct_bme280;
        extern struct tensaoPainel  struct_tensaoPainelSolar;
        extern struct tensaoBateria struct_tensaoBateriaInterna;
        extern struct system        struct_systemConfig;

#endif