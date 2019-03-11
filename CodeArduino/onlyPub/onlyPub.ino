#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "FIL TPLink 1";
const char* password = "filtplink1111";
const char* mqttServer = "192.168.0.101";
const int mqttPort=1883;
const char* mqttUser="fillab";
const char* mqttPassword="cyangit";

const int ledStatus = 14;

WiFiClient espClient;
PubSubClient client(espClient);

int hum;
String msg;
String macAdd;

void getHum()
{
  hum = analogRead(A0);
  hum = map(hum,0,1023,100,0);
  msg = "{\"mac\":\""+macAdd+"\",\"param\":{\"hum\":"+hum+"}}";
  int len = msg.length();
  char charBuf[len+1];
  msg.toCharArray(charBuf, len+1);
}
void reconnect()
{
    while(!client.connected())
  {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client",mqttUser,mqttPassword))
    {
      Serial.println("Connected!!!");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}
void setupWifi()
{
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(ledStatus,LOW);
  Serial.println("Connected to Wifi!!");
  macAdd= WiFi.macAddress();

  client.setServer(mqttServer,mqttPort);
}
void setup()
{
  pinMode(ledStatus,OUTPUT);
  digitalWrite(ledStatus,HIGH);
  Serial.begin(115200);
  Serial.setTimeout(2000);
  
  while(!Serial) { }
  setupWifi();
  reconnect();
  
  hum = analogRead(A0);
  hum = map(hum,0,1023,100,0);
  msg = "{\"mac\":\""+macAdd+"\",\"param\":{\"hum\":"+hum+"}}";
  int len = msg.length();
  char charBuf[len+1];
  msg.toCharArray(charBuf, len+1);
  client.publish("/data/packet",charBuf);
  delay(2000);
  digitalWrite(ledStatus,HIGH);
  Serial.println("I'm awake, but I'm going into deep sleep mode for 30 seconds");
  ESP.deepSleep(25e6);
  
}
void loop()
{
 
}
