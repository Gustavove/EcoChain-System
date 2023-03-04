import Home from "../pages/home";

async function backendGET(url) {
    return await fetch(url)
        .then(response => response.json())
        .catch(error => console.error(error));
}

//data in json format
async function backendPOST(url, data) {
    return  await fetch(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(data)
        })
            .then(response => response.json())
            .then(data => {
                console.log(data);
                alert(data.mensaje);
            })
            .catch(error => console.error(error));
}

export { backendGET, backendPOST}



