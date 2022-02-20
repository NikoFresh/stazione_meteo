#include <Arduino.h>
#include <cmath>

#include <ESP8266WiFi.h>
#include <PubSubClient.h> // mqtt lib
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include <secret.h> // keep hidden the wifi ssid and password

const char *ssid = SSID;
const char *psw = PSW;
const char *id = "current";
const char *topic = "stazione_meteo";
const char *mqttUser = MQTTUSER;
const char *mqttPass = MQTTPSW;
const int altitude = 785; // altitude in meters

IPAddress broker(192, 168, 1, 7);
WiFiClient wclient;
PubSubClient client(wclient);

Adafruit_BMP280 bme;

void WiFiConnect();
void MQTTConnect();
void sensorConnect();

void setup()
{
  Serial.begin(9600);

  WiFiConnect();                  // connect to wifi
  client.setServer(broker, 1883); // set the mqtt server
  sensorConnect();                // connect to bmp280 sensor
}

void loop()
{
  if (!client.connected())
  {
    MQTTConnect();
  }

  char payload[64];

  float outTemp = bme.readTemperature();
  float outPress = bme.readPressure() / 100; // save the pressure as hPa
  float outAltitude = bme.readAltitude();

  float barometer = outPress * pow((1 - (0.0065 * altitude) / (outTemp + 0.0065 * altitude + 273.15)), -5.257);

  sprintf(payload, "{ \"temp1\": \"%.2f\", \"pressure\": \"%.2f\", \"barometer\": \"%.2f\", \"altitude\": \"%.2f\" }", outTemp, outPress, barometer, outAltitude);

  client.publish(topic, payload);
  Serial.println("Messaggio inviato");

  delay(50);           // without it the program wont send out the payload
  ESP.deepSleep(30e6); // go into deepsleep for 30 seconds
}

void WiFiConnect()
{
  Serial.print("Connessione wifi...");
  WiFi.begin(ssid, psw);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print(" Connesso. IP: ");
  Serial.println(WiFi.localIP());
}

void MQTTConnect()
{
  Serial.print("Connessione MQTT...");
  while (!client.connected())
  {
    Serial.print(".");

    if (client.connect(id, mqttUser, mqttPass))
    {
      Serial.print(" Connesso al broker MQTT. Topic: ");
      Serial.println(topic);
      return;
    }

    delay(500);
  }
}

void sensorConnect()
{
  Serial.print("Connessione BMP280...");
  while (!bme.begin(0x76))
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Sensore connesso");
}