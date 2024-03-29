#include <WiFi.h>
#include <PubSubClient.h>
//*****************************************
//// AP + STA MODE TOGETHER………


// Local Access point Ip Address……
IPAddress local_IP(192,168,4,15);
IPAddress subnet(255,255,255,0);

// Cloud connection details
WiFiClient   espClient;
PubSubClient pubsubclient(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
const char *ssid ="JioFiber-ahGu7";
const char *password = "welcome2ibm";
const char* mqtt_server = "m12.cloudmqtt.com";

IPAddress staticIP(192,168,4,15);
IPAddress subnet2(255,255,255,0);

WiFiServer server(80);
void callback(char* topic, byte* payload, unsigned int length) {
}

void reconnect() {
  //this only applies to the cloud connection.
  // Loop until we're reconnected
  while (!pubsubclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (pubsubclient.connect(clientId.c_str(), "oxefqvkn", "uTM7RdarxTPA")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      pubsubclient.publish("outTopic", "hello world");
      // ... and resubscribe
      pubsubclient.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubsubclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{

    Serial.begin(9600);
        
    Serial.println ("Setting Soft-AP ") ;    
    WiFi.softAP ("Wemos_AP", "Wemos_comm");
    WiFi.softAPConfig(local_IP, local_IP, subnet);
    
    Serial.println(WiFi.softAPIP());
    server.begin();

    ////////////// . Part 2    /////////////
    
    Serial.println();
    
    WiFi.begin(ssid, password);
    
    //WiFi.config(staticIP, staticIP, subnet2);
    
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print ("." );      
    }
    
    Serial.println("Connected to cloud successfully");
    Serial.println(WiFi.localIP());
    pubsubclient.setServer(mqtt_server, 19757);
    pubsubclient.setCallback(callback);
}

void loop() {

  // Part 1 // Local Access Point
  WiFiClient client = server.available();
  if (!client) {return;}
  String request = client.readStringUntil('\r');
  Serial.println("********************************");
  Serial.println("From the station: " + request);
  client.flush();
  Serial.print("Byte sent to the station: ");
  Serial.println(client.println(request + "__recieved" + "\r"));

  // Part 2 .  Remote Cloud Point //

  if (!pubsubclient.connected()) {
    reconnect();
  }
  pubsubclient.loop();
  
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    snprintf (msg, 75, "%s", request.c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    
      /* ... */
      pubsubclient.publish("tempr", msg);
  }
}
