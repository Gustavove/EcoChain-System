import React, {useEffect, useState} from "react";
import Home from "../pages/home";
import {backendGET} from "../controllers/backendComunication";
import MapDevices from "./mapDevices";

function GeneralDashboard(props) {
    const [dataDevices, setdataDevices] = useState(null);

    useEffect(() => {
        backendGET("http://10.0.2.15:5000/sensor/all/registered").then((data) => {
            setdataDevices(data);
        });

    }, []);

    return (<div>
            <div>
                <div>
                    <MapDevices
                </div>
                <h1>Response from API:</h1>
                <p>{JSON.stringify(props.data)}</p>
            </div>
        </div>
    )
}

export default GeneralDashboard