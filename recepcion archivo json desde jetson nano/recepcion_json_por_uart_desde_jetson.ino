/* Codigo que recibe un archivo Json desde la Jetson nano hasta una ESP32
con la opción de modificar los datos o no, para continuar con la transmisión del archivo
json 
Transmisión y Recepción de un mnesaje simple
- Autor: Daniel Enrique Pazmiño Ortega
*/

// Se incluyen las librerías para la manipulación de archivos 
//#include <FS.h>   // manipulación de archivos en ESP32
#include <ArduinoJson.h>    // manipulacion de archivos Json

// Se definen los puertos seriales que se van a ocupar para la comunicación con la jetson, en este caso son el puerto serial UART2

#define RX2_PIN 16 
#define TX2_PIN 17 

void setup() {
  Serial.begin(9600); // se inicia el serial para el monitor serial
  Serial2.begin(115200, SERIAL_8N1, RX2_PIN, TX2_PIN); // Inicializar la comunicación serial con pines personalizados
}

void loop() {
  if (Serial2.available() > 0) { // se evalua si existe algun valor entrante en el puerto serial, si la condicion es verdadera 

    String received_data = Serial2.readString(); // se guarda en una variable lo que se recibe de la jetson
    Serial.println("Datos recibidos: ");
    Serial.println(received_data); // se muestra lo que llegó en el monitor serial
    Serial2.print("Recibido con exito\n"); // y se manda una respuesta al mensaje de entrada desde la esp32

    DynamicJsonDocument doc(1024);  // se crea el objeto doc de capacidad de 1kB para almacenar el json
    deserializeJson(doc, received_data); // Se deserializa y guarda el json en el objeto creado en el caso de que se vaya a manipular en la ESP

    serializeJson(doc, Serial);

  }
  

}


