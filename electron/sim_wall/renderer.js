// renderer.js

function changeContent(page) {
    var contentDiv = document.getElementById('contentDiv');
    fetch(`pages/${page}.html`)
        .then(response => response.text())
        .then(data => {
            contentDiv.innerHTML = data;
        })
        .catch(error => {
            console.error('Error fetching the page:', error);
        });
}

changeContent('menu')