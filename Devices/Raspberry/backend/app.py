from src.Classes.etheriumContract import EtheriumContract
from src.Classes.IPFShttp import IPFSconnection
from flask import Flask, request, jsonify, make_response
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
        contract_address = etheriumComunication.deployContract(MY_ADDRESS, MY_PRIVATE_KEY, PATH_SC_TRUFFLE + '/build/contracts/StorageContract.json', 3000000)
        config.new_contract(contract_address)
        config_info['contract_address'] = contract_address
        type_contract = 'new'

    response = {'contract_address': str(contract_address), 'type': type_contract}
    return jsonify(response)

# PROVIDER API
@app.route('/provider/new', methods=['POST'])
def new_provider():
    provider_address = request.form["provider_address"]
    #info = request.get_json()
    #provider_address = info["provider_address"]
    param = [provider_address]
    etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, 'newProvider(address)', param, 2000000)
    return 'ok'

@app.route('/provider/blockchain/getall', methods=['GET'])
def provider_blockchain_getall():
    function = "getAllProviders()"
    returned_types = ["address[]"]
    values = []
    info_providers = etheriumComunication.getValue(function, values, returned_types, MY_ADDRESS)
    providers_address = info_providers[0]
    return jsonify({'providers_address':providers_address})
@app.route('/provider/blockchain/getall/sensors', methods=['GET'])
def get_blockchain_all_sensor():
    query_parameters = request.args
    provider_address = query_parameters.get("provider_address")
    function = "getMacList(address)"
    returned_types = ["string[]"]
    values = [provider_address]
    infoMACS = etheriumComunication.getValue(function, values, returned_types, MY_ADDRESS)
    sensorsMAC = infoMACS[0]
    return jsonify({'sensorsMAC':sensorsMAC})


@app.route('/provider/blockchain/sensor/getall/lastinfo', methods=['GET'])
def get_blockchain_all_sensors_lastinfo():
    result = []
    query_parameters = request.args
    provider_address = query_parameters.get("provider_address")

    #Obtenemos todos las mac de sensores de un proveedor
    function = "getMacList(address)"
    returned_types = ["string[]"]
    values = [provider_address]
    info_ids = etheriumComunication.getValue(function, values, returned_types, MY_ADDRESS)
    sensors_ids = info_ids[0]

    for sensor_id in sensors_ids:
        # Obtenemos el ultimo cid de cada sensor
        function = "getLastCID(address,string)"
        returned_types = ["string"]
        values = [provider_address, sensor_id]
        info_cid = etheriumComunication.getValue(function, values, returned_types, MY_ADDRESS)
        lastcid = info_cid[0]

        # Por cada cid obtenemos su informacion del ultimo envio
        if lastcid != "Sensor hasn't cids":
            data = clientIPFS.getData(lastcid)
            sensor_data = data
            result.append(sensor_data[3][str(config.get_max_data_to_send())])
        else:
            result.append({'mac': sensor_id})

    print(result)
    return jsonify(result)

@app.route('/provider/blockchain/sensor/lastinfo', methods=['GET'])
def get_blockchain_sensor_lastinfo():
    result = []
    query_parameters = request.args
    provider_address = query_parameters.get("provider_address")
    sensor_id = query_parameters.get("id")

    # Obtenemos el ultimo cid
    function = "getLastCID(address,string)"
    returned_types = ["string"]
    values = [provider_address, sensor_id]
    info_cid = etheriumComunication.getValue(function, values, returned_types, MY_ADDRESS)
    lastcid = info_cid[0]

    # Obtenemos informacion del ultimo cid
    if lastcid != "Sensor hasn't cids":
        data = clientIPFS.getData(lastcid)
        result = {"data": data, "max_value": config.get_max_data_to_send()}
    else:
        result = {"message":"Sensor hasn't cids", "mac":sensor_id}

    print(result)
    return jsonify(result)

@app.route('/provider/blockchain/sensor/download/allinfo', methods=['GET'])
def download_bloackchain__all_info_sensor_json():
    query_parameters = request.args
    provider_address = query_parameters.get("provider_address")
    sensor_id = query_parameters.get("id")

    # Obtenemos todos los cids de un sensor
    function = "getAllCids(address,string)"
    returned_types = ["string[]"]
    values = [provider_address, sensor_id]
    info_cid = etheriumComunication.getValue(function, values, returned_types, MY_ADDRESS)
    cids = info_cid[0]

    # Obtenemos los datos
    result = []
    for cid in cids:
        if (cid != "Sensor hasn't cids"):
            data = clientIPFS.getData(cid)
            result.append({cid:data})
        else:
            result.append("Sensor hasn't data")

    response_data = {sensor_id:result}
    response = make_response(response_data)
    response.headers['Content-Disposition'] = 'attachment; filename=' + sensor_id + '.json'
    response.headers['Content-Type'] = 'application/json'
    return response



# SENSOR API

@app.route('/sensor/adddata', methods=['POST'])
def new_data():
    print(request.form)
    #sensor_data = request.form["data"]
    #data = json.loads(sensor_data)
    data = request.get_json()
    sensor_id = data.get("mac")
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
        valor = [sensor_id,
                 cid,
                 'https://ipfs.io/ipfs/' + cid,
                 time_stamp,
                 data_signed.v,
                 data_signed.r.to_bytes(32, byteorder='big'),
                 data_signed.s.to_bytes(32, byteorder='big'),
                 data_signed.messageHash]
        etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, 'newData(string,string,string,uint256,uint8,bytes32,bytes32,bytes32)', valor, 2000000)

        try:
            config.save_cid(sensor_id, cid)
        except ValueError as e:
            return str(e), 404

        result = "Data uploaded to IPFS and blockchain with cid: " + cid
    return result

@app.route('/sensor/new', methods=['POST'])
def new_sensor():
    data = request.get_json()
    sensor_id = data.get('id')
    if sensor_id is not None:
        result, status_code = config.add_sensor(sensor_id)
        info = (sensor_id)
        info_signed = etheriumComunication.signData(info, MY_PRIVATE_KEY)
        valor = [sensor_id,
                 info_signed.v,
                 info_signed.r.to_bytes(32, byteorder='big'),
                 info_signed.s.to_bytes(32, byteorder='big'),
                 info_signed.messageHash]
        etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, "newSensor(string,uint8,bytes32,bytes32,bytes32)", valor, 2000000)
        return result, status_code
    else:
        return 'Sensor id cannot be null', 400

@app.route('/sensor/remove', methods=['POST'])
def remove_sensor():
    data = request.get_json()
    sensor_id = data.get('id')
    if sensor_id is not None:
        result, status_code = config.delete_sensor(sensor_id)
        return result, status_code
    else:
        return 'Sensor id cannot be null', 400

@app.route('/sensor/all/registered', methods=['GET'])
def get_all_registered_sensor():
    data = config.get_registered_sensors()
    return jsonify(data)

@app.route('/sensor/getcids', methods=['GET'])
def get_all_cids_sensor():
    query_parameters = request.args
    sensor_id = query_parameters.get("id")
    data = config.get_cids_sensor(sensor_id)
    return jsonify(data)

@app.route('/sensor/all/lastinfo', methods=['GET'])
def get_all_last_sensor_info():
    registered_sensors = config.get_registered_sensors()
    result = []
    for sensor_id in registered_sensors:
        info = config.get_cids_sensor(sensor_id)
        if info != "Sensor hasn't cids":
            data = clientIPFS.getData(info[-1])
            sensor_data = data
            result.append(sensor_data[3][str(config.get_max_data_to_send())])
        else:
            result.append({'mac': sensor_id})
    print(result)
    return jsonify(result)

@app.route('/sensor/lastinfo', methods=['GET'])
def get_last_info():
    query_parameters = request.args
    sensor_id = query_parameters.get("id")
    info = config.get_cids_sensor(sensor_id)
    data = clientIPFS.getData(info[-1])
    result = {"data":data, "max_value":config.get_max_data_to_send()}
    print(result)
    return jsonify(result)

@app.route('/sensor/download/allinfo', methods=['GET'])
def download_all_info_json():
    query_parameters = request.args
    sensor_id = query_parameters.get("id")

    cids = config.get_cids_sensor(sensor_id)
    result = []
    if (cids != "Sensor hasn't cids"):
        for cid in cids:
            data = clientIPFS.getData(cid)
            result.append({cid:data})
    else:
        result.append("Sensor hasn't data")

    response_data = {sensor_id:result}
    response = make_response(response_data)
    response.headers['Content-Disposition'] = 'attachment; filename=' + sensor_id + '.json'
    response.headers['Content-Type'] = 'application/json'
    return response


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
    result = etheriumComunication.verify_signature("Hola", '8d5b30b642961f9096a40e8cae4be79038165435580e218e481b8e0a4126cc5d6f96290c84f7be3f29c3a0c5bfdb7fbfef1f63b2ede4cfcb920024fc18c53be6', '082e26c62ad9782277568bc09c1025a0acc94284f8b7a3ea53dae3a102d668ff31828fdcf1ee2c3fd88d9a4e69da6c2384a3bc9b10fd0b9271693edf962e3f81')
    print(result)
    app.run(debug=True, host='0.0.0.0')

# para almacenar bytes smartcontract: ej value4 = data_signed.signature.ljust(32, b'\x00'), para decodificar: decoded_value = binascii.hexlify(value3)
