import logo from './logo.svg';
import './App.css';
import {BrowserRouter, Navigate, Route, Routes} from "react-router-dom";

//Importamos componentes
import Explorer from './components/explorer'
import Home  from './components/home'
import NavBarComponent from "./layouts/navbar";

function App() {
  return (
    <div className="App">
      <BrowserRouter>
        <Routes>
            <Route path='/' element={<NavBarComponent/> }>
                <Route index  element={<Home/>} />
                <Route path='explorer' element={<Explorer/>}/>

                <Route path='*' element={ <Navigate replace to="/" />} />
            </Route>
        </Routes>
      </BrowserRouter>
    </div>
  );
}

export default App;
