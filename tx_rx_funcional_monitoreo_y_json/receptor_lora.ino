// -------------------------------------------- Librerías----------------------------------------------------

#include <SPI.h> // comunicacion
#include <LoRa.h> // // comunicacion
#include <ArduinoJson.h> // procesamiento de datos
#include <Wire.h> //  comunicacion


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
float power ;
float energy ;
float frequency ;
float pf ;

DynamicJsonDocument package(1024);
//JSON

float FPS;
long Id_count;
const char* timeString;
char Time[7];

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

  LoRa.setSyncWord(0xB7); // palabra de sincronización
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet 
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      String receivedData = LoRa.readString();
      Serial.println(receivedData);
      deserializeJson(package,receivedData);
      extract_values();
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}

void extract_values(){
  FPS = package["FPS"];
  Id_count = package["Id_count"];
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