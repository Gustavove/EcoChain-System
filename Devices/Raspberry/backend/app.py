from src.Classes.etheriumContract import EtheriumContract
from flask import Flask, request
import calendar
import time
import binascii

app = Flask(__name__)

#tenporal variables until wallet class will be created
MY_ADDRESS = '0x21e517bf6De93b1D783fEB84ddE42F589d845CEB'  # address
MY_PRIVATE_KEY = '8a30ed9c3bf9f8270a180f312fd3bda19a8ef5a9346f8d984b5405d864d9a98c'

URL = 'http://127.0.0.1:7545'  # url of my ganche node
PATH_SC_TRUFFLE = '/home/gustavo/Documentos/Universidad/TFG/Project/SmartContracts'  # smart contract path
CHAINID = 5777
PERIOD = 22
GASLIMIT = 50000000
GAS_PRICE = 1
etheriumComunication = EtheriumContract(URL, CHAINID, GAS_PRICE, GASLIMIT)

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
    data = etheriumComunication.encodeABI('newProvider(address)', param)
    etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, data, 2000000)
    return 'ok'

@app.route('/new-data', methods=['GET'])
def new_data():
    #sensor_data = request.form["data"]

    #Time-stamp
    current_GMT = time.gmtime()
    time_stamp = calendar.timegm(current_GMT)

    data = ('Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu' + 'https://ipfs.io/ipfs/Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu' + str(time_stamp))
    data_signed = etheriumComunication.signData(data, MY_PRIVATE_KEY)
    value3 = data_signed.signature.ljust(32, b'\x00')

    valors = ['Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu','https://ipfs.io/ipfs/Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu', time_stamp, value3, MY_ADDRESS]
    param = etheriumComunication.encodeABI('newData(string,string,uint256,bytes,address)', valors)

    etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, param, 2000000)
    return 'ok'
@app.route('/test', methods=['GET'])
def test():
    value = 'https://ipfs.io/ipfs/Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu'
    param = (value).encode("utf-8").ljust(32, b'\x00').hex()
    print('value hex: ' + param)
    decoded_value = bytes.fromhex(param).decode('utf-8')
    print('decoded_value' + decoded_value)

    # Time-stamp
    current_GMT = time.gmtime()
    time_stamp = calendar.timegm(current_GMT)

    data = 'Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu' + 'https://ipfs.io/ipfs/Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu' + str(time_stamp)
    signature = etheriumComunication.signData(data, MY_PRIVATE_KEY)
    print('signature is type ' + str(type(signature)) + 'and have: ' + str(signature))
    print('But I only want signature: ' + str(signature.signature.hex())) #comporbar que lo firma bien solidity

    valor = (1, 2, 'Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu','https://ipfs.io/ipfs/Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu',str(time_stamp), str(signature.signature), MY_ADDRESS)

    etheriumComunication.formatDataTransaction('test(bytes32)', valor)

    return 'ok'

@app.route('/test2', methods=['GET'])
def test2():
    # Time-stamp
    current_GMT = time.gmtime()
    time_stamp = calendar.timegm(current_GMT)

    value = 'Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu'
    value2 = 'https://ipfs.io/ipfs/Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu'
    data = value + value2 + str(time_stamp)
    data_signed = etheriumComunication.signData(data, MY_PRIVATE_KEY)
    print('value ' + str(data_signed))
    #value3 = data_signed.signature.ljust(32, b'\x00')
    value3 = data_signed.signature.ljust(32, b'\x00')
    print('coded_value ' + str(value3))
    decoded_value = binascii.hexlify(value3)
    print('decoded_value ' + str(decoded_value))
    valors = [value, value2, value3]
    param = etheriumComunication.encodeABI('test(string,string,bytes)', valors)
    etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, param, 2000000)
    return 'ok'

if __name__ == "__main__":  # There is an error on this line
    app.run(debug=True, host='0.0.0.0')
    print("test")