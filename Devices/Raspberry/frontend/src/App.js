import logo from './logo.svg';
import './App.css';
import {BrowserRouter, Navigate, Route, Routes} from "react-router-dom";
import React, { useEffect, useState } from 'react';
import Button from 'react-bootstrap/Button';
import {backendGET} from "./controllers/backendComunication";


//Importamos componentes
import Explorer from './pages/explorer'
import Home  from './pages/home'
import NavBarComponent from "./layouts/navbar";

function App() {

    const [isLoading, setLoading] = useState(false);
    const [responseData, setResponseData] = useState(null);

    useEffect(() => {
        if (isLoading) {
            backendGET("http://10.0.2.15:5000/init").then((data) => {
                setLoading(false);
                setResponseData(data);
            });
        }
    }, [isLoading]);

    const handleClick = () => setLoading(true);
    let code = <Button
        variant="primary"
        disabled={isLoading}
        onClick={!isLoading ? handleClick : null}>
        {isLoading ? 'Loading…' : 'Start'}
    </Button>
    if(responseData){
        code =
            <div className="App">
                <BrowserRouter>
                    <Routes>
                        <Route path='/' element={<NavBarComponent/> }>
                            <Route index  element={<Home data={responseData}/>} />
                            <Route path='explorer' element={<Explorer/>}/>
                            <Route path='*' element={ <Navigate replace to="/" />} />
                        </Route>
                    </Routes>
                </BrowserRouter>
            </div>
    }
    return code;
}

export default App;
