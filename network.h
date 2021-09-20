#include "private.h"

#if defined(ESP8266)
#define MAX_SRV_CLIENTS 8
#include <ESP8266WiFi.h>
//how many clients should be able to telnet to this ESP8266
// WiFiServer server(80);
// WiFiClient client[MAX_SRV_CLIENTS];
#elif defined(ESP32)
#include <WiFi.h>
#endif

//MQTT
// #include <PubSubClient.h>
WiFiClient WiFi_client;
// PubSubClient client(HOST_BROKER, HOST_PORT, BB_SUB_PUB::callback, espClient);
#define STATUS "dev/status"
#define FEED "dev/feed"
#define COMMAND "dev/command"

const int MSG_BUFF_SIZE = 50;
namespace
{
    char ip[16];
    char octet[3];
    char payload[MSG_BUFF_SIZE];
    char client_name[MSG_BUFF_SIZE];
};

namespace network
{
    void set_ip();
    char *get_client_name();

    void setup_WiFi()
    {
        delay(10);
        //connect to WiFi
        Serial.printf("Connecting to %s ", _SSID);

        WiFi.begin(_SSID, _PASSWRD);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(800);
            Serial.print(".");
        }
        // randomSeed(micros());

        Serial.println();
        Serial.println("WiFi connected");
    }

    void set_ip()
    {
        // get ip
        strcpy(ip, WiFi.localIP().toString().c_str());
        // get last oct
        char *idx = strrchr(ip, '.')+1;
        Serial.printf("index: %s", idx);
        strcpy(octet, idx);

        snprintf(
            client_name,
            MSG_BUFF_SIZE,
            "%s-%s",
            DEVICE_BASENAME,
            octet);

        Serial.printf("client name: %s", client_name);

        Serial.println();
        Serial.print("IP address: ");
        Serial.println(ip);
    }

    const char *get_ip()
    {
        if (ip == nullptr)
        {
            set_ip();
            Serial.println(octet);
        }
        return octet;
    }

    char *get_client_name()
    {
        Serial.print("Client Name: ");
        Serial.println(client_name);
        return client_name;
    }

    char *response_msg(const char *connected_device, int status)
    {
        switch (status)
        {
        case 0:
            sprintf(payload, "Turning %s OFF", connected_device);
        case 1:
            sprintf(payload, "Turning %s ON", connected_device);
        default:
            const char *err = "no action was taken";
            memcpy(payload, err, sizeof(err) * sizeof(char));
        }
        Serial.println(payload);
        return payload;
    }

    // void put(const char *msg, int state)
    // {
    //     sprintf(payload, "%s %d", msg, state);
    //     server.send(200, "text / plain", payload);
    // }

}; // namespace network

void _callback(char *topic, uint8_t *incoming, unsigned int length)
{
    // // In order to republish this payload, a copy must be made
    // // as the orignal payload buffer will be overwritten whilst
    // // constructing the PUBLISH packet.

    // // Allocate the correct amount of memory for the payload copy
    // char *p = (char *)malloc(length);
    // // Copy the payload to the new buffer
    // memcpy(p, payload, length);
    // mqtt_client.publish(FEED, p);
    // // Free the memory
    // free(p);
}

// class BB_SUB_PUB //: PubSubClient
// {
// private:
//     PubSubClient _client /*(HOST_BROKER, HOST_PORT, callback, espClient)*/;
//     //void callback(char *topic, byte *incoming, unsigned int length);
//     // WiFiClient client;

// public:
//     BB_SUB_PUB(){};
//     void init_client()
//     {
//         // _client = PubSubClient(WiFi_client);
//         Serial.println();
//         Serial.println("setting sever...");
//         _client.setServer(HOST_BROKER_ADDR, HOST_PORT);
//         Serial.println("setting callback...");
//         _client.setCallback(_callback);
//         Serial.println("setting client...");
//         _client.setClient(WiFi_client);
//         // _client.setClient(WiFi_client);

//         // Serial.print("connecting to...");
//         // Serial.print(BB_Network::get_client_name());
//         // _client.connect(BB_Network::get_client_name(), HOST_NAME, HOST_PASS);
//     }

//     // needs to go offline for a set amount of time before retrying.

//     void reconnect()
//     {
//         Serial.println("Attempting MQTT connection...");
//         // Attempt to connect
//         // Loop until we're reconnected
//         Serial.print("connecting.");
//         while (!_client.connected())
//         {
//             _client.connect(client_name, HOST_NAME, HOST_PASS);
//         }
//         if (_client.connected())
//         {
//             Serial.println("connected");
//             // Once connected, publish an announcement...
//             _client.publish(STATUS, client_name);
//             // ... and resubscribe
//             _client.subscribe(COMMAND);
//         }
//         else
//         {
//             Serial.print("failed, rc=");
//             Serial.print(_client.state());
//             Serial.println(" try again in 5 seconds");
//             // Wait 5 seconds before retrying
//             delay(5000);
//         }
//     }

//     void listen()
//     {
//         Serial.println("looping.");
//         if (!_client.connected())
//         {
//             reconnect();
//         }
//         _client.loop();
//         char msg[MSG_BUFF_SIZE];
//         snprintf(msg, MSG_BUFF_SIZE, "%s:%s", client_name, ip);
//         _client.publish(FEED, msg);
//     }

//     void publish(const char *destination, const char *payload)
//     {
//         if (destination == const_cast<char*>(STATUS))
//         {
//             char temp_str[MSG_BUFF_SIZE];
//             snprintf(temp_str, MSG_BUFF_SIZE, "Hello from %s", payload);
//             payload = temp_str;
//         }
//         _client.publish(destination, payload);
//     }

//     void subscribe(const char *topic)
//     {
//         _client.subscribe(topic);
//     }

// }; // namespace BB_SUB_PUB

// void setup_endpoints()
// {
//     // setup endpoints
//     Serial.println("setting up endpoints...");
//     server.on("/", [&]()
//               {
//                   String msg = "";
//                   // Call the 'handleRoot' function when a client requests URI "/"
//                   // check status of lights
//                   msg = Lights::get_status(lights);

//                   Network::put(msg);
//               });

//     server.on("/info", []()
//               {
//                   //TODO: respond w/ device info as JSON.
//                   Network::put("server_name: \t" + String(Info::device_name) +
//                                "\nserver_type: \tnode" + "\nendpoints:" +
//                                "\n" + "\t\"/\"" + "\n\t\"/info\"" + "\n\t\"/toggle_led\"");
//               });

//     server.on("/toggle_led", [&]()
//               { Network::put("Not available"); });

//     // server.on("/read", []() {
//     //     Network.put("");
//     // });

//     server.onNotFound(Network::handleNotFound); // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
// }
