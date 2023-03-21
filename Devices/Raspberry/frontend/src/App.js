import logo from './logo.svg';
import './App.css';
import {BrowserRouter, Navigate, Route, Routes} from "react-router-dom";
import React, { useEffect, useState } from 'react';
import {backendGET} from "./controllers/backendComunication";


//Importamos componentes
import Explorer from './pages/explorer'
import Home  from './pages/home'
import NavBarComponent from "./layouts/navbar";

function App() {
    const [responseData, setResponseData] = useState(null);

    useEffect(() => {
            backendGET("http://10.0.2.15:5000/init").then((data) => {
                setResponseData(data);
            });
    }, []);

    return(    <div className="App">
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
        )
}

export default App;
