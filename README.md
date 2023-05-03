# EcoChain System
EcoChain System is a system based on an IoT, IPFS and blockchain architecture for the management and storage of environmental measurement data in public and private environments.  The project is part of the TFG "Desarrollo de un sistema de almacenamiento descentralizado en una aplicación IoT" of the Universitat Politécnica de Catalunya - FIB.

The following code is based on a prototype, and its usage should be used in a testing environment.

## Arquitecture

## Table of Contents

- [Technologies and frameworks](#technologies-and-frameworks)
- [Install](#install)
  - [LoRa devices](#lora-devices)
  - [Descentralized services (IPFS, Etherium)](#descentralized-services)
  - [Backend](#backend)
  - [Frontend](#frontend)
- Tests
  - [Getting started](#getting-started)
  - [Device managing](#device-managing)
- [Known issues](#known-issues)

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



Before compiling the code modify 

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
