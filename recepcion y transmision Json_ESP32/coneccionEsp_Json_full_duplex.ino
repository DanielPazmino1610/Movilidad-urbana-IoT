/*
 Este codigo tiene la funcion de crear una comunicación efectiva de mensajes y
 respuestas entre la ESP32 y la Jetson nano
 Autor Daniel Enrique Pazmiño - Universidad Autonoma de Manizales 
*/
// Define los pines personalizados para la comunicación serial
#define RX_PIN 16  // Cambia al número del pin que estás utilizando como RX
#define TX_PIN 17  // Cambia al número del pin que estás utilizando como TX

String received_data; // se crea la variable para recibir los datos via UART

void setup() {
  Serial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // Inicializar la comunicación serial con pines personalizados

  delay(1000); // Esperar 1 segundo antes de enviar el siguiente mensaje
}

void loop() {
  if (Serial.available() > 0) { // Se lee el puerto serial hasta encontrar un mensaje
    received_data = Serial.readString(); // Se lee el serial y se almacena en la variable creada anteriormente
    Serial.print("Datos recibidos: ");
    Serial.println(received_data); // se muestra lo que llegó

    Serial.print("Hola desde la ESP32"); // en envia una respuesta por el monitoe serial
  }
}
