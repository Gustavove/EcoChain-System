import React, {useEffect, useState} from "react";
import {backendGET} from "../../controllers/backendComunication";
import ProviderListExplorer from "./providerList";
import MapDevices from "../Maps/mapDevices";
import DevicesListExplorer from "./devicesListExplorer";
import SensorDashboard from "./sensorDashboard";


function DashboardExplorer(props) {
    const [infoExplorers, setInfoExplorers] = useState(null);
    const [dataDevices, setDataDevices] = useState(null);
    const [currentProviderAddress, setCurrentProviderAddress] = useState("");
    const [sensorData, setSensorData] = useState(null);

    useEffect(() => {
        backendGET("http://10.0.2.15:5000/provider/blockchain/getall").then((data) => {
            setInfoExplorers(data);
        });
    }, []);

    const onHandlebuttonEspecifidInfo = (provider_address) => {
        backendGET("http://10.0.2.15:5000/provider/blockchain/sensor/getall/lastinfo?provider_address=" + provider_address).then((data) => {
            setDataDevices(data);
            setCurrentProviderAddress(provider_address);
        });
    }

    const onHandlebuttonSensorEspecifidInfo = (sensor_mac) => {
        backendGET("http://10.0.2.15:5000/provider/blockchain/sensor/lastinfo?provider_address=" + currentProviderAddress + "&id=" + sensor_mac).then((data) => {
            console.log(data)
                setSensorData(data);
        });
    }

    return (<div>
                 <div>
                    <ProviderListExplorer
                        devices={infoExplorers}
                        onHandlebuttonEspecifidInfo={onHandlebuttonEspecifidInfo}
                    />
                </div>
                <div>
                    {dataDevices && <div>
                                        <MapDevices data={dataDevices}/>
                                    </div>
                    }
                </div>
                <div>
                    {dataDevices && <DevicesListExplorer
                            data={dataDevices}
                            onHandlebuttonEspecifidInfo={onHandlebuttonSensorEspecifidInfo}
                            providerAddress={currentProviderAddress}
                        />
                    }
                </div>
                <div>
                    {sensorData && sensorData.message !== "Sensor hasn't cids" && <SensorDashboard
                                    data={sensorData}
                                    />
                    }
                </div>
            </div>
            )
}

export default DashboardExplorer