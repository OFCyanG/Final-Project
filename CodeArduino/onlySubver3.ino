
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Update these with values suitable for your network.

const char* ssid = "FIL TPLink 1";
const char* password = "filtplink1111";
const char* mqttServer = "filiotteam.ml";
const int   mqttPort=1883;
const char* mqttUser="fillab";
const char* mqttPassword="123123123";

int cout = 0;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
int value = 0;
const int pinCtrl = 5;
String macAdd;

void execution(int act)
{
  if (act == 0)
  {
    Serial.println("No");
    digitalWrite(BUILTIN_LED, HIGH);
    digitalWrite(pinCtrl, LOW);
  }
  if (act == 1)
    {
    Serial.println("Litter");
    digitalWrite(BUILTIN_LED, LOW);
    digitalWrite(pinCtrl, HIGH);
    delay(4000);
    digitalWrite(BUILTIN_LED, HIGH);
    digitalWrite(pinCtrl, LOW);
  }
  if (act == 2)
  {
    Serial.println("Litter");
    digitalWrite(BUILTIN_LED, LOW);
    digitalWrite(pinCtrl, HIGH);
    delay(7000);
    digitalWrite(BUILTIN_LED, HIGH);
    digitalWrite(pinCtrl, LOW);
  }
}
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  macAdd = WiFi.macAddress();
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String cmdString;
  DynamicJsonDocument doc(1024);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    cmdString = cmdString + String((char)payload[i]);
  }
  Serial.println();
  deserializeJson(doc, cmdString);
  JsonObject obj = doc.as<JsonObject>();
  
  if (obj[String("mac")] == macAdd)
  {
    if (obj[String("cmd")] == "pump")
    {
      execution(obj[String("lvl")]);
      client.publish("/outTopic", "Pump Done");
    }
    if (obj[String("cmd")] == "available?")
    {
      client.publish("/outTopic", "available");
    }
    if (obj[String("cmd")] == "sleep")
    {
      Serial.println("Into Sleep Mode");
      ESP.deepSleep(obj[String("time")]);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if (client.connect(clientId.c_str(),mqttUser,mqttPassword))
    if (client.connect("ESP8266ClientAC",mqttUser,mqttPassword))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("/outTopic", "CONN");
      // ... and resubscribe
      client.subscribe("/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(pinCtrl, OUTPUT);
  digitalWrite(pinCtrl, LOW);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  
}
