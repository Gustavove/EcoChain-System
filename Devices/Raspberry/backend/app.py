from src.Classes.etheriumContract import EtheriumContract
from src.Classes.IPFShttp import IPFSconnection
from flask import Flask, request, jsonify
import calendar
import time
import binascii
import json
from src.Modules import config
from flask_cors import CORS

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})
#CORS(app, origins=['http://10.0.2.15:3001']) solo permitir una dirección

#tenporal variables until wallet (metamask) will be created
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
    type_contract = 'already_deployed'
    if contract_address == 0x0:
        contract_address = etheriumComunication.deployContract(MY_ADDRESS, MY_PRIVATE_KEY, PATH_SC_TRUFFLE + '/build/contracts/StorageContract.json', 2000000)
        config.new_contract(contract_address)
        config_info['contract_address'] = contract_address
        type_contract = 'new'

    response = {'contract_address': str(contract_address), 'type': type_contract}
    return jsonify(response)

@app.route('/provider/new', methods=['POST'])
def new_provider():
    provider_address = request.form["provider_address"]
    #info = request.get_json()
    #provider_address = info["provider_address"]
    param = [provider_address]
    etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, 'newProvider(address)', param, 2000000)
    return 'ok'

@app.route('/sensor/adddata', methods=['POST'])
def new_data():
    sensor_data = request.form["data"]
    data = json.loads(sensor_data)
    sensor_id = data.get("mac")
    print(sensor_id)
    cid = clientIPFS.addData(data, sensor_id)

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

        try:
            config.save_cid(sensor_id, cid)
        except ValueError as e:
            return str(e), 404

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

@app.route('/sensor/new', methods=['POST'])
def new_sensor():
    sensor_id = request.form["id"]
    #data = request.get_json()
    #sensor_id = data.get('id')
    if sensor_id is not None:
        result, status_code = config.add_sensor(sensor_id)
        return result, status_code
    else:
        return 'Sensor id cannot be null', 400

@app.route('/sensor/remove', methods=['POST'])
def remove_sensor():
    sensor_id = request.form["id"]
    #data = request.get_json()
    #sensor_id = data.get('id')
    if sensor_id is not None:
        result, status_code = config.delete_sensor(sensor_id)
        return result, status_code
    else:
        return 'Sensor id cannot be null', 400

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
