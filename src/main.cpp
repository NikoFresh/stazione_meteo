#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <secret.h> // keep hidden the wifi ssid and password

const char *ssid = SSID;
const char *psw = PSW;
const char *id = "current";
const char *topic = "stazione_meteo";
const char *mqttUser = MQTTUSER;
const char *mqttPass = MQTTPSW;

IPAddress broker(192, 168, 1, 7);
WiFiClient wclient;
PubSubClient client(wclient);

void WiFiConnect();
void MQTTConnect();

void setup()
{
  Serial.begin(9600);

  WiFiConnect();                  // connect to wifi
  client.setServer(broker, 1883); // set the mqtt server
}

void loop()
{
  if (!client.connected())
  {
    MQTTConnect();
  }

  client.publish(topic, ".");
  delay(5000);
}

void WiFiConnect()
{
  Serial.print("Connessione al wifi");
  WiFi.begin(ssid, psw);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnesso. IP: ");
  Serial.println(WiFi.localIP());
}

void MQTTConnect()
{
  Serial.print("Connessione MQTT");
  while (!client.connected())
  {
    Serial.print(".");

    if (client.connect(id, mqttUser, mqttPass))
    {
      Serial.print("\nConnesso al broker MQTT. Topic: ");
      Serial.println(topic);
      return;
    }

    delay(500);
  }
}