#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#define   DHTTYPE DHT22 
#define   DHTPin D6
BH1750    lightMeter(0x23);
DHT       dht(DHTPin, DHTTYPE); 
/*  Provide neccessary information */
//const char* ssid = "FIL TPLink 1";
//const char* password = "filtplink1111";
const char*   ssid = "BME LAB";
const char*   password = "11091976";
const char*   mqttServer = "filiotteam.ml";
const int     mqttPort=1883;
const char*   mqttUser="fillab";
const char*   mqttPassword="123123123";
/* List of variables */
float         Temperature;
float         Humidity;
float         lux;
int           hum;
String        msg;
String        macAdd;
WiFiClient    espClient;
PubSubClient  client(espClient);
/* Setup connection to MQTT server */
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
/* Setup config wifi connection */
void setupWifi()
{
  WiFi.persistent(false);
  WiFi.forceSleepWake();
  delay(2000);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to Wifi!!");
  macAdd= WiFi.macAddress();

  client.setServer(mqttServer,mqttPort);
  delay(1000);
}

/* SETUP funtion */
void setup()
{
  /* Open serial for debug */
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }
  /* Set up two line for I2C communication */
  Wire.begin(D4,D3);
  delay(100);

  /* Connect to DHT22 */
  pinMode(DHTPin, INPUT);
  dht.begin();
  /* Check module BH1750 */
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else {
    Serial.println(F("Error initialising BH1750"));
  }
  /* Connect to Wifi AP */
  setupWifi();
  /* Connect to MQTT Broker */
  reconnect();
  /* Get data from sensor */
  hum = analogRead(A0);
  hum = map(hum,0,1023,100,0);
  Temperature = dht.readTemperature(); 
  Humidity = dht.readHumidity();
  lux = lightMeter.readLightLevel();
  /*  Config message */
  msg = "{\"mac\":\""+macAdd+"\",\"param\":{\"hum\":"+hum+",\"temp\":"+Temperature+",\"humk\":"+Humidity+",\"lux\":"+lux+"}}";
  Serial.println(msg);  
  int len = msg.length();
  char charBuf[len+1];
  msg.toCharArray(charBuf, len+1);
  /* Send message to MQTT Broker */
  client.publish("/data/packet",charBuf);
  delay(2000);
  /*  Enter sleep mode */
  Serial.println("I'm awake, but I'm going into deep sleep mode for 300 seconds");
  ESP.deepSleep(300e6);
}
void loop()
{}
