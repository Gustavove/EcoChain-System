import React, {useEffect, useState} from "react";
import {backendGET} from "../../controllers/backendComunication";
import ProviderListExplorer from "./providerList";


function DashboardExplorer(props) {
    const [infoExplorers, setInfoExplorers] = useState(null);
    const [sensorsMac, setSensorMacs] = useState(null)

    useEffect(() => {
        backendGET("http://10.0.2.15:5000/provider/blockchain/getall").then((data) => {
            setInfoExplorers(data);
        });
    }, []);

    const onHandlebuttonEspecifidInfo = (provider_address) => {
        backendGET("http://10.0.2.15:5000/provider/blockchain/getall/sensors?provider_address=" + provider_address).then((data) => {
            setSensorMacs(data);
        });
    }

    return (<div>
                <ProviderListExplorer
                    devices={infoExplorers}
                    onHandlebuttonEspecifidInfo={onHandlebuttonEspecifidInfo}
                />
            <div>

            </div>

            </div>
            )
}

export default DashboardExplorer