#ifndef _MISCELLANEOUS_H_
#define _MISCELLANEOUS_H_

#include <Arduino.h>
#include <WiFi.h>

typedef struct 
{
    WiFiClient* client_handler;
    TaskHandle_t* task_handler;
}Client_Instance_Task;


#endif