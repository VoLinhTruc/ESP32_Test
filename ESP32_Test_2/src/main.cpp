#include <Arduino.h>
#include <WiFi.h>

#include "Tree_Back_Model/Tree_Back_Model.h"
#include "Tree_Back_Model/Tree_Back_Managing_Task.h"
#include "Tree_Back_Model/Tree_Back_Executing_Task.h"

#include "Bico_MQTT_Client.h"

const char* test_root_ca= \
		"-----BEGIN CERTIFICATE-----\n" \
	"MIIEAzCCAuugAwIBAgIUBY1hlCGvdj4NhBXkZ/uLUZNILAwwDQYJKoZIhvcNAQEL\n" \
	"BQAwgZAxCzAJBgNVBAYTAkdCMRcwFQYDVQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwG\n" \
	"A1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1vc3F1aXR0bzELMAkGA1UECwwCQ0ExFjAU\n" \
	"BgNVBAMMDW1vc3F1aXR0by5vcmcxHzAdBgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hv\n" \
	"by5vcmcwHhcNMjAwNjA5MTEwNjM5WhcNMzAwNjA3MTEwNjM5WjCBkDELMAkGA1UE\n" \
	"BhMCR0IxFzAVBgNVBAgMDlVuaXRlZCBLaW5nZG9tMQ4wDAYDVQQHDAVEZXJieTES\n" \
	"MBAGA1UECgwJTW9zcXVpdHRvMQswCQYDVQQLDAJDQTEWMBQGA1UEAwwNbW9zcXVp\n" \
	"dHRvLm9yZzEfMB0GCSqGSIb3DQEJARYQcm9nZXJAYXRjaG9vLm9yZzCCASIwDQYJ\n" \
	"KoZIhvcNAQEBBQADggEPADCCAQoCggEBAME0HKmIzfTOwkKLT3THHe+ObdizamPg\n" \
	"UZmD64Tf3zJdNeYGYn4CEXbyP6fy3tWc8S2boW6dzrH8SdFf9uo320GJA9B7U1FW\n" \
	"Te3xda/Lm3JFfaHjkWw7jBwcauQZjpGINHapHRlpiCZsquAthOgxW9SgDgYlGzEA\n" \
	"s06pkEFiMw+qDfLo/sxFKB6vQlFekMeCymjLCbNwPJyqyhFmPWwio/PDMruBTzPH\n" \
	"3cioBnrJWKXc3OjXdLGFJOfj7pP0j/dr2LH72eSvv3PQQFl90CZPFhrCUcRHSSxo\n" \
	"E6yjGOdnz7f6PveLIB574kQORwt8ePn0yidrTC1ictikED3nHYhMUOUCAwEAAaNT\n" \
	"MFEwHQYDVR0OBBYEFPVV6xBUFPiGKDyo5V3+Hbh4N9YSMB8GA1UdIwQYMBaAFPVV\n" \
	"6xBUFPiGKDyo5V3+Hbh4N9YSMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEL\n" \
	"BQADggEBAGa9kS21N70ThM6/Hj9D7mbVxKLBjVWe2TPsGfbl3rEDfZ+OKRZ2j6AC\n" \
	"6r7jb4TZO3dzF2p6dgbrlU71Y/4K0TdzIjRj3cQ3KSm41JvUQ0hZ/c04iGDg/xWf\n" \
	"+pp58nfPAYwuerruPNWmlStWAXf0UTqRtg4hQDWBuUFDJTuWuuBvEXudz74eh/wK\n" \
	"sMwfu1HFvjy5Z0iMDU8PUDepjVolOCue9ashlS4EB5IECdSR2TItnAIiIwimx839\n" \
	"LdUdRudafMu5T5Xma182OC0/u/xRlEm+tvKGGmfFcN0piqVl8OrSPBgIlb+1IKJE\n" \
	"m/XriWr/Cq4h/JfB7NTsezVslgkBaoU=\n" \
	"-----END CERTIFICATE-----\n";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(100);
  
  
  // WiFi.begin((char*)NULL, (char*)NULL, 0, 0, 0);

  // TB_Task* tb_main_task;
	// createTBTask(NULL, 5, sizeof(TB_MQ), NULL, 0, 0, tbManagingTask, "Main_Task", 2048, tskIDLE_PRIORITY, &tb_main_task);
	// createTBTask(NULL, 0, 0, tb_main_task->qin, 0, 0, tbGetUserInput, "User Input", 1024, tskIDLE_PRIORITY, NULL);
  
	// createTBTask(NULL, 0, 0, NULL, 0, 0, test1, "test1", 1024, tskIDLE_PRIORITY, NULL);

  Serial.println("WiFi connecting...!!!");
  const char* ssid     = "BICO_2.4G";
  const char* password = "Bico0388";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 


}

void loop() {
  // put your main code here, to run repeatedly:

  Bico_MQTT_Client mqtt;
  BEGIN_PROPERTIES_TYPE mqtt_begin_prop;
  
  mqtt_begin_prop.server_address = (char*)"192.168.1.13";
  mqtt_begin_prop.server_port = 1883;

  // mqtt_begin_prop.server_address = (char*)"test.mosquitto.org";
  // mqtt_begin_prop.server_port = 8883;
  // mqtt_begin_prop.ca_cert = (char*)test_root_ca;

  Serial.println(mqtt.begin(mqtt_begin_prop));

{
  MQTT_Connect_Packet packet;
  MQTTConnectInitiation(&packet, 0, NULL, 0, NULL);
  packet.clean_session = 0;
  Serial.println("Connect");
  Serial.println(mqtt.connect(&packet));
}



{
  MQTT_Subscribe_Packet packet;
  MQTTSubscribeInitiation(&packet, 7, 5, "test1");
  Serial.println("Subscribe");
  Serial.println(mqtt.subscribe(&packet));
}



Serial.println("Available:");
Serial.println(mqtt.available());

{
  MQTT_Publish_Packet packet;
  MQTTPublishInitiation(&packet, 5, "test1", 6, "123456");
  Serial.println("Publish");
  Serial.println(mqtt.publish(&packet));
}



Serial.println("Available:");
Serial.println(mqtt.available());

{
  MQTT_Publish_Packet packet;
  MQTTPublishInitiation(&packet, 5, "test1", 6, "123456");
  Serial.println("Publish");
  Serial.println(mqtt.publish(&packet));
}


Serial.println("Available:");
Serial.println(mqtt.available());

{
  MQTT_Unsubscribe_Packet packet;
  MQTTUnsubscribeInitiation(&packet, 8, 5, "test1");
  Serial.println("Unsubscribe");
  Serial.println(mqtt.unsubscribe(&packet));
}



Serial.println("Available:");
Serial.println(mqtt.available());

{
  MQTT_Publish_Packet packet;
  MQTTPublishInitiation(&packet, 5, "test1", 6, "123456");
  Serial.println("Publish");
  Serial.println(mqtt.publish(&packet));
}



Serial.println("Available:");
Serial.println(mqtt.available());

{
  Serial.println("Ping");
  Serial.println(mqtt.pingreq());
}



{
  Serial.println("Disconnect");
  Serial.println(mqtt.disconnect());
}



  Serial.println(esp_get_free_heap_size());
  delay(2000);

}


