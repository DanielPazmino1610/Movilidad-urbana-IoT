/* Codigo que recibe un archivo Json desde la Jetson nano hasta una ESP32
con la opción de modificar los datos o no, para continuar con la transmisión del archivo
json a un gateway por comunicación LoRa
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

// Se definen los puertos seriales que se van a ocupar para la comunicación con la jetson, en este caso son el puerto serial UART2

#define RX2_PIN 16 
#define TX2_PIN 17 

// se definen los pines para la inicicacion del modulo LoRa SX1276 con la ESP32 

#define SCK 18
#define MISO 19
#define MOSI 23
#define SS 5
#define RST 12
#define DIO0 13

void setup() {
  // ------------------------------------------------------- COMUNICACION -----------------------------------------------------

  Serial.begin(115200); // se inicia el serial para el monitor serial
  Serial2.begin(115200, SERIAL_8N1, RX2_PIN, TX2_PIN); // Inicializar la comunicación serial con pines personalizados para la comunicacion con la jetson

  //-------------------------------------------------------- LoRa Parametros -------------------------------------------------


  // Se inicializa el modulo LoRa 
  while (!Serial);

  Serial.println("LoRa Sender");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DIO0);

  if (!LoRa.begin(915E6)) { // frecuencia de funcionamiento
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //Se ingresa una palabra de sincronización para evitar la interceptacion de la información 
  //LoRa.setSyncWord(0xB7); //palabra de sincronización

}


void loop() {
  if (Serial2.available() > 0) { // se evalua si existe algun valor entrante en el puerto serial, si la condicion es verdadera 

    String received_data = Serial2.readString(); // se guarda en una variable lo que se recibe de la jetson
    Serial2.println("Recibido con exito"); // y se manda una respuesta al mensaje de entrada desde la esp32
    Serial.println("Datos recibidos: ");
    Serial.println(received_data); // se muestra lo que llegó en el monitor serial
    
    

    DynamicJsonDocument doc(256);  // se crea el objeto doc de capacidad de 1kB para almacenar el json
    deserializeJson(doc, received_data); // Se deserializa y guarda el json en el objeto creado en el caso de que se vaya a manipular en la ESP
    

    //String docu = serializeJson(doc, Serial);

    LoRa.beginPacket();
    //LoRa.print(doc);
    //deserializeJson(doc, LoRa);
    serializeJson(doc, LoRa); //Se envía el archivo por LoRa
    
    LoRa.endPacket();

  }
}