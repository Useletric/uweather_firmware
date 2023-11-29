#ifndef STRUCTS_H
#define STRUCTS_H

    /* Importação do framework Arduino */
    #include <Arduino.h>

        struct anemometro{ /* Struct referente aos dados da rede e do A7670SA */
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
        };

        struct bme280{
            float temp = 0;
            float umi = 0;
            float pressure =  0;
            float altitude = 0;
        };

        struct tensaoPainel{

        };

        struct tensaoBateria{

        };

        struct system{
            int timer_ReadSensors = 2000;
            int timer_MQTTConnection = 30000;

        };

        extern struct anemometro    struct_anemometro;
        extern struct bme280        struct_bme280;
        extern struct tensaoPainel  struct_tensaoPainelSolar;
        extern struct tensaoBateria struct_tensaoBateriaInterna;
        extern struct system        struct_systemConfig;

#endif