// SPDX-License-Identifier: GPL-3.0

pragma solidity ^0.8.9;

contract StorageContract{ 

    address payable owner;

    struct SensorData{
        string cid; //Identificador
        string url; 
        uint256 timestamp; 
        uint8 v;
        bytes32 r;
        bytes32 s;
        bytes32 hash;
        bool created;
    }

    struct DataProvider {
        address provider_address; //Identificador
        mapping(string => SensorData) Data;
        string[] SensorDataList;
        bool created;
    }

    mapping(address => DataProvider) public Devices;
    address[] public DataProviderList;

    event EventProviderAdded (
        address indexed addressProvider,
        uint date
        );
    
    event EventDataAdded (
        string indexed cid,
        address indexed _provider_address,
        string url
    );

    constructor() {
        owner = payable(msg.sender);
    }

    function newProvider (
        address _provider_address
    )

    public {
        require(payable(msg.sender) == owner, "Only contract's owner can add a provider");
        require(Devices[_provider_address].created == false, "Provider has already added");

        Devices[_provider_address].provider_address = _provider_address;
        Devices[_provider_address].created = true;
        DataProviderList.push(_provider_address);
        
        emit EventProviderAdded(_provider_address, block.timestamp);

    }

    /*
     * @dev Upload new data using its signature
     * @param v, r, s and hash make up signature using the EIP-191 standard.
        The signature is generated using encode_defunct and eth.account.sign_message in python
     */
    function newData (
        string memory _cid,
        string memory _url,
        uint256 _timestamp,
        uint8 _v,
        bytes32 _r,
        bytes32 _s,
        bytes32 _hash
    )
    public {
        address recoveredAddress = ecrecover(_hash, _v, _r, _s);
        require(Devices[recoveredAddress].created == true, "Invalid signature, provider doesn't exists");
        require(recoveredAddress == msg.sender, "Only provider can upload new data");


        Devices[recoveredAddress].Data[_cid].cid = _cid;
        Devices[recoveredAddress].Data[_cid].url = _url;
        Devices[recoveredAddress].Data[_cid].timestamp= _timestamp;
        Devices[recoveredAddress].Data[_cid].v = _v;
        Devices[recoveredAddress].Data[_cid].r = _r;
        Devices[recoveredAddress].Data[_cid].s = _s;
        Devices[recoveredAddress].Data[_cid].created = true;

        Devices[recoveredAddress].SensorDataList.push(_cid);

        emit EventDataAdded(_cid, recoveredAddress, _url);
    }

    // @return returns a list with all cids of one provider
    function getDataProviderCids(address _provider_address) public view returns (string[] memory) {
        require(Devices[_provider_address].created == true, "Provider doesn't exists");
        return Devices[_provider_address].SensorDataList;
    }

    // @return returns a list with all addreess of providers
    function getAllDataProviders() public view returns (address[] memory) {
        return DataProviderList;
    }

    // @return returns information of one SensorData
    function getSensorData(string memory _cid, address _provider_address) public view returns (string memory, string memory, uint256, uint8, bytes32, bytes32, bytes32) {
        require(Devices[_provider_address].created == true, "Provider doesn't exists");
        require(Devices[_provider_address].Data[_cid].created  == true, "SensorData not found");
        SensorData memory sensor_data = Devices[_provider_address].Data[_cid];

        return (sensor_data.cid, sensor_data.url, sensor_data.timestamp, sensor_data.v, sensor_data.r, sensor_data.s, sensor_data.hash);
    }

}
