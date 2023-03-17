import os
import json

cwd = os.getcwd()
PATH_FILE = os.path.join(cwd, 'temp_sensors_data.json')

#Inicia en caso que no exista el archivo
def init():
    cwd = os.getcwd()
    path_file = os.path.join(cwd, 'temp_sensors_data.json')
    if not os.path.isfile(path_file):
        with open(path_file, 'w') as file:
            json.dump({}, file, indent=2)

#Almacena los datos de un mismo sensor temporalmente, cada dato guardado tiene un id
def save_sensor_data(sensor_id, data):
    with open(PATH_FILE, 'r') as file:
        sensors_data = json.load(file)
    sensors = sensors_data.get('sensors', [])
    if sensor_id not in sensors:
        sensors.append(sensor_id)
        sensors_data['sensors'] = sensors
    if sensor_id not in sensors_data:
        sensors_data[sensor_id] = []
    count = len(sensors_data[sensor_id]) + 1
    sensors_data[sensor_id].append({count: data})
    with open(PATH_FILE, 'w') as file:
        json.dump(sensors_data, file, indent=2)
def get_last_data_id(sensor_id):
    with open(PATH_FILE, 'r') as file:
        sensors_data = json.load(file)
    if sensor_id not in sensors_data:
        return 0
    sensor_data = sensors_data[sensor_id]
    if not sensor_data:
        return 0
    last_data = sensor_data[-1]
    return list(last_data.keys())[0]

def get_sensor_data(sensor_id):
    with open(PATH_FILE, 'r') as file:
        sensors_data = json.load(file)
    if sensor_id not in sensors_data:
        raise ValueError('Sensor not found')
    return json.dumps(sensors_data[sensor_id], indent=2)

def delete_sensor_data(sensor_id):
    with open(PATH_FILE, 'r') as file:
        sensors_data = json.load(file)
    if sensor_id not in sensors_data:
        raise ValueError('Sensor not found')
    sensors_data.pop(sensor_id)
    sensors = sensors_data.get('sensors', [])
    if sensor_id in sensors:
        sensors.remove(sensor_id)
        sensors_data['sensors'] = sensors
    with open(PATH_FILE, 'w') as file:
        json.dump(sensors_data, file, indent=2)
