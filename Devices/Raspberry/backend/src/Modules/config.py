import os
import json

cwd = os.getcwd()
PATH_FILE = os.path.join(cwd, 'config.json')

#DEFAULT INFO
URL = 'http://127.0.0.1:7545'  # url of my ganche node
CHAINID = 1337
GASLIMIT = 50000000 #Gas maximo que puede consumir la transacción
GAS_PRICE = 1 #incentivo para los mineros
MAX_DATA_TO_SEND = 4

#Guarda en un archivo de configuración los datos esenciales del projecto,, si ya existe los carga
def init():
    try:
        with open(PATH_FILE, 'r') as file:
            data = json.load(file)
            return data
    except FileNotFoundError:
        with open(PATH_FILE, 'w') as file:
            data = {'contract_address': 0x0,
                    'url': URL,
                    'chainid': CHAINID,
                    'gaslimit': GASLIMIT,
                    'gasprice': GAS_PRICE,
                    'max_data_to_send': MAX_DATA_TO_SEND,
                    'sensors': []}
            json.dump(data, file, indent=2)
            return data

#Modfifca el archivo de configuración para guardar los datos
def new_contract(contract_address):
    with open(PATH_FILE, 'r') as file:
        data = json.load(file)

    with open(PATH_FILE, 'w') as file:
        data['contract_address'] = contract_address
        json.dump(data, file, indent=2)

def add_sensor(sensor_id):
    with open(PATH_FILE, 'r') as file:
        data = json.load(file)
    sensors = data.get('sensors', [])
    if sensor_id in sensors:
        return 'Sensor already exists', 400
    sensors.append(sensor_id)
    data['sensors'] = sensors
    with open(PATH_FILE, 'w') as file:
        json.dump(data, file, indent=2)
    return 'Sensor added successfully', 200

#Elimina un sensor de la lista de sensores registrados, no se eliminan sus cids!
def delete_sensor(sensor_id):
    with open(PATH_FILE, 'r') as file:
        data = json.load(file)
    sensors = data.get('sensors', [])
    if sensor_id not in sensors:
        return 'Sensor not found', 404
    sensors.remove(sensor_id)
    data['sensors'] = sensors
    with open(PATH_FILE, 'w') as file:
        json.dump(data, file, indent=2)
    return 'Sensor deleted successfully', 200

#Guarda por cada sensor registrado sus cids uno a uno
def save_cid(sensor_id, cid):
    with open(PATH_FILE, 'r') as file:
        data = json.load(file)
    sensors = data.get('sensors', [])
    if sensor_id not in sensors:
        raise ValueError('Sensor not found')
    cids = data.get('cids', {})
    if sensor_id not in cids:
        cids[sensor_id] = []
    if cid not in cids[sensor_id]:
        cids[sensor_id].append(cid)
    data['cids'] = cids
    with open(PATH_FILE, 'w') as file:
        json.dump(data, file, indent=2)

#Obten los sensores registrados, sino hay ninguno registrado devuelve un array vacio
def get_registered_sensors():
    with open(PATH_FILE) as f:
        config = json.load(f)
        sensors = config.get('sensors', [])
    return sensors if isinstance(sensors, list) else []

#Obten los cid de un sensor
def get_cids_sensor(sensor_id):
    with open(PATH_FILE) as f:
        config = json.load(f)
        cids = config.get('cids', {})
        if sensor_id not in cids:
            return "Sensor hasn't cids"
        return cids[sensor_id]
def get_max_data_to_send():
    with open(PATH_FILE, 'r') as f:
        data = json.load(f)
        return data['max_data_to_send']


def set_max_data_to_send(value):
    with open('config.json') as f:
        data = json.load(f)
    data["max_data_to_send"] = value
    with open('config.json', 'w') as f:
        json.dump(data, f, indent=2)