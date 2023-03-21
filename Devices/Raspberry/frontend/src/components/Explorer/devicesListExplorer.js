import React, { useEffect, useState } from 'react';
import Table from 'react-bootstrap/Table';
import Button from 'react-bootstrap/Button';
import OptionPanel from "../Home/optionPanel";


function DevicesListExplorer(props) {
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
                                window.location.href = "http://localhost:5000/sensor/download/allinfo?" + "provider_address=" + props.providerAddress + "&id=" + item.mac;
                            }}
                            download
                        >
                            Download all data
                        </Button>
                    </td>
                </tr>
                ))
            }
            </tbody>
        </Table>
        </>
    );
}

export default DevicesListExplorer;