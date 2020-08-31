#include <Arduino.h>
#include "Tree_Back_Model/Tree_Back_Model.h"
#include "Tree_Back_Model/Tree_Back_Managing_Task.h"
#include "Tree_Back_Model/Tree_Back_Executing_Task.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(100);
  delay(100);
  
  WiFi.begin((char*)NULL, (char*)NULL, 0, 0, 0);

  TB_Task* tb_main_task;
	createTBTask(NULL, 5, sizeof(TB_MQ), NULL, 0, 0, tbManagingTask, "Main_Task", 2048, tskIDLE_PRIORITY, &tb_main_task);
	createTBTask(NULL, 0, 0, tb_main_task->qin, 0, 0, tbGetUserInput, "User Input", 1024, tskIDLE_PRIORITY, NULL);
  
	// createTBTask(NULL, 0, 0, NULL, 0, 0, test1, "test1", 1024, tskIDLE_PRIORITY, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}

  // WiFiClient tran;
  // while(1)
  // {
  //   tran.connect("192.168.1.13", 6969, 100);
  //   // tran.stop();
  //   Serial.println(esp_get_free_heap_size());
  //   vTaskDelay(100);
  // }