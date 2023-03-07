import React, { useEffect, useState } from 'react';
import {backendPOST} from "../controllers/backendComunication";
import Table from 'react-bootstrap/Table';
import Button from 'react-bootstrap/Button';


function DevicesList(props) {
    const handlebutton = (sensor_id) => {
        backendPOST("http://10.0.2.15:5000/sensor/remove", {'id': sensor_id}).then( (info) => {
            console.log(info);
            props.onHandlebuttonUpdate();
        });
    };
    return (
        <Table striped bordered hover>
            <thead>
            <tr>
                <th>MAC</th>
                <th>Status</th>
                <th></th>
                <th></th>
            </tr>
            </thead>
            <tbody>
            {
                props.data && props.data.map((item) => (
                <tr key={item.mac}>
                    <td>{item.mac}</td>
                    <td>Active</td>
                    <td> <Button variant="info">More info</Button></td>
                    <td> <Button variant="danger" onClick={() => handlebutton(item.mac)}> Delete Sensor </Button></td>
                </tr>
                ))
            }
            </tbody>
        </Table>

    );
}

export default DevicesList;