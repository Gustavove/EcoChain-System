import React, { useEffect, useState } from 'react';
import {backendPOST} from "../../controllers/backendComunication";
import Table from 'react-bootstrap/Table';
import Button from 'react-bootstrap/Button';

function ProviderListExplorer(props) {
    const handlebuttoninfo = (id) => {
        props.onHandlebuttonEspecifidInfo(id)
    }

    return (
        <>
        <Table striped bordered hover>
            <thead>
            <tr>
                <th>Address</th>
                <th></th>
            </tr>
            </thead>
            <tbody>
            {
                props.devices && props.devices.providers_address.map((item) => (
                <tr key={item}>
                    <td>{item}</td>
                    <td>Active</td>
                    <td> <Button variant="primary" onClick={() => handlebuttoninfo(item)}>More info</Button></td>
                </tr>
                ))
            }
            </tbody>
        </Table>
        </>
    );
}

export default ProviderListExplorer;