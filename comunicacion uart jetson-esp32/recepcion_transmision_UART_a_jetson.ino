/* Codigo para conectar la esp32 y la jetson nano por UART
Transmisión y Recepción de un mnesaje simple
- Autor: Daniel Enrique Pazmiño Ortega
*/


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
    Serial.print("Datos recibidos: ");
    Serial.println(received_data); // se muestra lo que llegó en el monitor serial
    Serial2.println("Hola desde la ESP32"); // y se manda una respuesta al mensaje de entrada desde la esp32
  }
  // Esto es para ingresar datos por el monitor serial y enviarlos directamente a la jetson nano
  if (Serial.available() > 0) {
    String received_data0 = Serial.readString();
    Serial.print("Datos recibidos: ");
    Serial.println(received_data0);
    Serial2.println(received_data0);

  }

}
