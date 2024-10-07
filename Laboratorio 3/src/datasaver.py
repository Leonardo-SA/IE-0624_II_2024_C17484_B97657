import serial

# Nombre del archivo donde se almacenarán los datos
fileName = "output.csv"

# Encabezado de las columnas en el archivo CSV
encabezado = "Volt1; Volt2; Volt3; Volt4\n"

# Configuración del puerto serial
ser = serial.Serial(
    port='COM3',        # Puerto COM al que está conectado el dispositivo
    baudrate=9600,      # Velocidad de comunicación en baudios (bits por segundo)
    parity=serial.PARITY_NONE,  # Sin bit de paridad
    stopbits=serial.STOPBITS_ONE,  # Un bit de parada
    bytesize=serial.EIGHTBITS,  # Tamaño de los datos (8 bits)
    timeout=20         # Tiempo de espera para recibir datos (en segundos)
)

# Confirmación de la conexión con el puerto serial
print("Connected to: " + ser.portstr)

# Inicialización de variables
contador = 0          # Contador de valores procesados
valores = []          # Lista para almacenar los valores leídos de la serie
file = open(fileName, mode='w', newline='')  # Abre el archivo CSV para escritura
file.write(encabezado)  # Escribe el encabezado en el archivo

# Número de datos que se esperan recibir antes de cerrar el puerto
num_datos = 200
# Contador de datos recibidos
contador_datos = 0

# Bucle principal que continúa recibiendo datos hasta alcanzar el número deseado
while True:
    # Lee una línea de datos del puerto serial, la decodifica y elimina espacios
    linea_serie = ser.readline().decode().strip()
    # Divide la línea en valores separados por comas y los añade a la lista 'valores'
    valores.extend(linea_serie.split(','))
    print(valores)  # Imprime los valores leídos

    # Si se han recibido al menos 4 valores, procesarlos
    if len(valores) >= 4:
        while len(valores) >= 4:
            # Combina los primeros 4 valores de la lista en una sola línea separada por ";"
            linea = ";".join([str(valor) for valor in valores[:4]])
            # Escribe la línea en el archivo CSV
            file.write(linea)
            file.write("\n")
            # Elimina los primeros 4 valores de la lista para continuar con los siguientes
            valores = valores[4:]
            contador += 4  # Incrementa el contador de valores procesados

    # Incrementa el contador de datos recibidos
    contador_datos += 1
    # Si ya se han recibido el número de datos esperados, salir del bucle
    if contador_datos >= num_datos:
        break

# Cierra la conexión serial y el archivo CSV
ser.close()
file.close()

