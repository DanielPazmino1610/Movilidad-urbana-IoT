#Codigo para la recepción de datos por UART desde una ESP32 
#El codigo sirve para la recepción de 
#Autor Daniel Pazmiño Ortega 


import serial #Se importa la librería SERIAL necesaria para habilitar y usar los puertos UART de la jetson

#configuración del objeto serial
ser = serial.Serial('/dev/ttyTHS1', 115200) #se seleccionan los puertos ttyTHS1 con el comando /dev/ para acceder sin los 
# permisos necesarios, 115200 son los baudios o velocidad de transmisión UART, ese valor debe ser el mismo en la 
#ESP32 y en la jetson

#lectura de datos

While True:
	if ser.in_waiting > 0: #si existe algo en el puerto serial, entonces se lee y se decodifica utf-8 formato de caracteres
		data = ser.readline().decode('utf-8') # y se almacena
		print(f"Datos recibidos: {data}") #se muestra en pantalla
		mensaje = "hola desde jetson nano" 
		ser.write(mensaje.encode('utf-8')) # y se envía un mensaje de respuesta hacia la ESP32


