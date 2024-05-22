// -------------------------------------------- Librerías----------------------------------------------------

#include <SPI.h> // comunicacion
#include <LoRa.h> // // comunicacion
#include <ArduinoJson.h> // procesamiento de datos
#include <Wire.h> //  comunicacion
#include <WiFi.h>
#include <PubSubClient.h>


#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23
#define DIO0 26

// Creación de variables para el monitoreo electrico y datos de la jetson
// DC

float BusVoltageDC;
float ShuntVoltageDC;
float CurrentDC;
float PowerDC;
#define r_shunt 0.1 

//AC
float voltage;
float current;
float power;
float energy;
float frequency;
float pf;

DynamicJsonDocument package(1024);
//JSON

float FPS;
//char Id_count[8];
long id_count = 0;
const char* timeString;
char Time[7];
char jsonFile[2000];
String receivedData;

// WiFi

const char *ssid = "Tenda_E5D488"; // Enter your Wi-Fi name
const char *password = "Tenda123";  // Enter Wi-Fi password

/*
const char *ssid = "Red 2do Piso*"; // Enter your Wi-Fi name
const char *password = "r1r2r3r4r5";  // Enter Wi-Fi password
*/

// MQTT Broker
const char *mqtt_broker = "192.168.32.33";
const char *topic = "testtopic/2";
const char *mqtt_username = "admin";
const char *mqtt_password = "emqx1234";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DIO0);

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  //LoRa.setSyncWord(0xB7); // palabra de sincronización


  //----------------------------------------------------
  // En esta parte se hace la conección a wifi y se suscribe y publica al topico MQTT
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // Publish and subscribe
    //client.publish(topic, "Hi, I'm t-beam (gateway)");
    client.subscribe(topic);
  
}

void loop() {
  //client.loop();


  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet 
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      receivedData = LoRa.readString();
      Serial.println(receivedData);
      deserializeJson(package,receivedData);
      extract_values();
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    //------------------------------------------- MQTT
    receivedData.toCharArray(jsonFile, sizeof(jsonFile));
    client.publish(topic, jsonFile);
  }
}

void extract_values(){
  FPS = package["FPS"];
  /*const char* id_str =  package["Id_count"];
  strcpy(Id_count,id_str);*/
  id_count = package["Id_count"];
  const char* timeString =  package["Time"];
  strcpy(Time,timeString);
  voltage = package["VolAC"];
  current = package["currAC"];
  power = package["PowAC"];
  pf = package["fpAC"];
  frequency = package["freqAC"];
  energy = package["energia"];
  BusVoltageDC = package["VolDC"];
  CurrentDC = package["currDC"];
  PowerDC = package["PowDC"];
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}