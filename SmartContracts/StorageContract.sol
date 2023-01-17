// SPDX-License-Identifier: GPL-3.0

pragma solidity 0.8.9;

contract StorageContract{ 

    address payable owner;

    struct SensorData{
        bytes32 cid; //String identificador
        bytes32 url; //String
        bytes32 timestamp; //String
        bytes signature;
        bool created;
    }

    struct DataProvider {
        address provider_address; //identificador
        mapping(bytes32 => SensorData) Data;
        bytes32[] SensorDataList;
        bool created;
    }

    mapping(address => DataProvider) public Devices;
    address[] public DataProviderList;

    constructor() {
        owner = payable(msg.sender);
    }

    function newProvider (
        address _provider_address
    )
    public {
        require(payable(msg.sender) == owner, "Only contract's owner can add a sensor");
        require(Devices[_provider_address].created == true, "Sensor has already added");

        Devices[_provider_address].provider_address = _provider_address;
        Devices[_provider_address].created = true;
        DataProviderList.push(_provider_address);
    }

    function newData (
        bytes32 _cid,
        bytes32 _url,
        bytes32 _timestamp,
        bytes memory _signature,
        address _provider_address
    )
    public {
        require(payable(msg.sender) == owner, "Only contract's owner can add data");
        require(!Devices[_provider_address].created, "A sensor was already registred with the same public key");

        Devices[_provider_address].Data[_cid].cid = _cid;
        Devices[_provider_address].Data[_cid].url = _url;
        Devices[_provider_address].Data[_cid].timestamp= _timestamp;
        Devices[_provider_address].Data[_cid].signature = _signature;
        Devices[_provider_address].Data[_cid].created = true;

        Devices[_provider_address].SensorDataList.push(_cid);
    }

    /*
     *@dev verifies a signature from public key, also verifies if the sensor exists in the whitelist
    
    function verifyDataSignature(bytes memory _public_key, bytes32 _cid, bytes32 _url, bytes32 _timestamp, bytes memory _signature) private view returns (bool) {

        require(Devices[_public_key].created true, "No sensor found with this public key");

        // Verificar la firma de los datos del sensor utilizando la clave pública del dispositivo del sensor
        // La firma se ha hecho a partir de w3.eth.account.signTransaction()
        bytes memory checker = abi.encode(_cid, _url, _timestamp);
        return bytes(ecrecover(
                                keccak256(abi.encodePacked(checker, _signature)),  // hash
                                _signature[0:32],  // v
                                _signature[32:64], // r
                                _signature[64:96]  // s
                                )) == _public_key;
    }
    */

    
}
