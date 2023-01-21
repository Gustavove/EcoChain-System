from src.Classes.etheriumContract import EtheriumContract
from flask import Flask, request

app = Flask(__name__)

#tenporal variables until wallet class will be created
MY_ADDRESS = '0x21e517bf6De93b1D783fEB84ddE42F589d845CEB'  # address funded in genesis file
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
    valor = (int(provider_address, base=16),)
    etheriumComunication.makeTransaction(MY_ADDRESS, MY_PRIVATE_KEY, 'newProvider(address)', valor, 2000000)
    return 'ok'

@app.route('/new-data', methods=['POST'])
def new_data():
    sensor_data = request.form["data"]
    return 'ok'
@app.route('/test', methods=['GET'])
def test():
    return 'contract_address: ' + str(etheriumComunication.contractAddress)


if __name__ == "__main__":  # There is an error on this line
    app.run(debug=True, host='0.0.0.0')
    print("test")