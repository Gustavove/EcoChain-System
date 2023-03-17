// SPDX-License-Identifier: GPL-3.0

pragma solidity ^0.8.9;

contract StorageContract {
    address payable owner;

    struct SensorData {
        string cid; //Identificador
        string url;
        uint256 timestamp;
        uint8 v;
        bytes32 r;
        bytes32 s;
        bytes32 hash;
        bool created;
    }

    struct Sensor {
        string mac; //Identificador
        mapping(string => SensorData) Data;
        string[] SensorDataList;
        bool created;
    }

    struct DataProvider {
        address provider_address; //Identificador
        mapping(string => Sensor) SensorInfo;
        string[] SensorInfoList;
        bool created;
    }

    mapping(address => DataProvider) public Devices;
    address[] public DataProviderList;

    event EventProviderAdded(address indexed addressProvider, uint256 date);

    event EventSensorAdded(
        address indexed addressProvider,
        string mac,
        uint256 date
    );

    event EventDataAdded(
        string indexed cid,
        address indexed _provider_address,
        string mac,
        string url
    );

    constructor() {
        owner = payable(msg.sender);
    }

    function newProvider(address _provider_address) public {
        require( payable(msg.sender) == owner, "Only contract's owner can add a provider");
        require( Devices[_provider_address].created == false, "Provider has already added");

        Devices[_provider_address].provider_address = _provider_address;
        Devices[_provider_address].created = true;
        DataProviderList.push(_provider_address);

        emit EventProviderAdded(_provider_address, block.timestamp);
    }

    /*
     * @dev Add new sensor using provider signature
     * @param v, r, s and hash make up signature using the EIP-191 standard.
        The signature is generated using encode_defunct and eth.account.sign_message in python
     */
    function newSensor(
        string memory _mac,
        uint8 _v,
        bytes32 _r,
        bytes32 _s,
        bytes32 _hash
    ) public {
        address recoveredAddress = ecrecover(_hash, _v, _r, _s);
        require(Devices[recoveredAddress].created == true, "Invalid signature, provider doesn't exists");
        require(Devices[recoveredAddress].SensorInfo[_mac].created == false, "Sensor has already added");

        Devices[recoveredAddress].SensorInfo[_mac].mac = _mac;
        Devices[recoveredAddress].SensorInfoList.push(_mac);

        emit EventSensorAdded(recoveredAddress, _mac, block.timestamp);
    }

    /*
     * @dev Upload new data using its signature
     * @param v, r, s and hash make up signature using the EIP-191 standard.
        The signature is generated using encode_defunct and eth.account.sign_message in python
     */
    function newData(
        string memory _mac,
        string memory _cid,
        string memory _url,
        uint256 _timestamp,
        uint8 _v,
        bytes32 _r,
        bytes32 _s,
        bytes32 _hash
    ) public {
        address recoveredAddress = ecrecover(_hash, _v, _r, _s);
        require(Devices[recoveredAddress].created == true, "Invalid signature, provider doesn't exists");
        require(recoveredAddress == msg.sender, "Only provider can upload new data");
        require(Devices[recoveredAddress].SensorInfo[_mac].created == true, "Sensor doesn't exists");

        Devices[recoveredAddress].SensorInfo[_mac].Data[_cid].cid = _cid;
        Devices[recoveredAddress].SensorInfo[_mac].Data[_cid].url = _url;
        Devices[recoveredAddress].SensorInfo[_mac].Data[_cid].timestamp = _timestamp;
        Devices[recoveredAddress].SensorInfo[_mac].Data[_cid].v = _v;
        Devices[recoveredAddress].SensorInfo[_mac].Data[_cid].r = _r;
        Devices[recoveredAddress].SensorInfo[_mac].Data[_cid].s = _s;
        Devices[recoveredAddress].SensorInfo[_mac].Data[_cid].created = true;

        Devices[recoveredAddress].SensorInfo[_mac].SensorDataList.push(_cid);

        emit EventDataAdded(_cid, recoveredAddress, _mac, _url);
    }

    function getAllProviders() public view returns (address[] memory) {
        return DataProviderList;
    }

    function getMacList(address _provider_address) public view returns (string[] memory) {
        require(Devices[_provider_address].created == true, "Provider doesn't exists");
        return Devices[_provider_address].SensorInfoList;
    }

    function getCidList(address _provider_address, string memory _mac) public view returns (string[] memory) {
        require(Devices[_provider_address].created == true, "Invalid provider address");
        require(Devices[_provider_address].SensorInfo[_mac].created == true, "Invalid sensor MAC");
        
        return Devices[_provider_address].SensorInfo[_mac].SensorDataList;
    }

    function getLastCID(address _provider_address, string memory _mac) public view returns (string memory) {
        require(Devices[_provider_address].created == true, "Invalid provider address");
        require(Devices[_provider_address].SensorInfo[_mac].created == true, "Invalid sensor MAC");

        uint256 len = Devices[_provider_address].SensorInfo[_mac].SensorDataList.length;
        require(len > 0, "No data available for this sensor");

        return Devices[_provider_address].SensorInfo[_mac].SensorDataList[len - 1];
    }
}
