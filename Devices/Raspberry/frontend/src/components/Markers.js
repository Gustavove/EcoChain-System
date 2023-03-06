import React from "react";
import {Marker} from "react-leaflet";
import {IconLocation} from "./iconLocation";
function Markers(props){
    const markers = props.data && props.data.map((item) => (
        <Marker
            key={item.mac}
            position={{lat:item.gps.latitude, lng:item.gps.longitude}}
            icon={IconLocation}
        />
        ))
    return markers;
}

export default Markers