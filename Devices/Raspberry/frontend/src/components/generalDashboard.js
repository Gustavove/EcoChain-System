import React, {useEffect, useState} from "react";
import Home from "../pages/home";
import {backendGET} from "../controllers/backendComunication";
import DevicesList from "./devicesList";
import MapDevices from "./mapDevices";


function GeneralDashboard(props) {
    const [dataDevices, setdataDevices] = useState(null);

    useEffect(() => {
        backendGET("http://10.0.2.15:5000/sensor/all/lastinfo").then((data) => {
            setdataDevices(data);
        });
    }, []);

    const onHandlebuttonUpdate = () => {
        backendGET("http://10.0.2.15:5000/sensor/all/lastinfo").then((data) => {
            setdataDevices(data);
        });
    }

    return (<div className="abs-center">
                    <div>
                        <MapDevices data={dataDevices}/>
                    </div>
                    <div>
                        <DevicesList data={dataDevices} onHandlebuttonUpdate={onHandlebuttonUpdate} />
                    </div>
            </div>
            )
}

export default GeneralDashboard