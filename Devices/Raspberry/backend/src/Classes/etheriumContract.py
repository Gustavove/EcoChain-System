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
        hex_values = hex_string.split(" ")  # Separar la cadena por los espacios
        byte_list = []
        for hex_val in hex_values:
            byte_list.append(int(hex_val, 16))
        return byte_list

    def decrypt_message(self, key, ciphertext):
        # Crear objeto AES y establecer la clave
        aes = AES.new(key, AES.MODE_ECB)

        print(len(ciphertext))

        # Dividir el ciphertext en bloques de 16 bytes y desencriptar cada bloque
        result = []
        for i in range(0, len(ciphertext), 16):
            block = ciphertext[i:i + 16]
            print(block)
            decrypted_block = aes.decrypt(bytes(block))
            # Convertir el bloque desencriptado en una lista de bytes y agregarlo a la lista `result`
            result.extend(decrypted_block)

            print('result' + str(i))
            print(result)
            print('result')


        # Convertir la lista `result` en un objeto bytes y devolverlo
        return bytes(result)

    def decrypt(self, message, key_):
        key = bytes([0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
             0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10])
        ciphertext = self.string_to_arraybytes("14 7D 7B 5A 3 36 84 A1 FB 4B 63 5F 80 A8 3F BC 89 71 B8 58 9 DC 2 44 29 7E E8 BF 31 A5 70 EC 89 93 A9 7 8E 58 6E 58 51 69 AC 17 B3 D8 49 ED 3C 9E 35 8E D2 C3 0 24 FD 2B 1C D1 EC 3 F5 57 8C 98 45 D4 F0 C5 FD 9 7 DF 1A 8C BC 52 7 CD F3 9D 77 31 40 B1 DB 2D 37 B8 F7 FD C1 3D E5 47 3C 91 BE 9B 93 F3 67 2F 43 56 78 B2 D 6F 87 C8 94 F3 40 87 BA 5D 94 6C 5A AB 88 F7 DE FC 7E D5 E1 24 A1 ED D BF 8B 53 CD 46 AB 8E 93 AC AE 95")

        result = self.decrypt_message(key, ciphertext)
        print(result)
        return 'ok'

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
