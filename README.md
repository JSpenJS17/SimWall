## Description
Implements a variety of cellular automatas desktop wallpapers.
## Installation
**Note: This has been tested on: Ubuntu 22.04**
```
git clone https://github.com/JSpenJS17/SimWall.git
cd SimWall
make
```

`./simwall -h` for command line usage. 
## Electron GUI
Follow installation instructions above, then check if npm is installed with
```
npm -v
```
If not, install it. Then run
```
npm install electron-builder
cd electron/sim_wall
npm run build
```
After it's done building, there will be an executable file in the `electron/sim_wall/dist/` directory that will run the Electron GUI. The name of the executable changes based on OS.
##  Keybinds
- `Ctrl-Alt-Q` Quit
- `Ctrl-Alt-P` Pause
- `Ctrl-Alt-A` Enter/Exit add mode (unavailable for Langton's Ant)

-- Add Mode: Hold the left mouse button to add cells 
- `Ctrl-Alt-D` Delete all cells
## Command Line Arguments
| Feature         | Flag           | Default Value | Description |
|-|-|-|-|
| Daemonize | `-D`, `-d`, `--daemonize` | False         | Daemonize the process (no terminal window when running) [mac link]|
| Alive Color     | `-alive`       | FFFFFFFF      | Set the alive cell color |
| Dead Color      | `-dead`        | 000000FF      | Set the dead cell color |
| Dying Color     | `-dying`       | 808080FF      | Set the dying cell color (BB only) |
| Framerate       | `-fps`         | 10.0          | Set the framerate (float value) |
| Brian's Brain   | `-bb`          | False         | Run Brian's Brain instead of Game of Life |
| Seeds           | `-seeds`       | False         | Run Seeds instead of Game of Life |
| Langton's Ant   | `-ant`         | False         | Run Langton's Ant instead of Game of Life |
| Ant Parameters  | `-ant_params`  | ants.txt      | Give input ruleset, color list, ant locations, color, and directions as a .txt file |
| Circles         | `-c`           | False         | Draw circles instead of squares |
| Cell Size       | `-s`           | 25            | Set the cell size in pixels |
| No Keybinds     | `-nk`          | False         | Disables keybinds|
| No Restocking   | `-nr`          | False         | Will disable restocking of cells|
| Clear Board     | `-clear`       | False         | Starts the simulation with a clear board. Includes `-nr`|
| Usage           | `-h`, `--help` | False         | Print command line flag instructions |