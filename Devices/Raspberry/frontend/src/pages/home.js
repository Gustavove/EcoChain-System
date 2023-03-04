import React, { useEffect, useState } from 'react';
import Button from 'react-bootstrap/Button';
import {backendGET} from "../controllers/backendComunication";

function Home(props) {
    return (<div>
                <div>
                    <h1>Response from API:</h1>
                    <p>{JSON.stringify(props.data)}</p>
                </div>
               </div>
    )
}

export default Home