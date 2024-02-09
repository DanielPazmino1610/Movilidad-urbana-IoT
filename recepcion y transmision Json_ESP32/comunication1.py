#Codigo para la recepción y envio de datos por UART con una ESP32 
#El codigo sirve para la recepción y envio de cadenas de texto por UART con un microcontrolador
#Autor Daniel Pazmiño Ortega 


import serial #Se importa la librería SERIAL necesaria para habilitar y usar los puertos UART de la jetson



# Configura el objeto Serial

ser = serial.Serial('/dev/ttyTHS1', 115200) #se seleccionan los puertos ttyTHS1 con el comando /dev/ para acceder sin los 
# permisos necesarios, 115200 son los baudios o velocidad de transmisión UART, ese valor debe ser el mismo en la 
#ESP32 y en la jetson

# Ejemplo de escritura de datos
message_to_send = "Hola desde Jetson Nano \n" # Se guarda el mensaje en una variable
ser.write(message_to_send.encode('utf-8')) # se codifica el mensaje en utf-8 y se lo envia

# lectura a de datos
while True: #entra en un bucle infinito dedicado a la lectura del puerto serial en caso de una 
    if ser.in_waiting > 0: 
    	data = ser.readline().decode('utf-8')
    	print(f"Datos recibidos: {data}")



