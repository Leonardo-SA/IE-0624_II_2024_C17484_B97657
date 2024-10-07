import serial

# Configuración de archivo y puerto serial
fileName = "output.csv"
ser = serial.Serial(port='COM3', baudrate=9600, timeout=20)

# Abrir archivo y escribir encabezado
with open(fileName, mode='w', newline='') as file:
    file.write("Volt1; Volt2; Volt3; Volt4\n")
    
    # Leer datos del puerto serial y escribir en el archivo
    for _ in range(200):  # Leer 200 líneas
        linea_serie = ser.readline().decode().strip()
        valores = linea_serie.split(',')
        
        if len(valores) == 4:  # Asegurarse de que hay 4 valores
            file.write(";".join(valores) + "\n")

ser.close()
