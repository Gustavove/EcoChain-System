// SPDX-License-Identifier: GPL-3.0

pragma solidity 0.8.9;

contract StorageContract{ 

    address payable owner;

    struct SensorData{
        uint256 tx_index; //indice de la transacción
        bytes32 cid; //String
        bytes32 url; //String
        bytes32 timestamp; //String
        bytes signature;
        bool created;
    }

    struct SensorDevice {
        bytes public_key; //identificador del sensor
        mapping(bytes32 => SensorData) Data;
        bytes32[] SensorDataList;
        bool created;
    }

    mapping(bytes32 => SensorDevice) public Devices;
    bytes[] public SensorDataList;

    constructor() public {
        
    }

    function newData (
        uint256  _tx_index,
        bytes32 _cid,
        bytes32 _url,
        bytes32 _timestamp,
        bytes memory _signature,
        bytes memory _public_key
    )
    public {

        //Data[_cid].tx_index = _tx_index;
        //Data[_cid].cid = _cid;
        //Data[_cid].url = _url;
        //Data[_cid].timestamp = _timestamp;
        //Data[_cid].signature = _signature;
        //Data[_cid].public_key = _public_key;
        //SensorDataList.push(_cid);
    }
}
