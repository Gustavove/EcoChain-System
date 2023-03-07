import React, { useEffect, useState } from 'react';
import { render } from 'react-dom';
import { MapContainer, TileLayer} from 'react-leaflet';
import 'leaflet/dist/leaflet.css';
import Markers from "./Markers";

function MapDevices(props){
    return (
        <MapContainer center={{lat:'41.3180904', lng:'2.0098645'}} zoom={13}>
            <TileLayer
                attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
                url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            />
            <Markers data={props.data}/>
        </MapContainer>
    );
}

export default MapDevices