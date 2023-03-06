import React, { useEffect, useState } from 'react';
import Button from 'react-bootstrap/Button';
import {backendGET} from "../controllers/backendComunication";
import GeneralDashboard from "../components/generalDashboard";

function Home(props) {
    return (<div>
                <div>
                    <GeneralDashboard />
                </div>
               </div>
    )
}

export default Home