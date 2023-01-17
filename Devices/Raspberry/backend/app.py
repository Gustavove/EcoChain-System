import os

from flask import Flask, request, jsonify

app = Flask(__name__)

#tenporal variables until wallet class will be created
myAddress = '0x808704d7C39F1402E8Bd5547F6369e00dDE24cD4'  # address funded in genesis file
myPrivateKey = 'ee8c2cf08d19374beb3aa9cda9fa53afe268dd91b5b15ae5f5645a2026f1267d'

# compile your smart contract with truffle first
truffleFile = json.load(open(PATH_SC_TRUFFLE + '/build/contracts/AdditionContract.json'))
abi = truffleFile['abi']
bytecode = truffleFile['bytecode']

@app.route('/init', methods=['GET'])
def init():
    return "Backend information would be send to inicialize the conexión with sensor/node"

@app.route('/new-node', methods=['POST'])
def new_node():
    public_key = request.form["public_key"]

@app.route('/new-data', methods=['POST'])
def new_node():
    sensor_data = request.form["data"]


