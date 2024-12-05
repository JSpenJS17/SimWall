// renderer.js

const { kill } = require('process');
const { ipcRenderer } = require('electron');
const { exec } = require('child_process');
const path = require('path');
const fs = require('fs');


// uncomment this block for final builds. make sure to comment out other paths
let macPath = path.join(process.resourcesPath, 'execs', 'MacOS', 'simwall_cmd');
let linuxPath = path.join(process.resourcesPath, 'execs', 'Linux', 'simwall_cmd');
let windowsPath = path.join(process.resourcesPath, 'execs', 'Windows', 'simwall_cmd');

// let macPath = "./execs/mac/simwall_cmd"
// let linuxPath = "./execs/linux/simwall_cmd"
// let windowsPath = "execs\\windows\\simwall_cmd


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

let default_settings = JSON.parse(JSON.stringify(settings));

let childProcess = null;

// functions
function changeContent(page) {
    settings = JSON.parse(JSON.stringify(default_settings));
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
        if (settings[i][0] == valueName) {
            settings[i][1] = value;
            break;
        }
    }
}

function toggler(valueName) {
    for (let i = 0; i < settings.length; i++) {
        if (settings[i][0] == valueName) {
            settings[i][1] = !settings[i][1];
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
                if (value) {
                    const filePath = path.join(process.resourcesPath, 'langtonsFile.txt');
                    command += `-ant ${filePath} `;
                }
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
const { remote } = require('electron');
const { ipcRenderer } = require('electron');
    
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
    console.log('Killing process');

    if (childProcess) {
        try {
            if (process.platform == "win32") {
                exec(`taskkill /PID ${childProcess.pid} /T /F`);
            } else {
                process.kill(childProcess.pid);
            }
        } catch (error) {
            console.error('Error killing process:', error);
        }
    }
}

function updateRGBAColor(type) {
    const red = parseInt(document.getElementById(type + 'Red').value).toString(16).padStart(2, '0');
    const green = parseInt(document.getElementById(type + 'Green').value).toString(16).padStart(2, '0');
    const blue = parseInt(document.getElementById(type + 'Blue').value).toString(16).padStart(2, '0');
    const alpha = Math.round((document.getElementById(type + 'Alpha').value / 255) * 255).toString(16).padStart(2, '0');
    const rgbaColor = `rgba(${parseInt(red, 16)}, ${parseInt(green, 16)}, ${parseInt(blue, 16)}, ${parseInt(alpha, 16) / 255})`;
    document.getElementById(type + 'Button').style.backgroundColor = rgbaColor;
    const rgbaColorHex = `${red}${green}${blue}${alpha}`;
    document.getElementById(type + 'Value').innerHTML = rgbaColorHex;
    setter(type, rgbaColorHex);
}

function updateFrameRateValue(value) {
    document.getElementById('frameRateValue').textContent = value;
    setter('fps', value);
}

function updateCellSizeValue(value) {
    document.getElementById('cellSizeValue').textContent = value;
    setter('cellSize', value);
}

function closeWindow() {
    ipcRenderer.send('close-window');
}

function displayExample(num) {
    const antFileContent = document.getElementById('antFileContent');
    const filePath = path.join(process.resourcesPath, 'ExampleAnts', `ex_ants${num}.txt`);
    fetch(filePath)
        .then(response => response.text())
        .then(data => {
            antFileContent.value = data;
        })
        .catch(error => {
            console.error('Error fetching the example file:', error);
        });
}

function setLangtonsFile() {
    const filePath = path.join(process.resourcesPath, 'langtonsFile.txt');
    const antFileContent = document.getElementById('antFileContent').value;

    fs.writeFile(filePath, antFileContent, (err) => {
        if (err) {
            console.error('Error writing to file:', err);
        } else {
            console.log('File has been saved.');
        }
    });
}

// things to do (on startup or statically)
changeContent('menu');

