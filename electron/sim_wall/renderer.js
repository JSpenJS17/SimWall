//renderer.js

// used to detect when button is pressed and log it, for now
document.addEventListener('DOMContentLoaded', () => {
    const buttons = document.querySelectorAll('.column .button:not(.is-success)');
    let selectedButton = null;

    buttons.forEach(button => {
        button.addEventListener('click', () => {
            if (selectedButton) {
                selectedButton.classList.remove('is-white');
            }
            button.classList.add('is-white');
            selectedButton = button;
        });
    });

    document.querySelector('.button.is-success').addEventListener('click', () => {
        if (selectedButton) {
            console.log(selectedButton.id);
        }
    });
});