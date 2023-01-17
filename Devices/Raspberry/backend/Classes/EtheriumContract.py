import requests
import json
import web3  # Release 4.0.0-beta.8class Transaction:
import time


def __init__(self, _url, _chainID, _gaslimit):
    # blockchain info
    self.url = _url
    self.chainID = _chainID
    self.gaslimit = _gaslimit
    self.requestID = 0
    self.gasPrice = 1

    # create persistent HTTP connection
    self.session = requests.Session()
    self.w3 = web3.Web3()


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


def getNonce(self, _myAdress):
    requestObject, self.requestID = self.createJSONRPCRequestObject('eth_getTransactionCount', [_myAdress, 'latest'],
                                                                    self.requestID)
    responseObject = self.postJSONRPCRequestObject(self.url, requestObject)
    myNonce = self.w3.toInt(hexstr=responseObject['result'])
    print('nonce of address {} is {}'.format(_myAdress, myNonce))
    return myNonce


def signTransaction(self, _transaction_dict, myPrivateKey):
    signed_transaction_dict = self.w3.eth.account.signTransaction(_transaction_dict, myPrivateKey)
    params = [signed_transaction_dict.rawTransaction.hex()]
    return params


def formatDataTransaction(self, _function, _values):
    data = self.w3.sha3(text=_function)[0:4].hex()  # methodId
    print('methodID of smartcontract function: ' + data)
    for value in _values:
        param = (value).to_bytes(32, byteorder='big').hex()
        data += param
    return data


def sendTransaction(self, _params):
    requestObject, self.requestID = self.createJSONRPCRequestObject('eth_sendRawTransaction', _params, self.requestID)
    responseObject = self.postJSONRPCRequestObject(self.url, requestObject)
    transactionHash = responseObject['result']
    print('transaction hash {}'.format(transactionHash))
    return transactionHash


def transactionMined(self, _transactionHash):
    while (True):
        requestObject, self.requestID = self.createJSONRPCRequestObject('eth_getTransactionReceipt', [_transactionHash],
                                                                   self.requestID)
        responseObject = self.postJSONRPCRequestObject(self.url, requestObject)
        receipt = responseObject['result']
        if (receipt is not None):
            break
        time.sleep(22 / 10)
    return receipt


def getValue(self, _function, _myAddress, _contractAddress):
    data = self.w3.sha3(text=_function)[0:4].hex()
    transaction_dict = {'from': _myAddress,
                        'to': _contractAddress,
                        'chainId': self.chainID,
                        'data': data}

    params = [transaction_dict, 'latest']
    requestObject, self.requestID = self.createJSONRPCRequestObject('eth_call', params, self.requestID)
    responseObject = self.postJSONRPCRequestObject(self.url, requestObject)
    print("responseObject: ", responseObject)
    result = self.w3.toInt(hexstr=responseObject[
        'result'])  # por ahora retorna como solo como Int, mirar más adelante para que retorne más general

    return result


def deployContract(self, _myAddress, _myPrivateKey, _path_truffle, _gas):  # posiblemente no necesite ser un metodo

    # compile your smart contract with truffle first
    truffleFile = json.load(open(_path_truffle))
    bytecode = truffleFile['bytecode']

    ### create your transaction
    transaction_dict = {'from': _myAddress,
                        'to': '',  # empty address for deploying a new contract
                        'chainId': self.chainID,
                        'gasPrice': self.gasPrice,
                        # careful with gas price, gas price below the --gasprice option of Geth CLI will cause problems. I am running my node with --gasprice '1'
                        'gas': _gas,  # rule of thumb / guess work
                        'nonce': self.getNonce(_myAddress),
                        'data': bytecode}  # no constrctor in my smart contract so bytecode is enough

    ### sign the transaction
    params = self.signTransaction(transaction_dict, _myPrivateKey)

    ### send the transacton to your node
    transactionHash = self.sendTransaction(params)
    print('contract submission hash {}'.format(transactionHash))

    ### wait for the transaction to be mined and get the address of the new contract
    receipt = self.transactionMined(transactionHash)
    if (receipt['status'] == '0x1'):
        contractAddress = receipt['contractAddress']
        print('newly deployed contract at address {}'.format(contractAddress))
    else:
        raise ValueError('transacation status is "0x0", failed to deploy contract. Check gas, gasPrice first')

    return contractAddress


def makeTransaction(self, _myAddress, _contractAddress, _myPrivateKey, _function, _values,
                    _gas):  # posiblemente no necesite ser un metodo
    if _contractAddress == '':
        raise Exception("Address couldn't be empty")
    else:
        _contractAddress = self.w3.toChecksumAddress(_contractAddress)
        data = self.formatDataTransaction(_function, _values)
        transaction_dict = {'from': _myAddress,
                            'to': _contractAddress,
                            'chainId': self.chainID,
                            'gasPrice': self.gasPrice,
                            # careful with gas price, gas price below the threshold defined in the node config will cause all sorts of issues (tx not bieng broadcasted for example)
                            'gas': _gas,  # rule of thumb / guess work
                            'nonce': self.getNonce(_myAddress),
                            'data': data}

        ### sign the transaction
        params = self.signTransaction(transaction_dict, _myPrivateKey)

        ### send the transacton to your node
        transactionHash = self.sendTransaction(params)

        ### wait for the transaction to be mined
        receipt = self.transactionMined(transactionHash)

        if (receipt['status'] == '0x1'):
            print('transaction successfully mined')
        else:
            raise ValueError('transacation status is "0x0", failed to deploy contract. Check gas, gasPrice first')
