import requests
import json
import web3  # Release 4.0.0-beta.8class Transaction:
import pprint
import time

    def __init__(self, _url, _path_truffle, _chainID, _gaslimit):
        # blockchain info
        self.url = _url
        self.path_sc_truffle = _path_truffle
        self.chainID = chainID
        self.gaslimit = _gaslimit

        # create persistent HTTP connection
        self.session = requests.Session()
        self.w3 = web3.Web3()
        self.pp = pprint.PrettyPrinter(indent=2)


    def contractTobBytecodeAbi(self, _path):
        truffleFile = json.load(open(PATH_SC_TRUFFLE + _path))
        abi = truffleFile['abi']
        bytecode = truffleFile['bytecode']
        return (abi,bytecode)

    def createJSONRPCRequestObject(self, _method, _params, _requestId):
         return {"jsonrpc": "2.0",
                 "method": _method,
                 "params": _params,  # must be an array [value1, value2, ..., valueN]
                  "id": _requestId}, _requestId + 1
    def postJSONRPCRequestObject(self, _HTTPEnpoint, _jsonRPCRequestObject):
        response = self.session.post(_HTTPEnpoint,
                                json=_jsonRPCRequestObject,
                                headers={'Content-type': 'application/json'})
        return response.json()

    def getNonce(self, _myAdress, _requestId):
        requestObject, requestId = createJSONRPCRequestObject('eth_getTransactionCount', [_myAdress, 'latest'], _requestId)
        responseObject = postJSONRPCRequestObject(self.url, requestObject)
        myNonce = w3.toInt(hexstr=responseObject['result'])
        print('nonce of address {} is {}'.format(_myAdress, myNonce))
        return myNonce

    def makeTransacction(self, _myAddress, isContract = false, _receiverAddress):
        if isContract:
            receiverAddress = ''
        else:
            receiverAddress = _receiverAddress
        transaction_dict = {'from': myAddress,
                    'to': receiverAddress # empty address for deploying a new contract
                    'chainId': CHAINID,
                    'gasPrice': 1,
                    # careful with gas price, gas price below the --gasprice option of Geth CLI will cause problems. I am running my node with --gasprice '1'
                    'gas': 2000000,  # rule of thumb / guess work
                    'nonce': myNonce,
                    'data': bytecode}  # no constrctor in my smart contract so bytecode is enough