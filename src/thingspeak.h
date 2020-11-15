#include <WiFiClient.h>

#define THING_SPEAK_SERVER "api.thingspeak.com"
#define THING_SPEAK_API_KEY "KN9FIOSFBIR2NG6P"
const char *server = THING_SPEAK_SERVER;
String apiKey = THING_SPEAK_API_KEY;

bool send_to_thingspeak_server(WiFiClient &client, int &pm1, int &pm2, int &pm10)
{
    if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
    {
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(pm1);
        postStr += "&field2=";
        postStr += String(pm2);
        postStr += "&field3=";
        postStr += String(pm10);
        postStr += "\r\n\r\n";

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
    }
    client.stop();
    return true;
}