import requests
import json
import web3  # Release 4.0.0-beta.8class Transaction:
import time
from eth_account.messages import encode_defunct
from eth_abi import encode_abi, decode_abi
import pprint

from Crypto.Cipher import AES
import base64


pp = pprint.PrettyPrinter(indent=2)

class EtheriumContract:
    def __init__(self, _url, _chainID, _gasprice, _gaslimit, _contract_address):
        # blockchain info
        self.url = _url
        self.chainID = _chainID
        self.gaslimit = _gaslimit
        self.requestID = 0
        self.gasPrice = _gasprice
        self.contractAddress = _contract_address

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
        requestObject, self.requestID = self.createJSONRPCRequestObject('eth_getTransactionCount', [_myAdress, 'latest'], self.requestID)
        responseObject = self.postJSONRPCRequestObject(self.url, requestObject)
        myNonce = self.w3.toInt(hexstr=responseObject['result'])
        print('nonce of address {} is {}'.format(_myAdress, myNonce))
        return myNonce

    def signTransaction(self, _transaction_dict, myPrivateKey):
        signed_transaction_dict = self.w3.eth.account.signTransaction(_transaction_dict, myPrivateKey)
        params = [signed_transaction_dict.rawTransaction.hex()]
        return params

    def signData(self, data, myPrivateKey):
        # prepared_message = defunct_hash_message(primitive=data)
        # signature = self.w3.eth.account.signHash(prepared_message, '0x' + privateKeyUser)

        # This part prepares "version E" messages, using the EIP-191 standard
        message = encode_defunct(text=data)
        # This part signs any EIP-191-valid message
        signed_message = self.w3.eth.account.sign_message(message, myPrivateKey)
        return signed_message

    def encodeABI(self, _function, _values):
        methodID = self.w3.sha3(text=_function)[0:4].hex()  # methodId
        if _values:
            # obtenemos el formato de los valores
            parts = _function.split("(")[1]
            input_parameters = parts[:-1]  # remove the last ")"
            input_parameters = input_parameters.split(",")
            # identificamos las direcciones mal hechas
            cont = 0
            for input in input_parameters:
                if input == 'address':
                    _values[cont] = self.w3.toChecksumAddress(_values[cont])
                cont += 1
            encoded_data = encode_abi(input_parameters, _values).hex()
            codification = methodID + encoded_data
        else:
            codification = methodID

        return codification

    def sendTransaction(self, _params):  # try-catch para responseObject - Si hay lagun error no encuentra 'result'
        requestObject, self.requestID = self.createJSONRPCRequestObject('eth_sendRawTransaction', _params, self.requestID)
        responseObject = self.postJSONRPCRequestObject(self.url, requestObject)

        if 'error' in responseObject:
            raise Exception(responseObject['error']['message'])

        transactionHash = responseObject['result']
        print('transaction hash {}'.format(transactionHash))
        return transactionHash

    def transactionMined(self, _transactionHash):
        while (True):
            requestObject, self.requestID = self.createJSONRPCRequestObject('eth_getTransactionReceipt', [_transactionHash], self.requestID)
            responseObject = self.postJSONRPCRequestObject(self.url, requestObject)
            receipt = responseObject['result']
            if (receipt is not None):
                print(receipt)
                break
            time.sleep(22 / 10)
        return receipt

    def getValue(self, _function, _valors, _returned_types, _myAddress):
        if self.contractAddress == 0x0:
            raise Exception("Deploy a contract first")
        else:
            data = self.encodeABI(_function, _valors)
            transaction_dict = {'from': _myAddress,
                                'to': self.contractAddress,
                                'chainId': self.chainID,
                                'data': data}

        params = [transaction_dict, 'latest']
        requestObject, self.requestID = self.createJSONRPCRequestObject('eth_call', params, self.requestID)
        responseObject = self.postJSONRPCRequestObject(self.url, requestObject)

        if 'error' in responseObject:
            raise Exception(responseObject['error']['message'])

        returned_values = bytes.fromhex(responseObject['result'][2:]) #IMPORTANTE SUBDIVIDIRLO EN 2 QUE SINO PETA POR SER DEMASIADO GRANDE!!!!!!!
        decoded_data = decode_abi(_returned_types, returned_values)

        return decoded_data

    def string_to_arraybytes(self, hex_string):
        hex_string = hex_string.strip()
        hex_values = hex_string.split(" ")  # Separar la cadena por los espacios
        byte_list = []
        for hex_val in hex_values:
            byte_list.append(int(hex_val, 16))
        return byte_list

    def decrypt_message(self, key, ciphertext):
        # Crear objeto AES y establecer la clave
        aes = AES.new(key, AES.MODE_ECB)

        # Dividir el ciphertext en bloques de 16 bytes y desencriptar cada bloque
        result = []
        for i in range(0, len(ciphertext), 16):
            block = ciphertext[i:i + 16]
            decrypted_block = aes.decrypt(bytes(block))
            # Convertir el bloque desencriptado en una lista de bytes y agregarlo a la lista `result`
            result.extend(decrypted_block)

        # Convertir la lista `result` en un objeto bytes y devolverlo
        return bytes(result)

    def decrypt(self, message, key):
        ciphertext = self.string_to_arraybytes(message)
        result = self.decrypt_message(key, ciphertext)

        decoded_str = result.decode("utf-8")  # Decodificar el byte a string
        json_data = decoded_str.rstrip('0')

        return json_data

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
        self.contractAddress = self.w3.toChecksumAddress(contractAddress)

        return self.contractAddress

    def makeTransaction(self, _myAddress, _myPrivateKey, _function, _valors, _gas):  # posiblemente no necesite ser un metodo
        if self.contractAddress == 0x0:
            raise Exception("Deploy a contract first")
        else:
            data = self.encodeABI(_function, _valors)
            transaction_dict = {'from': _myAddress,
                                'to': self.contractAddress,
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
                raise ValueError('transaction status is "0x0", failed to deploy contract. Check gas, gasPrice, parameters first')
