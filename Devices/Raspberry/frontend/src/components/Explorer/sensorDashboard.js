import React, {useEffect, useState} from "react";
import MapDevices from "../Maps/mapDevices";
import Slidebar from "../slidebar";
import Table from "react-bootstrap/Table";

function SensorDashboard(props) {
    const [value, setValue] = useState(1);

    function onHandleValue(value) {
        setValue(value);
    }

    return (
        <div>
            <h1>{props.data.data[value-1][value].mac}</h1>
            <div>
                <Slidebar valor={props.data.max_value} onHandleValue={onHandleValue}/>
            </div>
            <div className="abs-center">
                <div>
                    <MapDevices data={props.data.data[value-1][value]}/>
                </div>
                <div>
                    <Table striped bordered hover>
                        <thead>
                        <tr>
                            <th>PH</th>
                            <th>TDS</th>
                        </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>{props.data.data[value-1][value].message.pH}</td>
                                <td>{props.data.data[value-1][value].message.tds}</td>
                            </tr>
                        </tbody>
                    </Table>
                </div>
            </div>
        </div>
        )
}

export default SensorDashboard