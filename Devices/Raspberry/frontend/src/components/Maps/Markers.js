import React from "react";
import {Marker} from "react-leaflet";
import {IconLocation} from "./iconLocation";
function Markers(props){
    let markers
    if (Array.isArray(props.data)) {
        markers = props.data && props.data.map((item) => (
            item.gps && item.gps.latitude && item.gps.longitude ? (
                <Marker
                    key={item.mac}
                    position={{ lat: item.gps.latitude, lng: item.gps.longitude }}
                    icon={IconLocation}
                />
            ) : null
        ))
    } else {
        markers = props.data && <Marker
            position={{lat:props.data.gps.latitude, lng:props.data.gps.longitude}}
            icon={IconLocation}
        />
    }
    return markers;
}

export default Markers