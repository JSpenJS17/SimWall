// renderer.js

const { kill } = require('process');

// vars
let macPath = '../../MacOS/execs/SimWall'
let linuxPath = '../../Linux/exec'
let windowsPath = '../../Windows/exec'

// default vars
let settings = [
    ['deamonize', false],
    ['briansBrain', false],
    ['seeds', false],
    ['langtonsAnt', false],
    ['circles', false],
    ['disableKeybinds', false],
    ['noRestocking', false],
    ['startClear', false],
    ['aliveColor', 'FFFFFFFF'],
    ['deadColor', '000000FF'],
    ['dyingColor', '808080FF'],
    ['cellSize', 25],
    ['fps', 10.0]
];

let childProcess = null;

// functions
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
    
    switch(page){
        case ('gol'):
            for (let i = 0; i < settings.length; i++) {
                if (settings[i][0] === 'briansBrain' || settings[i][0] === 'langtonsAnt' || settings[i][0] === 'seeds') {
                    settings[i][1] = false;
                }
            }
            break;
        case('bb'):
            for (let i = 0; i < settings.length; i++) {
                if (settings[i][0] === 'briansBrain') {
                    settings[i][1] = true;
                } else if (settings[i][0] === 'langtonsAnt' || settings[i][0] === 'seeds') {
                    settings[i][1] = false;
                }
            }
            break;
        case('la'):
            for (let i = 0; i < settings.length; i++) {
                if (settings[i][0] === 'langtonsAnt') {
                    settings[i][1] = true;
                } else if (settings[i][0] === 'briansBrain' || settings[i][0] === 'seeds') {
                    settings[i][1] = false;
                }
            }
            break;
        case('seeds'):
            for (let i = 0; i < settings.length; i++) {
                if (settings[i][0] === 'seeds') {
                    settings[i][1] = true;
                } else if (settings[i][0] === 'briansBrain' || settings[i][0] === 'langtonsAnt') {
                    settings[i][1] = false;
                }
            }
            break;
    }
}

function setter(valueName, value) {
    for (let i = 0; i < settings.length; i++) {
        if (settings[i][0] === valueName) {
            settings[i][1] = value;
            break;
        }
    }
}

function callExec() {
    // determine proper path

    let execPath;
    switch (process.platform) {
        case 'darwin':
            execPath = macPath;
            break;
        case 'linux':
            execPath = linuxPath;
            break;
        case 'win32':
            execPath = windowsPath;
            break;
    }

    let command = execPath + ' ';
    
    // Build arguments from settings array
    for (let [setting, value] of settings) {
        switch(setting) {
            case 'deamonize':
                if (value) command += '-D ';
                break;
            case 'briansBrain':
                if (value) command += '-bb ';
                break;
            case 'seeds':
                if (value) command += '-seeds ';
                break;
            case 'langtonsAnt':
                if (value) command += '-ant ';
                break;
            case 'circles':
                if (value) command += '-c ';
                break;
            case 'disableKeybinds':
                if (value) command += '-nk ';
                break;
            case 'noRestocking':
                if (value) command += '-nr ';
                break;
            case 'startClear':
                if (value) command += '-clear ';
                break;
            case 'aliveColor':
                command += `-alive ${value} `;
                break;
            case 'deadColor':
                command += `-dead ${value} `;
                break;
            case 'dyingColor':
                command += `-dying ${value} `;
                break;
            case 'cellSize':
                command += `-s ${value} `;
                break;
            case 'fps':
                command += `-fps ${value} `;
                break;
        }
    }

    // Remove trailing space
    command = command.trim();

    // Execute the command using Node's child_process
    const { exec } = require('child_process');
    
    // if other process has started, kill it first
    if (childProcess) {
        killProcess();
    }

    // Start process
    childProcess = exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${error}`);
            return;
        }
        if (stderr) {
            console.error(`stderr: ${stderr}`);
        }
        if (stdout) {
            console.log(`stdout: ${stdout}`);
        }
    });
}

function killProcess() {
    if (childProcess) {
        try {
            if (process.platform == "win32") {
                require('child_process').exec(`taskill /pid ${childProcess.pid} /f`);
            } else {
                process.kill(childProcess.pid);
            }
        } catch (error) {
            console.error('Error killing process:', error);
        }
    }
}

// things to do (on startup)
changeContent('menu')
