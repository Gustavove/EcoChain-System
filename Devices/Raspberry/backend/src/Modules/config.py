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
                    'max_data_to_send': MAX_DATA_TO_SEND}
            json.dump(data, file, indent=2)
            return data

#Modfifca el archivo de configuración para guardar los datos
def new_contract(contract_address):
    data = {}
    with open(PATH_FILE, 'r') as file:
        data = json.load(file)

    with open(PATH_FILE, 'w') as file:
        data['contract_address'] = contract_address
        json.dump(data, file, indent=2)

