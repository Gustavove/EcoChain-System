import { useState } from 'react';
import { Button, Modal, Form } from 'react-bootstrap';
import {backendPOST} from "../controllers/backendComunication";

function OptionPanel(props) {
    const [showModal, setShowModal] = useState(false);
    const [value, setValue] = useState('');

    const handleClose = () => setShowModal(false);
    const handleShow = () => setShowModal(true);

    const handleAdd = () => {
        backendPOST("http://10.0.2.15:5000/sensor/new", {'id': value}).then( (info) => {
            console.log(info);
            props.onUpdate();
        });
        handleClose();
    };

    const handleChange = (event) => {
        setValue(event.target.value);
    };

    return (
        <>
            <Button variant="primary" onClick={handleShow}>
                Add Device +
            </Button>
            <Modal show={showModal} onHide={handleClose}>
                <Modal.Header closeButton>
                    <Modal.Title>Enter MAC:</Modal.Title>
                </Modal.Header>
                <Modal.Body>
                    <Form.Control type="text" value={value} onChange={handleChange} />
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="secondary" onClick={handleClose}>
                        Cancel
                    </Button>
                    <Button variant="primary" onClick={() => handleAdd()}>
                        Add
                    </Button>
                </Modal.Footer>
            </Modal>
        </>
    );
}

export default OptionPanel;