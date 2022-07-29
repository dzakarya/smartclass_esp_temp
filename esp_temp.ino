
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "Jack";
const char* password = "namakamu";
const char* mqtt_server = "192.168.100.79";
const char* mqttUser = "smartclass";
const char* mqttPassword = "jone";
const int thermistorPin = A0;
char* senVal = "";
const char* topic = "in_temp_topic";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqttUser,mqttPassword)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, "0");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


float temperature(){
  float v = (analogRead(thermistorPin)*5.00)/1023;
  float temp = (v*20000)/(5-v);
  return v;
}

void setup() {
  pinMode(thermistorPin, INPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    float temp = temperature();
    dtostrf(temp, 4, 2, senVal);
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "%s", senVal);
    Serial.println(msg);
    client.publish(topic, msg);
  }
}
