import React, { useEffect, useState } from 'react';
import {backendPOST} from "../controllers/backendComunication";
import Table from 'react-bootstrap/Table';
import Button from 'react-bootstrap/Button';
import OptionPanel from "./optionPanel";


function DevicesList(props) {
    const handlebutton = (sensor_id) => {
        backendPOST("http://10.0.2.15:5000/sensor/remove", {'id': sensor_id}).then( (info) => {
            console.log(info);
            props.onHandlebuttonUpdate();
        });
    };
    const handlebuttoninfo = (id) => {
        props.onHandlebuttonEspecifidInfo(id)
    }

    return (
        <>
        <Table striped bordered hover>
            <thead>
            <tr>
                <th>MAC</th>
                <th>Status</th>
                <th></th>
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
                    <td> <Button variant="primary" onClick={() => handlebuttoninfo(item.mac)}>More info</Button></td>
                    <td>
                        <Button
                            variant="info"
                            onClick={() => {
                                window.location.href = "http://localhost:5000/sensor/download/allinfo?id=" + item.mac;
                            }}
                            download
                        >
                            Download all data
                        </Button>
                    </td>
                    <td> <Button variant="danger" onClick={() => handlebutton(item.mac)}> Delete Sensor </Button></td>
                </tr>
                ))
            }
            </tbody>
        </Table>
            <OptionPanel onUpdate={props.onHandlebuttonUpdate}/>
        </>
    );
}

export default DevicesList;