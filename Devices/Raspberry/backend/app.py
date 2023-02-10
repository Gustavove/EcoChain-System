from src.Classes.etheriumContract import EtheriumContract
from src.Classes.IPFShttp import IPFSconnection
from flask import Flask, request
import calendar
import time
import binascii
import json
from src.Modules import config

app = Flask(__name__)

#tenporal variables until wallet class will be created
MY_ADDRESS = '0x21e517bf6De93b1D783fEB84ddE42F589d845CEB'  # address
MY_PRIVATE_KEY = '8a30ed9c3bf9f8270a180f312fd3bda19a8ef5a9346f8d984b5405d864d9a98c'

PATH_SC_TRUFFLE = '/home/gustavo/Documentos/Universidad/TFG/Project/SmartContracts'  # smart contract path
config_info = config.init()
etheriumComunication = EtheriumContract(config_info['url'],
                                        config_info['chainid'],
                                        config_info['gasprice'],
                                        config_info['gaslimit'],
                                        config_info['contract_address'])
clientIPFS = IPFSconnection(config_info['max_data_to_send'])

@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"

@app.route('/init', methods=['GET']) #almacenar los datos del contrato en un archivo aparte y comprobar antes si ya existe uno
def init():
    contract_address = config_info['contract_address']
    if contract_address == 0x0:
        contract_address = etheriumComunication.deployContract(MY_ADDRESS, MY_PRIVATE_KEY, PATH_SC_TRUFFLE + '/build/contracts/StorageContract.json', 2000000)
        config.new_contract(contract_address)
        config_info['contract_address'] = contract_address
    return 'Contract deployed with address: ' + str(contract_address)

@app.route('/provider/new', methods=['POST'])
def new_provider():
    provider_address = request.form["provider_address"]
    param = [provider_address]
    etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, 'newProvider(address)', param, 2000000)
    return 'ok'

@app.route('/sensor/adddata', methods=['POST'])
def new_data():
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
        print(str(data_signed))
        valor = [cid,'https://ipfs.io/ipfs/' + cid, time_stamp, data_signed.v, data_signed.r.to_bytes(32, byteorder='big'), data_signed.s.to_bytes(32, byteorder='big'), data_signed.messageHash]
        etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, 'newData(string,string,uint256,uint8,bytes32,bytes32,bytes32)', valor, 2000000)
        result = "Data uploaded to IPFS and blockchain with cid: " + cid
    return result


@app.route('/provider/getcids', methods=['GET'])
def provider_cids():
    query_parameters = request.args
    provider_address = query_parameters.get("provider_address")
    function = "getDataProviderCids(address)"
    values = [provider_address]
    returned_types = ["string[]"]
    cod = etheriumComunication.getValue(function, values, returned_types, MY_ADDRESS)
    print(str(cod))
    return 'ok'

@app.route('/provider/getall', methods=['GET'])
def provider_getall():
    function = "getAllDataProviders()"
    returned_types = ["address[]"]
    values = []
    cod = etheriumComunication.getValue(function, values, returned_types, MY_ADDRESS)
    print(str(cod))
    return 'ok'

@app.route('/sensor/getdata', methods=['GET'])
def sensor_getdata():
    query_parameters = request.args
    cid = query_parameters.get("cid")
    provider_address = query_parameters.get("provider_address")
    function = "getSensorData(string,address)"
    returned_types = ['string','string','uint256','uint8','bytes32','bytes32','bytes32']
    values = [cid, provider_address]
    cod = etheriumComunication.getValue(function, values, returned_types, MY_ADDRESS)
    print(str(cod))
    return 'ok'

@app.route('/test', methods=['GET'])
def test():

    """ data = {"gps": {"latitude": "41.376966669024604",
                            "longitude": "2.1546503841953832",
                            "altitude": "150.44"},
                    "mac": "00:1B:54:11:3A:B7",
                    "message": {"pH": "1",
                                "tds": "100"},
                    "provider": "0x21e517bf6De93b1D783fEB84ddE42F589d845CEB"} """

    cid = 'QmYevdNueNL243CNzd3PExfeVBZmgesFGXmxbbhTbxte7B'
    current_GMT = time.gmtime()
    time_stamp = calendar.timegm(current_GMT)

    data = str(
                {'cid': cid,
                 'url':'https://ipfs.io/ipfs/' + cid,
                 'time_stamp': str(time_stamp)}
               )

    data_signed = etheriumComunication.signData(data, '8a30ed9c3bf9f8270a180f312fd3bda19a8ef5a9346f8d984b5405d864d9a98c')
    print(str(data_signed))
    valor = [data_signed.v, data_signed.r.to_bytes(32, byteorder='big'), data_signed.s.to_bytes(32, byteorder='big'), data_signed.messageHash]

    etheriumComunication.makeTransaction('0xD62b91863401862a9e2a4fF7f71c332b8d55Ff31', '32957d5252f821bdb86fb61f28234e29623e33f4c4b1103fd22c81bd3c9384bc', 'test(uint8,bytes32,bytes32,bytes32)', valor, 2000000)

    return 'ok'

if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0')

# para almacenar bytes smartcontract: ej value4 = data_signed.signature.ljust(32, b'\x00'), para decodificar: decoded_value = binascii.hexlify(value3)
