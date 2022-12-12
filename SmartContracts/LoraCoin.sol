// SPDX-License-Identifier: GPL-3.0

pragma solidity ^0.8.9;

import "@openzeppelin/contracts/token/ERC20/ERC20.sol";

contract LoraCoin is ERC20 {
    /*
     *   Address of the SC deployer wich will be the only one
     *   that will be able create gateways and verified nodes
     */
    address public root;

    uint256 public constant ethNeededToRegisterNode = 1; //Only used in the future
    uint256 public constant registerReward = 1;
    uint256 public constant sentReward = 10;
    uint256 public constant tokenAmountCreate = 10000;

    struct NetworkDevice {
        address owner;
        uint8 v;
        bytes32 r;
        bytes32 s;
        bytes32[] transactions;
        bool created;
    }
    
    //Used to store all registered transactions to validate if they are already registered
    mapping(bytes32 => bool) transactionsValidation;

    //Stores a map of the network devices using their addresses as keys
    mapping(address => NetworkDevice) public devices;
    mapping(address => address[]) public devicesByOwners;
    address[] deviceAddresses;

    constructor() ERC20("LoraCoin", "LoRaC") {
        root = msg.sender;
        uint256 n = 1000;
        _mint(msg.sender, n * 10**uint256(decimals()));
    }

    function getOwner() public view returns (address) {
        return root;
    }

    function hello(address _address) public view returns (bool) {
        require(
            _address != root,
            "Address must be different of the root address"
        );
        return true;
    }

    function mint() public returns (bool) {
        _mint(msg.sender, 10000000000);
        return true;
    }

    // @notice creates a new device
    // @pre msg.sender = root
    // @param _address is the address of the device to be registered
    // @param _v,_r,_s parameter _address (lowercase) ECDSA signed by the msg.sender
    // @return device created or not
    function createDevice(
        address _address,
        uint8 _v,
        bytes32 _r,
        bytes32 _s
    ) public returns (bool) {
        require(msg.sender == root, "Only system root can create devices");
        require(
            _address != root,
            "Address must be different of the root address"
        );
        require(
            !devices[_address].created,
            "Device already added to the network"
        );
        // require(
        //     recoverSigner(addressToString(_address), _v, _r, _s) == root,
        //     "Bad signature."
        // );
        devices[_address] = NetworkDevice(
            address(0x0),
            _v,
            _r,
            _s,
            new bytes32[](0),
            true
        );
        deviceAddresses.push(_address);
        return true;
    }

    // // @notice creates a new device
    // // @pre msg.sender = root
    // // @param message is the message
    // // @param sig parameter _address (lowercase) ECDSA signed by the msg.sender
    // // @return device created or not
    // function createDevice(bytes32 message, bytes memory sig)
    //     public
    //     returns (bool)
    // {
    //     require(msg.sender == root, "Only system root can create devices");

    //     address _address = dataToAddress(message);

    //     require(
    //         _address != root,
    //         "Address must be different of the root address"
    //     );
    //     require(
    //         !devices[_address].created,
    //         "Device already added to the network"
    //     );

    //     uint8 v;
    //     bytes32 r;
    //     bytes32 s;

    //     (v, r, s) = splitSignature(sig);

    //     require(ecrecover(message, v, r, s) == root, "Bad signature.");
    //     devices[_address] = NetworkDevice(
    //         address(0x0),
    //         v,
    //         r,
    //         s,
    //         new bytes32[](0),
    //         true
    //     );
    //     deviceAddresses.push(_address);
    //     return true;
    // }

    // @notice returns a list with all network devices
    // @return list with all deviceAddresses
    function getDevices() public view returns (address[] memory) {
        return deviceAddresses;
    }

    // @notice used to send data to the BC and pays to the owner of the sensor
    // @param sensor location and sensoring info
    // @return whether the creation was successful or not
    function sendTransaction(bytes memory data) public returns (bool) {
        require(!devices[msg.sender].created, "Device not created yet"); // gateway already registerd
        require(
            devices[msg.sender].owner != address(0x0),
            "Device don't has an owner"
        ); // gateway already registerd
        require(data.length > 0, "No data sent");
        _mint(devices[msg.sender].owner, registerReward);
        return true;
    }

    // @notice used to save transaction and paying to the gateway
    // @param transaction hash
    // @return whether the creation was successful or not
    function registerTransaction(
        address _sensorAddress,
        bytes32 transactionHash
    ) public returns (bool) {
        require(
            !transactionsValidation[transactionHash],
            "Transaction already registered"
        );
        require(!devices[msg.sender].created, "Gateway not created yet"); // gateway already registerd
        require(!devices[_sensorAddress].created, "Node not created yet"); // gateway already registerd
        require(
            devices[msg.sender].owner != address(0x0),
            "Gateray don't has an owner"
        ); // gateway already registerd
        require(
            devices[_sensorAddress].owner != address(0x0),
            "Node don't has an owner"
        ); // gateway already registerd
        devices[_sensorAddress].transactions.push(transactionHash);
        transactionsValidation[transactionHash] = true;
        _mint(devices[msg.sender].owner, sentReward);
        return true;
    }

    function uint2hexstr(uint256 i) private pure returns (string memory) {
        if (i == 0) return "00";
        bool short;
        if (i < 16) short = true;
        uint256 j = i;
        uint256 length;
        while (j != 0) {
            length++;
            j = j >> 4;
        }
        uint256 mask = 15;
        bytes memory bstr = new bytes(length);
        uint256 k = length;
        while (i != 0) {
            uint256 curr = (i & mask);
            bstr[--k] = curr > 9
                ? bytes1(uint8(87 + curr))
                : bytes1(uint8(48 + curr)); // 55 = 65 - 10
            i = i >> 4;
        }
        if (short) return concatenate("0", string(bstr));
        return string(bstr);
    }

    function concatenate(string memory a, string memory b)
        private
        pure
        returns (string memory)
    {
        return string(abi.encodePacked(a, b));
    }

    function addressToString(address _address)
        private
        pure
        returns (string memory)
    {
        bytes memory z = abi.encodePacked(_address);
        string memory n = "0x";
        for (uint8 i = 0; i < 20; i++) {
            n = concatenate(n, uint2hexstr(uint8(z[i])));
        }
        return string(n);
    }

    // @notice registers the owner of a device
    // @param _address is the address of the device to be registered
    // @param signature value of the hashed owner address signed by the own dev
    // @return whether the creation was successful or not
    function registerDevice(
        address _address,
        uint8 v,
        bytes32 r,
        bytes32 s
    ) public returns (bool) {
        require(devices[_address].created, "Device not created yet"); // gateway already registerd
        require(
            devices[_address].owner == address(0x0),
            "Device already has an owner"
        ); // gateway already registerd
        require(
            (_address == recoverSigner(addressToString(msg.sender), v, r, s)),
            "Error recovering signature"
        );
        devices[_address].owner = msg.sender;
        _mint(msg.sender, tokenAmountCreate);
        return true;
    }

    /*
     * @dev Recover signer address from a message by using their signature
     * @param hash bytes32 message, the hash is the signed message. What is recovered is the signer address.
     * @param sig bytes signature, the signature is generated using web3.eth.sign()
     */
    function recoverSigner(
        string memory message,
        uint8 v,
        bytes32 r,
        bytes32 s
    ) private pure returns (address signer) {
        // The message header; we will fill in the length next
        string memory header = "\x19Ethereum Signed Message:\n000000";
        uint256 lengthOffset;
        uint256 length;
        assembly {
            // The first word of a string is its length
            length := mload(message)
            // The beginning of the base-10 message length in the prefix
            lengthOffset := add(header, 57)
        }
        require(length <= 999999);
        uint256 lengthLength = 0;
        uint256 divisor = 100000;
        while (divisor != 0) {
            // The place value at the divisor
            uint256 digit = length / divisor;
            if (digit == 0) {
                // Skip leading zeros
                if (lengthLength == 0) {
                    divisor /= 10;
                    continue;
                }
            }
            lengthLength++;
            length -= digit * divisor;
            divisor /= 10;
            digit += 0x30;
            lengthOffset++;
            assembly {
                mstore8(lengthOffset, digit)
            }
        }
        // The null string requires exactly 1 zero (unskip 1 leading 0)
        if (lengthLength == 0) {
            lengthLength = 1 + 0x19 + 1;
        } else {
            lengthLength += 1 + 0x19;
        }
        // Truncate the tailing zeros from the header
        assembly {
            mstore(header, lengthLength)
        }
        // Perform the elliptic curve recover operation
        bytes32 check = keccak256(abi.encodePacked(header, message));
        return ecrecover(check, v, r, s);
    }

    // function recoverSigner(bytes32 message, bytes memory sig)
    //     internal
    //     pure
    //     returns (address)
    // {
    //     uint8 v;
    //     bytes32 r;
    //     bytes32 s;

    //     (v, r, s) = splitSignature(sig);

    //     return ecrecover(message, v, r, s);
    // }

    // function splitSignature(bytes memory sig)
    //     internal
    //     pure
    //     returns (
    //         uint8,
    //         bytes32,
    //         bytes32
    //     )
    // {
    //     require(sig.length == 65);

    //     bytes32 r;
    //     bytes32 s;
    //     uint8 v;

    //     assembly {
    //         // first 32 bytes, after the length prefix
    //         r := mload(add(sig, 32))
    //         // second 32 bytes
    //         s := mload(add(sig, 64))
    //         // final byte (first byte of the next 32 bytes)
    //         v := byte(0, mload(add(sig, 96)))
    //     }

    //     return (v, r, s);
    // }

    function dataToAddress(bytes32 data) internal pure returns (address) {
        return address(uint160(uint256(data)));
    }
}

