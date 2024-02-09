#Codigo para el envío de un archivo Json por comunicación UART hacia una ESP32
#Este codigo envia el archivo completo. versión 1
#Autores: Daniel Pazmiño Ortega, Jhonny Stiven Ibarbo

# Importamos librerias necesarias
import serial #manejo del puerto serial
import pandas as pd #manejo de archivos json y otros
import time #manejo de tiempos, se usó para las pausas
import json #manejo de archivos json

# Configura el objeto Serial
ser = serial.Serial('/dev/ttyTHS1', 115200)  # Ajusta el nombre del puerto serial

#obtención del archivo Json 
datos1 = pd.read_json('/home/estacioniot/Desktop/conectionEsp/Video_01.json') #se obtiene el archivo como dataframe
#dato_envio = datos1.iloc[-1]
datos = datos1.to_json()  #se pasa el archivo completo a Json

#otra manera de pasar el archivo a json
#jsondata = json.dumps(datos)### PARA REVISAR ERROR DE LLEGADA EN DESORDEN
#print(datos)

#envio del json por UART
ser.write(datos.encode('utf-8')) #Codificacion utf-8
time.sleep(1) #pausa de un segundo

#recepcion de la confirmación de llegada
while True:
	if ser.in_waiting > 0:
		data = ser.readline().decode('utf-8')
		print(f"Datos recibidos: {data}")

