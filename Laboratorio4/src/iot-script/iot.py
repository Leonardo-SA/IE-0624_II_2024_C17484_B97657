# C17484_B97657
# Import the necessary libraries for MQTT, serial communication, time, and JSON handling
import paho.mqtt.client as mqtt
import serial, time, json

# Configure the serial port to connect to the device at 115200 baud and a 1-second timeout
serial_port = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=1)
print("Connected")  # Confirm serial connection

# Variables to store data and header information (descriptive only in this case)
data_storage = []
data_header = ["Axis movement and battery level"]  # Data description
print(data_header)

# Configure the MQTT client with a unique identifier
mqtt_client = mqtt.Client("C17484_B97657")
mqtt_client.connected = False  # Set the initial state as disconnected

# Parameters for the MQTT connection
mqtt_broker = "iot.eie.ucr.ac.cr"  # MQTT broker address
mqtt_port = 1883  # Standard MQTT port
mqtt_topic = "v1/devices/me/telemetry"  # Publish topic
device_token = "9brptocshhnkv8zpx6iw"  # Device token for authentication

# Set the username (device token) and connect to the broker
mqtt_client.username_pw_set(device_token)
mqtt_client.connect(mqtt_broker, mqtt_port)
data_dict = dict()  # Initialize a dictionary to store data

# Infinite loop to read serial data and publish it to the MQTT server
while True:
    if serial_port.in_waiting > 0:  # Check if there is data on the serial port
        raw_line = serial_port.readline().decode('utf-8').strip()  # Read and decode the line of data
        
        # Split the line into parts using ',' and remove the last empty element
        parsed_parts = raw_line.split(',')[:-1]
        data_payload = {}  # Dictionary to store key-value data pairs
        
        if len(parsed_parts) > 3:  # Process data only if there are at least four elements
            for part in parsed_parts:  # Separate each part into key and value
                key, value = part.split(': ')
                data_payload[key] = float(value)  # Store each value as a float
            
            # Normalize the battery value from 0 to 100%
            data_payload["BATT"] = data_payload["BATT"] / 9 * 100  # Convert range from 0-9 to 0-100
            print(data_payload)  # Print processed data to the console
            
            # Send data to the MQTT server in JSON format
            mqtt_client.publish(mqtt_topic, json.dumps(data_payload))  # Convert to JSON for publishing
