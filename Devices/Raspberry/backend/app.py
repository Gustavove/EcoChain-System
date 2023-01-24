from src.Classes.etheriumContract import EtheriumContract
from src.Classes.IPFShttp import IPFSconnection
from flask import Flask, request
import calendar
import time
import binascii
import json

app = Flask(__name__)

#tenporal variables until wallet class will be created
MY_ADDRESS = '0x21e517bf6De93b1D783fEB84ddE42F589d845CEB'  # address
MY_PRIVATE_KEY = '8a30ed9c3bf9f8270a180f312fd3bda19a8ef5a9346f8d984b5405d864d9a98c'

URL = 'http://127.0.0.1:7545'  # url of my ganche node
PATH_SC_TRUFFLE = '/home/gustavo/Documentos/Universidad/TFG/Project/SmartContracts'  # smart contract path
CHAINID = 1337
GASLIMIT = 50000000 #Gas maximo que puede consumir la transacción
GAS_PRICE = 1 #incentivo para los mineros
MAX_DATA_TO_SEND = 4
etheriumComunication = EtheriumContract(URL, CHAINID, GAS_PRICE, GASLIMIT)
clientIPFS = IPFSconnection(MAX_DATA_TO_SEND)

@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"

@app.route('/init', methods=['GET']) #almacenar los datos del contrato en un archivo aparte y comprobar antes si ya existe uno
def init():
    contract_address = etheriumComunication.deployContract(MY_ADDRESS, MY_PRIVATE_KEY, PATH_SC_TRUFFLE + '/build/contracts/StorageContract.json', 2000000)
    return 'new contract with address: ' + contract_address

@app.route('/new-provider', methods=['POST'])
def new_provider():
    provider_address = request.form["provider_address"]
    param = [provider_address]
    etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, 'newProvider(address)', param, 2000000)
    return 'ok'

@app.route('/new-data', methods=['POST'])
def new_data():
    result = ""

    sensor_data = request.form["data"]
    data = json.loads(sensor_data)
    cid = clientIPFS.addData(data)
    if cid == "":
        result = "Data uploaded to provider"

    else:
        #Time-stamp
        current_GMT = time.gmtime()
        time_stamp = calendar.timegm(current_GMT)

        data = (cid + 'https://ipfs.io/ipfs/' + cid + str(time_stamp))
        data_signed = etheriumComunication.signData(data, MY_PRIVATE_KEY)
        value4 = data_signed.signature.ljust(32, b'\x00') #para decodificar: decoded_value = binascii.hexlify(value3)
        valor = [cid,'https://ipfs.io/ipfs/' + cid, time_stamp, value4, MY_ADDRESS]

        etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, 'newData(string,string,uint256,bytes,address)', valor, 2000000)

        result = "Data uploaded to IPFS and blockchain with cid: " + cid
    return result

@app.route('/test', methods=['GET'])
def test():
    """ data = {"gps": {"latitude": "41.376966669024604",
                            "longitude": "2.1546503841953832",
                            "altitude": "150.44"},
                    "mac": "00:1B:54:11:3A:B7",
                    "message": {"pH": "1",
                                "tds": "100"},
                    "provider": "0x21e517bf6De93b1D783fEB84ddE42F589d845CEB"} """

    get_data = clientIPFS.getData('QmQrzxuvMBrgD9xhHnmHWRRNWMT6kHHm6PoadXE26NbpNL')
    print('type: ' + str(type(get_data)) + ' value: ' + str(get_data))

    for key in get_data:
        print('key: ' + key + ' type: ' + str(type(key)))
        print('gps: ' + str(get_data[key]['gps']))

    print('quiero coger del segundo el gps: ' + str(get_data['0']['gps'])) #En JSON las claves siempre son strings
    #print('type: ' + str(type(dic_data)) + ' value: ' + str(dic_data[0]))

    return 'ok'

if __name__ == "__main__":  # There is an error on this line
    app.run(debug=True, host='0.0.0.0')
    print("test")