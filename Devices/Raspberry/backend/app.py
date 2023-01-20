from src.Classes.etheriumContract import EtheriumContract
from flask import Flask, request

app = Flask(__name__)

#tenporal variables until wallet class will be created
myAddress = '0x21e517bf6De93b1D783fEB84ddE42F589d845CEB'  # address funded in genesis file
myPrivateKey = '8a30ed9c3bf9f8270a180f312fd3bda19a8ef5a9346f8d984b5405d864d9a98c'

URL = 'http://127.0.0.1:7545'  # url of my ganche node
PATH_SC_TRUFFLE = '/home/gustavo/Documentos/Universidad/TFG/Project/SmartContracts'  # smart contract path

@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"

@app.route('/init', methods=['GET'])
def init():
    return "Backend information would be send to inicialize the conexión with sensor/node"

@app.route('/new-node', methods=['GET'])
def new_provider():
    etherium_transaction = EtheriumContract('http://127.0.0.1:7545', 5777, 50000000)
    etherium_transaction.deployContract(myAddress, myPrivateKey, PATH_SC_TRUFFLE + '/build/contracts/StorageContract.json', 2000000)

    return 'ok'

@app.route('/new-data', methods=['POST'])
def new_node():
    sensor_data = request.form["data"]

if __name__ == "__main__":  # There is an error on this line
    app.run(debug=True, host='0.0.0.0')
    print("test")