import React, {useEffect, useState} from "react";
import Home from "../pages/home";
import {backendGET} from "../controllers/backendComunication";
import DevicesList from "./devicesList";


function GeneralDashboard(props) {
    const [dataDevices, setdataDevices] = useState(null);

    useEffect(() => {
        backendGET("http://10.0.2.15:5000/sensor/all/lastinfo").then((data) => {
            setdataDevices(data);
        });

    }, []);

    return (<div>
                <div>
                    <DevicesList data={dataDevices} />
                </div>
            </div>
            )
}

export default GeneralDashboard