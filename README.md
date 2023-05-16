# EcoChain System
EcoChain System is a system based on an IoT, IPFS and blockchain architecture for the management and storage of environmental measurement data in public and private environments.  The project is part of the TFG "Desarrollo de una aplicación IoT basada en IPFS y blockchain" of the Universitat Politécnica de Catalunya - FIB.

The following code is based on a prototype, and its usage should be used in a testing environment.

## Arquitecture

## Table of Contents

- [Technologies and frameworks](#technologies-and-frameworks)
- [Install](#install)
  - [LoRa devices](#lora-devices)
  - [Descentralized services (IPFS, Etherium)](#descentralized-services)
  - [Backend](#backend)
  - [Frontend](#frontend)
- [Getting started](#getting-started)
- [Known issues](#known-issues)
- [License](#license)

## Technologies and frameworks
* [Node.js](https://nodejs.org/): Version 18.9.0
* [npm.js](https://www.npmjs.com/): Version 8.19.2
* [Platformio for Visual Studio](https://platform.io/)
* [Kubo -IPFS](https://docs.ipfs.tech/install/command-line/) Version 0.17.0
* [React-boostrap](https://react-bootstrap.github.io/)
* [Truffle suite](https://trufflesuite.com/) and [Ganache](https://trufflesuite.com/ganache/)
* [Python](https://www.python.org/)

## Install

### LoRa devices

Before compiling the code modify:

* /Devices/Lora/LoraDevices/src/config.h
```cpp
//WIFI configurations
...
#define RUT_SSID "Your wifi ssid"
#define WIFIPWD "Your wifi password"
...
```
* /Devices/Lora/LoraDevices/src/HttpEth.cpp
```cpp
String HttpEth::sendData(String message) {
  ...
  http.begin("https://YOUR_DEVICE_BACKEND_IP/sensor/adddata");
  ...
}
```
Compile with Platform.io and upload to your device, the prototype joins both node and gateway in one device

Note: TTGO CH9102X QFN28 LORA32 868MHz ESP32 as recomended lora device

### Descentralized services

#### Blockchain

Download [Ganache](https://trufflesuite.com/ganache/) from truffle web and start a new workspace, take note of the following parameters:

* Gas price
* Gas limit 
* Network id
* RCP server

Choose an account and on the right side of the key symbol take note of ACCOUNT ADDRESS and PRIVATE KEY.

#### IPFS

Download Kubo in the following [link](https://dist.ipfs.tech/kubo/v0.17.0/kubo_v0.17.0_linux-amd64.tar.gz)

or 

```bash
wget https://dist.ipfs.tech/kubo/v0.17.0/kubo_v0.17.0_linux-amd64.tar.gz
```

Install kubo

```bash
./install.sh
```
Inicialize local node

```bash
ipfs init
```
Inicialize ipfs daemon

```bash
ipfs daemon
```

### Backend

Install nodejs and npm

```bash
sudo apt install nodejs
```
Install Truffle suite

```bash
npm install -g truffle
```

Install python and depended libraries
```bash
sudo apt-get install python3.6
pip install eth_abi:2.2.0
pip install eth_account==0.5.9
pip install Flask==2.2.3
pip install Flask_Cors==3.0.10
pip install ipfshttpclient==0.7.0
pip install pycryptodome==3.15.0
pip install Requests==2.29.0
pip install web3==5.31.1
```
Before initialising the backend modify:

* /Devices/Raspberry/backend/src/Modules/config.py
```Python
...
#DEFAULT INFO
URL = 'http://127.0.0.1:7545'  # Url of ganche node
CHAINID = 1337
GASLIMIT = 6721975 # Maximum gas consumption of the transaction
GAS_PRICE = 1 # Incentive for miners (leave at 1)
MAX_DATA_TO_SEND = 4 # Number of data accumulated before sending it to the backend (Currently only 4 supported)
...
```

* /Devices/Raspberry/backend/app.py
```Python
...
MY_ADDRESS = '0x21e517bf6De93b1D783fEB84ddE42F589d845CEB'  # Ehereum address
MY_PRIVATE_KEY = '8a30ed9c3bf9f8270a180f312fd3bda19a8ef5a9346f8d984b5405d864d9a98c' #Ethereum private key
PATH_SC_TRUFFLE = '/SmartContracts'  # Smart Contract path (modify with your own path)
...
```

Inicilize backend

```bash
python3.8 app.py
```

### Frontend

```bash
npm install
npm start
```

## Getting started

The contract is precompiled, if you want to modify and compile it:

* /SmartContracts
```bash
npm init -y
npm install --save @openzeppelin/contracts
truffle compile
```

Inicialize frontend to depploy the contract and later add a provider
```bash
curl -d 'provider_address=0x808704d7C39F1402E8Bd5547F6369e00dDE24cD4' -X POST http://127.0.0.1:5000/provider/new
```
Add the MAC of a sensor in the frontend to allow it to upload data to the system

## Known issues

* Permission denied: '/dev/ttyACM0'
```bash
sudo chmod a+rw /dev/ttyACM0
```

* If the boards do not flash click on PPG button, usually happens if they are broken.

* VersionMismatch: Unsupported daemon version '0.17.0' (not in range: 0.5.0 ≤ … < 0.15.0)

Modify the library in the local environment

```Python
...
VERSION_MINIMUM   = "0.5.0"
VERSION_BLACKLIST = []
VERSION_MAXIMUM   = "0.18.0"
...
```

## License

This code is under the terms of the [MIT license](https://opensource.org/licenses/MIT)
