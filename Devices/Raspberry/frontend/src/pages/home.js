import React, { useEffect, useState } from 'react';
import GeneralDashboard from "../components/generalDashboard";
import MapDevices from "../components/mapDevices";

function Home(props) {
    return (<div>
                <div>
                    <GeneralDashboard />
                </div>
            </div>
    )
}

export default Home