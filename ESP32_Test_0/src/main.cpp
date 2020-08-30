#include <Arduino.h>
#include <WiFi.h>
#include <FreeRTOS.h>
#include <ArduinoJson.h>
#include "miscellaneous.h"

#include <iostream> 
#include <unordered_map> 
using namespace std;

const char* ssid     = "BICO_2.4G";
const char* password = "Bico0388";

unordered_map<string, int> umap; 

void defaultTask(void* parameters);
void blinkTask(void* period);
void tcpServerManagerTask(void* q_handler);
xQueueHandle to_tcpServerManagerTask_queue;

void clientHandlerTask(void* client);
void serverHandlerTask(void* server);

void setup()
{
  xTaskCreate(
    defaultTask,
    "default_task", 
    4096, 
    NULL, 
    tskIDLE_PRIORITY, 
    NULL
  );
}

void loop()
{

}





//---------------------------------------------------------------------------------



void defaultTask(void* parameters)
{

  (void) parameters;

  Serial.begin(9600);
  Serial.setTimeout(200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500);
        Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  

//-----------------------------------------------

to_tcpServerManagerTask_queue = xQueueCreate(MAX_NUM_OF_MESSAGE_QUEUE_ELEMENT, sizeof(int));

//-----------------------------------------------

  TaskHandle_t blink_task_handle = NULL;
  uint32_t task_blink_period = 1000;
  xTaskCreate(
    blinkTask,
    "blink_task", 
    1024, 
    &task_blink_period, 
    tskIDLE_PRIORITY, 
    &blink_task_handle
  );
  
  xTaskCreate(
    tcpServerManagerTask,
    "tcp_server_manager_task", 
    4096, 
    NULL, 
    tskIDLE_PRIORITY, 
    NULL
  );
  
//--------------------------------------------------------------------------

  while (1)
  {  
    if(Serial.available())
    {
      String cmd = Serial.readString();

      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, cmd);
      if (error) 
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
      }
      else
      {
        String cmd_name = doc["cmd_name"];
        String server_alias = doc["server_alias"];
        uint16_t server_port = doc["server_port"];
        
        Serial.println("-------------------------");
        Serial.println(cmd_name);
        Serial.println(server_alias);
        Serial.println(server_port);
        Serial.println("-------------------------");

        // Preparing Message Queue here -------------------------------------------
        Message_Queue* temp_mq_handle = creatMQ();

        temp_mq_handle->message = createMQMessage(cmd_name.length());
        memcpy(temp_mq_handle->message, cmd_name.c_str(), cmd_name.length());
        temp_mq_handle->message[cmd_name.length()-1] = 0;

        Server_Properties* sp_handle = createSerProp();

        sp_handle->alias = createSerPropAlias(server_alias.length());
        memcpy(sp_handle->alias, server_alias.c_str(), server_alias.length());
        sp_handle->alias[server_alias.length()-1] = 0;
        
        sp_handle->port = server_port;

        temp_mq_handle->data = sp_handle;
        // Preparing Message Queue here -------------------------------------------

        if(xQueueSend(to_tcpServerManagerTask_queue, &temp_mq_handle, 1000))
        {

        }
        else
        {
          Serial.println("Command is not executed !");
        }
      }
    }

    //---------------------------

    static uint32_t check_point = millis();
    if(millis() - check_point > 500)
    {
      Serial.println(esp_get_free_heap_size());
      check_point = millis();
    };

    vTaskDelay(1);
  }
}

//-----------------------------------------------------------------------------------------------

void blinkTask(void* period)
{
  uint32_t _period = *((uint32_t*)period);

  pinMode(2, OUTPUT);

  for(;;)
  {
    digitalWrite(2, HIGH);
    vTaskDelay(_period);
    digitalWrite(2, LOW);
    vTaskDelay(_period);
  }
}

void tcpServerManagerTask(void* para)
{
  Message_Queue* _mq_handle;

  while(1)
  {
    if (xQueueReceive(to_tcpServerManagerTask_queue, &_mq_handle, 1000))
    {
      String message = _mq_handle->message;
      String server_alias = ((Server_Properties*)(_mq_handle->data))->alias;
      String server_port = String(((Server_Properties*)(_mq_handle->data))->port);

      deleteWholeSerProp(((Server_Properties*)(_mq_handle->data)));
      deleteMQMessage(_mq_handle);
      deleteMQ(_mq_handle);

      Serial.println(message);
      Serial.println(server_alias);
      Serial.println(server_port);
    }

    vTaskDelay(1);
  }
}



void clientHandlerTask(void* client)
{
  WiFiClient* _client = (WiFiClient*)client;

  while (_client->connected()) 
  {            // loop while the client's connected
    if (_client->available()) 
    {             // if there's bytes to read from the client,
      uint8_t data = (uint8_t)(_client->read());
      Serial.write(data);
    }

    vTaskDelay(1);
  }
  
  Serial.print("Client stop: ");
  Serial.print(_client->remoteIP());
  Serial.print(":");
  Serial.print(_client->remotePort());
  Serial.print(" on ");
  Serial.print(_client->localIP());
  Serial.print(":");
  Serial.println(_client->localPort());

  _client->stop();
  delete _client;
  
  vTaskDelete(NULL);
}

void serverHandlerTask(void* server)
{
  WiFiServer* _server = (WiFiServer*)server;

  while(1)
  {
    WiFiClient client_0 = _server->available();

    if (client_0) 
    {          
      WiFiClient* client = new WiFiClient();
      *client = client_0;
      
      String client_id = String(client->remoteIP().toString()) + ":" + String(client->remotePort()) + " on " + String(client->localIP().toString()) + ":" + String(client->localPort());
      Serial.print("New client: ");
      Serial.println(client_id);

      xTaskCreate
      (
        clientHandlerTask,
        client_id.c_str(),
        2048,
        (void*)client,
        tskIDLE_PRIORITY,
        NULL
      );
    }

    vTaskDelay(1);
  }
}