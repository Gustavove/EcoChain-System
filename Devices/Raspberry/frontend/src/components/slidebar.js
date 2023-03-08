import React, { useState } from 'react';

function Slidebar(props) {
    const [valorSeleccionado, setValorSeleccionado] = useState(1);

    function handleChange(event) {
        const nuevoValorSeleccionado = parseInt(event.target.value, 10);
        setValorSeleccionado(nuevoValorSeleccionado);
        props.onHandleValue(nuevoValorSeleccionado);
    }

    return (
        <div>
            <input type="range" min="1" max={props.valor} value={valorSeleccionado} onChange={handleChange} />
            <span>{valorSeleccionado}</span>
        </div>
    );
}
export default Slidebar;