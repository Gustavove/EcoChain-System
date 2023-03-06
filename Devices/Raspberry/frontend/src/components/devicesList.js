import React, { useEffect, useState } from 'react';
import {backendGET} from "../controllers/backendComunication";
import Table from 'react-bootstrap/Table';


function DevicesList(props) {
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
                    <td></td>
                    <td></td>
                </tr>
                ))
            }
            </tbody>
        </Table>
    );
}

export default DevicesList;