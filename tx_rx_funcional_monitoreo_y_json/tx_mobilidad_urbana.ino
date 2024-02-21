/* Codigo que recibe un archivo Json desde la Jetson nano hasta una ESP32
con la opción de modificar los datos o no, para continuar con la transmisión del archivo,
extrae los valores para 1 minuto de conteo de personas, el dato del tiempo y fps.
Aparte se sacan las variables electricas para el monitoreo de la estación Voltaje en DC y AC, 
Correiente AC Y DC, Potencia AC Y DC, factor de potencia, frecuencia y energía consumida todo se empaqueta
en un archivo json y estransmitido por LoRa al gateway
- Autor: Daniel Enrique Pazmiño Ortega
*/

//-----------------------------------------------------------------------------------------------------------------------------------
//                                                              LIBRERÍAS

// Se incluyen las librerías para la manipulación de archivos 


//#include <FS.h>   // manipulación de archivos en ESP32
#include <ArduinoJson.h>    // manipulacion de archivos Json

// Se incluye la librería para el envio de datos json a traves de comunicación LoRa
#include <SPI.h>
#include <LoRa.h>

// Se incluyen las libreías para el funcionamiento de los sensores de monitoreo elecctrico
#include <Wire.h> //  comunicacion
#include "DFRobot_INA219.h" // Sensor DC
#include <PZEM004Tv30.h> // Sensor Ac

// Se definen los puertos seriales que se van a ocupar para la comunicación con el sensor AC, en este caso son el puerto serial UART0

#define PZEM_RX_PIN 3
#define PZEM_TX_PIN 1
#define PZEM_SERIAL Serial
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);

// Se definen los puertos seriales que se van a ocupar para la comunicación con la jetson, en este caso son el puerto serial UART2

#define RX2_PIN 16 
#define TX2_PIN 17 

// Se declaran las variables encargadas de almacenar lo que se extrae del archivo json en la jetson

DynamicJsonDocument package(256);
String json;
float FPS;
long Id_count;
const char* timeString;
char Time[7];

// Se crea el objeto para manejo del sensor INA219

DFRobot_INA219_IIC     ina219(&Wire, INA219_I2C_ADDRESS4);

// Revise the following two paramters according to actual reading of the INA219 and the multimeter
// for linearly calibration
float ina219Reading_mA = 1000;
float extMeterReading_mA = 1000;

// se definen los pines para la inicicacion del modulo LoRa SX1276 con la ESP32 

#define SCK 18
#define MISO 19
#define MOSI 23
#define SS 5
#define RST 12
#define DIO0 13

// Creación de variables para el monitoreo electrico
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



void setup() {
  // ------------------------------------------------------- COMUNICACION -----------------------------------------------------

  //Serial.begin(115200); // se inicia el serial para el monitor serial

  Serial2.begin(115200, SERIAL_8N1, RX2_PIN, TX2_PIN); // Inicializar la comunicación serial con pines personalizados para la comunicacion con la jetson

  //-------------------------------------------------------- LoRa Parametros -------------------------------------------------


  // Se inicializa el modulo LoRa 
  while (!Serial);

  //Serial.println("LoRa Sender");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DIO0);

  if (!LoRa.begin(915E6)) { // frecuencia de funcionamiento
    //Serial.println("Starting LoRa failed!");
    while (1);
  }
  //Se ingresa una palabra de sincronización para evitar la interceptacion de la información 
  LoRa.setSyncWord(0xB7); //palabra de sincronización

  //Linear calibration
  ina219.linearCalibrate(ina219Reading_mA, extMeterReading_mA);
  //Serial.println();
}


void loop() {

  // la esp hace un barrido constante del bufer y solo ejectuta las actividades cuando haya información para recibir, procesar y transmitir
  if (Serial2.available() > 0) { // se evalua si existe algun valor entrante en el puerto serial, si la condicion es verdadera 
    String received_data = Serial2.readString(); // se guarda en una variable lo que se recibe de la jetson
    Serial2.println("Recibido con exito"); // y se manda una respuesta al mensaje de entrada desde la esp32
    //Serial.println("Datos recibidos: ");
    //Serial.println(received_data); // se muestra lo que llegó en el monitor serial
    
    
    DynamicJsonDocument doc(1024);  // se crea el objeto doc de capacidad de 1kB para almacenar el json
    deserializeJson(doc, received_data); // Se deserializa y guarda el json en el objeto creado en el caso de que se vaya a manipular en la ESP
  
    // se extrae las caracteristicas de interes del archivo json 
    FPS = doc["FPS"];
    timeString = doc["Time"];
    Id_count = doc["Id_count"];
    strcpy(Time,timeString); // se obtiene el tiempo en formato str
    
    // se sacan las variables electricas 
    obtenerDC();
    obtenerAC1();

    // se empaqueta en un archivo json
    montar_json();

    // se transmite por comunicación LoRa
    mandarLora();
  }

}

void mandarLora(){ // funcion de empaquetmiento y envío LoRa 

  LoRa.beginPacket(); // abre el paquete
 
  LoRa.print(json); 
  //serializeJson(package,LoRa); // envía el paquete 

  LoRa.endPacket(); // cierra el paquete
}

void obtenerDC(){ // se obtienen las variables dc con el sensor ina219
  BusVoltageDC = ina219.getBusVoltage_V();
  ShuntVoltageDC = ina219.getShuntVoltage_mV();
  CurrentDC = ShuntVoltageDC/r_shunt;
  PowerDC = CurrentDC*BusVoltageDC;
}

void obtenerAC1(){ // se obtienen las variables ac  con el sensor pzem004t v3
  delay(10);
  current = pzem.current();
  power = pzem.power();
  voltage = pzem.voltage();
  pf = pzem.pf();
  energy = pzem.energy();
  frequency = pzem.frequency();
  pf = pzem.pf();
  delay(10);
}


void montar_json(){  // se empaqueta todos los datos en un archivo json dinamico 
  package["FPS"] = FPS;
  package["Id_count"] = Id_count;
  package["Time"] = Time;
  package["VolAC"] = voltage;
  package["currAC"] = current;
  package["PowAC"] = power;
  package["fpAC"] = pf;
  package["freqC"] = frequency;
  package["energia"] = energy;
  package["VolDC"] = BusVoltageDC;
  package["currDC"] = CurrentDC;
  package["PowDC"] = PowerDC;

  serializeJson(package,json); // se lo pasa a cadena 
  Serial2.println(json); // y se imprime para revisarlo
}
