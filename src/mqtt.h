#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <config.h>

long lastMsg = 0;
char msg[DEFAULT_MESSAGE_LENGTH];
int value = 0;

char will_topic[56];
char subs_topic[56];
char topic_prefix[56];

WiFiClient client;
PubSubClient mqttclient(client);

String get_beacon_id()
{
    String bid = WiFi.macAddress();
    bid.replace(":", "");
    return bid;
}

String getFullTopic(String topic)
{
    String t = get_beacon_id();
    t = t + "/" + topic;
    return t;
}

//send String values
bool send_mqtt_string(String topic, String value, bool retain)
{
    bool res = false;
    if (client.connected())
    {
        topic = getFullTopic(topic);
        char tempStringTopic[50];
        char tempStringvalue[20];
        topic.toCharArray(tempStringTopic, topic.length() + 1);
        value.toCharArray(tempStringvalue, value.length() + 1);
        res = mqttclient.publish(tempStringTopic, tempStringvalue, retain);

        if (res != true)
        {
            Serial.print(" MQTT Send Failure");
            packetsFailPM++;
        }
        else
        {
            packetsSentPM++;
        }
    }
    else
        Serial.print(" MQTT Not connected");

    return res;
}


bool send_mqtt_int(String topic, long value, bool retain)
{

    bool res = false;

    if (mqttclient.connected())
    {
        topic = getFullTopic(topic);
        char tempStringTopic[50];
        char tempStringvalue[20];

        topic.toCharArray(tempStringTopic, topic.length() + 1);
        dtostrf(value, 1, 0, tempStringvalue);
        res = mqttclient.publish(tempStringTopic, tempStringvalue);

        if (res != true)
        {
            Serial.print(" MQTT Send Failure");
            packetsFailPM++;
        }
        else
        {
            packetsSentPM++;
        }
    }
    else
        Serial.print(" MQTT Not connected");

    return res;
}

bool send_mqtt_float(String topic, double value, bool retain)
{
    bool res = false;
    if (mqttclient.connected())
    {
        topic = getFullTopic(topic);
        char tempStringTopic[50];
        char tempStringvalue[20];
        topic.toCharArray(tempStringTopic, topic.length() + 1);
        dtostrf(value, 1, 2, tempStringvalue);
        res = mqttclient.publish(tempStringTopic, tempStringvalue, retain);

        if (res != true)
        {
            Serial.print(" MQTT Send Failure");
            packetsFailPM++;
        }
        else
        {
            packetsSentPM++;
        }
    }
    else

        Serial.print(" MQTT Not connected");

    return res;
}


//Send device Configurations
void sendConfig()
{
    // Update this to a Json structure

    send_mqtt_int("config/dataFrequency", data_frequency, false);
    send_mqtt_int("config/unitCelsius", default_unit_C, false);
    send_mqtt_string("config/rootTopic", mqtt_topic_prefix, false);
    send_mqtt_string("config/subsTopic", mqtt_subs_topic, false);
    send_mqtt_int("config/timeZone", timeZone, false);
    send_mqtt_int("config/scanTime", blue_scan_time, false);
    send_mqtt_int("config/scanInterval", blue_interval, false);
    send_mqtt_int("config/scanWindow", blue_window, false);
    send_mqtt_int("config/calibrationRssi", calibrationRssi, false);
    send_mqtt_int("config/activeScan", blue_active_scan, false);
    send_mqtt_int("config/maxDistance", blue_distance_max, false);
    send_mqtt_int("config/minDistance", blue_distance_min, false);
    send_mqtt_int("config/spaceCapacity", spaceCapacity, false);
    send_mqtt_int("config/spaceThreshold", spaceThreshold, false);
    send_mqtt_int("config/spaceYellow", spaceYellow, false);
    send_mqtt_int("config/spaceGreen", spaceGreen, false);
    send_mqtt_int("config/spaceRed", spaceRed, false);
}


void callback(char *topic, byte *message, unsigned int length)
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    String topicroot = get_beacon_id();
    topicroot += "/set/";

    bool iscommandvalid = true;

    //FREQUENCY OF DATA SENDING
    if (String(topic) == topicroot + "dataFrequency")
    {
        Serial.print("Changing Frequency to ");
        data_frequency = messageTemp.toInt();
    }

    else if (String(topic) == topicroot + "calibrationRssi")
    {
        Serial.print("Changing RSSI Calibration");
        calibrationRssi = messageTemp.toInt();
    }

    // DO AN OTA BASED ON PASSED FIRMWARE
    else if (String(topic) == topicroot + "ota")
    {
        Serial.print("Custom Firmware update");
        updateFirmware(messageTemp);
    }

    else if (String(topic) == topicroot + "restart" || String(topic) == topicroot + "reboot")
    {
        Serial.print("Rebooting by mqtt");
        ESP.restart();
    }

    else if (String(topic) == topicroot + "timeZone")
    {
        Serial.print("Change the time Zone");
        timeZone = messageTemp.toInt();
    }

    else if (String(topic) == topicroot + "maxDistance")
    {
        Serial.print("Change the max Distance");
        blue_distance_max = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "minDistance")
    {
        Serial.print("Change the min Distance");
        blue_distance_min = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "scanTime")
    {
        Serial.print("Change Scan Time");
        blue_scan_time = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "scanWindow")
    {
        Serial.print("Change Scan Window");
        blue_window = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "scanInterval")
    {
        Serial.print("Change Scan Interval");
        blue_interval = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "activeScan")
    {
        Serial.print("Change ActiveScan");
        blue_active_scan = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "spaceCapacity")
    {
        Serial.print("Change spaceCapacity");
        spaceCapacity = messageTemp.toInt();
         
    }
    else if (String(topic) == topicroot + "spaceThreshold")
    {
        Serial.print("Change spaceThreshold");
        spaceThreshold = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "spaceYellow")
    {
        Serial.print("Change spaceYellow");
        spaceYellow = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "spaceRed")
    {
        Serial.print("Change spaceRed");
        spaceRed = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "spaceGreen")
    {
        Serial.print("Change spaceGreen");
        spaceGreen = messageTemp.toInt();
    }
    else if (String(topic) == topicroot + "setDefaults" || String(topic) == topicroot + "factoryreset")
    {
        Serial.print("Setting defaults");
        set_default_config();
    }
    else
    {
        iscommandvalid = false;
    }


    if (iscommandvalid)
    {
        save_config();
        read_config();
        send_mqtt_string("set/cmd", "OK", false);
        sendConfig();
    }

}
//

void subscribe_to_topic(String topic)
{
    String substopic = get_beacon_id() + "/" + topic + "/#";
    char tempStringSubsTopic[50];
    substopic.toCharArray(tempStringSubsTopic, substopic.length() + 1);
    send_mqtt_string("system/sub-topic", tempStringSubsTopic, false);
    mqttclient.subscribe(tempStringSubsTopic);
}

void init_mqtt(String sw_version, String hw_version)
{

    mqttclient.setServer(mqtt_server, default_mqtt_port);
    mqttclient.setCallback(callback);

    String sbeacon_id = get_beacon_id();
    String swilltopic = sbeacon_id + "/status";

    char tempStringWillTopic[50];
    char tempStringclientID[50];

    sbeacon_id.toCharArray(will_topic, sbeacon_id.length() + 1);
    sbeacon_id.toCharArray(tempStringclientID, sbeacon_id.length() + 1);
    swilltopic.toCharArray(tempStringWillTopic, swilltopic.length() + 1);

    if (mqttclient.connect(tempStringclientID, mqtt_user, mqtt_pass, tempStringWillTopic, 0, true, "offline"))
    {
        Serial.println("MQTT connected");

        // Subscribe to the commands
        subscribe_to_topic(DEFAULT_SUB_TOPIC);

        send_mqtt_string("system/will-topic", swilltopic, false);
        send_mqtt_string("system/software", sw_version, false);
        send_mqtt_string("system/hardware", hw_version, false);
        send_mqtt_string("status", "online", true);

        sendConfig();
        
    }
    else
    {

        //Serial.print("failed, rc=");
        switch (mqttclient.state())
        {
        case -4:
            Serial.println("MQTT_CONNECTION_TIMEOUT");
            break;
        case -3:
            Serial.println("MQTT_CONNECTION_LOST");
            break;
        case -2:
            Serial.println("MQTT_CONNECT_FAILED");
            break;
        case -1:
            Serial.println("MQTT_DISCONNECTED");
            break;
        case 1:
            Serial.println("MQTT_CONNECT_BAD_PROTOCOL");
            break;
        case 2:
            Serial.println("MQTT_CONNECT_BAD_CLIENT_ID");
            break;
        case 3:
            Serial.println("MQTT_CONNECT_BAD_PROTOCOL");
            break;
        case 4:
            Serial.println("MQTT_CONNECT_UNAVAILABLE");
            break;
        case 5:
            Serial.println("MQTT_CONNECT_UNAUTHORIZED");
            break;
        default:
            break;
        }
    }
}

void reconnect()
{

    mqttclient.setServer(mqtt_server, 1883);
    mqttclient.setCallback(callback);

    String sbeacon_id = get_beacon_id();
    String swilltopic = sbeacon_id;

    char tempStringSubsTopic[50];
    char tempStringWillTopic[50];
    char tempStringclientID[50];

    sbeacon_id.toCharArray(will_topic, sbeacon_id.length() + 1);
    sbeacon_id.toCharArray(tempStringclientID, sbeacon_id.length() + 1);

    swilltopic.toCharArray(tempStringWillTopic, swilltopic.length() + 1);

    String becon_id = get_beacon_id();

    for (int i = 0; i < 5; i++)
    {
        Serial.print("Attempting MQTT connection...");
        if (mqttclient.connect("SecureClient"))
        {
            // Subscribe to the commands
            subscribe_to_topic(DEFAULT_SUB_TOPIC);
            send_mqtt_string("status", "online", true);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqttclient.state());
            delay(2000);
        }
    }
}