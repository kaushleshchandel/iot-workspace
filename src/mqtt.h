#include <Arduino.h>
#include <PubSubClient.h>

#define DEFAULT_MQTT_SERVER "34.71.63.201"
#define DEFAULT_MQTT_USER "m5-beacon"
#define DEFAULT_MQTT_PASSWORD "hf*!EYfk4jQQjfG9nGf7SHQbJ5" 
#define DEFAULT_MESSAGE_LENGTH 50

const char* mqtt_server = DEFAULT_MQTT_SERVER;
const char* mqtt_user = DEFAULT_MQTT_USER;
const char* mqtt_pass = DEFAULT_MQTT_PASSWORD;

long lastMsg = 0;
char msg[DEFAULT_MESSAGE_LENGTH];
int value = 0;


String get_beacon_id()
{
  String bid  = WiFi.macAddress();
  bid.replace(":", "");
  return bid;
}

String getFullTopic(String topic )
{
  String t = get_beacon_id();
  t = t + "/" + topic ;
  return t;
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if (messageTemp == "on") {
      Serial.println("on");
    }
    else if (messageTemp == "off") {
      Serial.println("off");
    }
  }
}


//send String values
void send_mqtt_string(PubSubClient& client, String topic, String value)
{

  if (client.connected()) {
    topic = getFullTopic(topic);
    char tempStringTopic[50];
    char tempStringvalue[20];
    topic.toCharArray(tempStringTopic, topic.length() +1);
    value.toCharArray(tempStringvalue, value.length()+1);
    client.publish(tempStringTopic, tempStringvalue);
  }

}

void send_mqtt_int(PubSubClient& client, String topic, long value)
{

  if (client.connected()) {
    topic = getFullTopic(topic);
    char tempStringTopic[50];
    char tempStringvalue[20];

    topic.toCharArray(tempStringTopic, topic.length()+1);
    dtostrf(value, 1, 0, tempStringvalue);
    client.publish(tempStringTopic, tempStringvalue);

  }

}

void send_mqtt_float(PubSubClient& client, String topic, double value)
{
  if (client.connected()) {
    topic = getFullTopic(topic);
    char tempStringTopic[50];
    char tempStringvalue[20];
    topic.toCharArray(tempStringTopic, topic.length()+1);
    dtostrf(value, 1, 2, tempStringvalue);
    client.publish(tempStringTopic, tempStringvalue);
  }

}



void init_mqtt(PubSubClient& client, String sw_version, String hw_version)
{

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  String becon_id = get_beacon_id();
 

  if (client.connect("SECURE_BEACON", mqtt_user, mqtt_pass)) {
    Serial.println("MQTT connected");
    client.subscribe( "SECURE_BEACON/set");
    send_mqtt_string(client, "sw", sw_version );
    send_mqtt_string(client, "hw", hw_version );
  } else {
    // = false;
    Serial.print("failed, rc=");
    Serial.print(client.state());
  }

}

void reconnect(PubSubClient& client) {

  // will try five times
  // Loop until we're reconnected
  for (int i = 0; i < 5; i++) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("SecureClient")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
      //SEND_MQTT = true;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
      //SEND_MQTT = false;
    }
  }
}