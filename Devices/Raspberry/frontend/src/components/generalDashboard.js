import React, {useEffect, useState} from "react";
import Home from "../pages/home";
import {backendGET} from "../controllers/backendComunication";
import DevicesList from "./devicesList";
import MapDevices from "./mapDevices";
import EspecificDashboard from "./especificDashboard";


function Dashboard(props) {
    const [dataDevices, setdataDevices] = useState(null);
    const [dataEspecificDevice, setEspecificDevice] = useState(null);

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
    const onHandlebuttonEspecifidInfo = (device_info) => {
        backendGET("http://10.0.2.15:5000/sensor/lastinfo?id=" + device_info).then((data) => {
            setEspecificDevice(data);
        });
    }

    return (<div>
            <div className="abs-center">
                            <div>
                                <MapDevices data={dataDevices}/>
                            </div>
                            <div>
                                <DevicesList
                                    data={dataDevices}
                                    onHandlebuttonUpdate={onHandlebuttonUpdate}
                                    onHandlebuttonEspecifidInfo={onHandlebuttonEspecifidInfo}
                                />
                            </div>
                    </div>
            <div>
                { dataEspecificDevice && <EspecificDashboard data={dataEspecificDevice}/>}
            </div>
            </div>
            )
}

export default Dashboard