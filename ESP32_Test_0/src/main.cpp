#include <Arduino.h>
#include <FreeRTOS.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "miscellaneous.h"

const char* ssid     = "BICO_2.4G";
const char* password = "Bico0388";

void defaultTask(void* parameters);
void blinkTask(void* period);
void analogReadTask(void* pin);
void clientHandlerTask(void* client);
void serverHandlerTask(void* server);

void setup()
{


  //--------------------------------------------------------------------------

  xTaskCreate(
    defaultTask,
    "default_task", 
    4096, 
    NULL, 
    tskIDLE_PRIORITY, 
    NULL
  );

  // TaskHandle_t analog_read_task_handle_1 = NULL;
  // uint8_t analog_pin_1 = 35;
  // xTaskCreate(
  //   analogReadTask,
  //   "analog_read_task_1", 
  //   1024, 
  //   &analog_pin_1, 
  //   tskIDLE_PRIORITY, 
  //   &analog_read_task_handle_1
  // );

  // #define NUM_OF_TCP_LISTENER 2
  // for (size_t i = 0; i < NUM_OF_TCP_LISTENER; i++)
  // {
  //   TaskHandle_t tcp_listen_task = NULL;
  //   uint16_t tcp_port = 100+i;
  //   char task_name[50] = "tcp_listener_";
  //   char task_index[2];
  //   itoa(i, task_index, 10);
  //   strcat(task_name, task_index);

  //   // xTaskCreate(
  //   //   analogReadTask,
  //   //   "analog_read_task", 
  //   //   1024, 
  //   //   &analog_pin, 
  //   //   tskIDLE_PRIORITY, 
  //   //   &analog_read_task_handle
  //   // );
  // }
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

  TaskHandle_t analog_read_task_handle = NULL;
  uint8_t analog_pin = 34;
  xTaskCreate(
    analogReadTask,
    "analog_read_task", 
    1024, 
    &analog_pin, 
    tskIDLE_PRIORITY, 
    &analog_read_task_handle
  );

  //--------------------------------------------------------------------------

  WiFiServer* server;
  TaskHandle_t server_task_handler = NULL;

  while (1)
  {  
    if(Serial.available())
    {
      String cmd = Serial.readString();

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, cmd);
      if (error) 
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }

      String cmd_name = doc["cmd_name"];
      uint16_t server_port = doc["server_port"];
      
      Serial.println(cmd_name);
      Serial.println(server_port);

      if(cmd_name == String("new"))
      {
        WiFiServer server_0(server_port);
        server = new WiFiServer;
        *server = server_0;
        server->begin();

        Serial.println("Server Start.");
        xTaskCreate
        (
          serverHandlerTask,
          "server_handler_task",
          4096,
          (void*)server,
          tskIDLE_PRIORITY,
          &server_task_handler
        );
      }
      else if (cmd_name == "delete")
      {
        if((server_task_handler != NULL) && (server != NULL))
        {
          vTaskDelete(server_task_handler);
          server->end();
          delete server;
          Serial.println("Server End.");
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

void analogReadTask(void* pin)
{
  uint8_t _pin = *((uint8_t*)pin);

  for (;;)
  {
    uint16_t value = analogRead(_pin);
    
    StaticJsonDocument<200> doc;
    doc["pin"] = _pin;
    doc["analog_value"] = value;
    serializeJson(doc, Serial);
    Serial.println();

    vTaskDelay(2000);
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