Implements a Conway's Game of Life and Brian's Brain as a desktop wallpaper.

Display made with X11 libraries in C.

## TODO:
Add subdirectories for Linux, Mac, Windows w/ common library

Add display for paused vs add mode

Link Langton's Ant

## Feature Flags

| Feature         | Flag           | Default Value | Description                                                                 |
|-----------------|----------------|---------------|-----------------------------------------------------------------------------|
| Daemonize       | `-D`, `-d`     | False         | Daemonize the process (no terminal window when running) [mac link]          |
| Alive Color     | `-alive`       | FFFFFFFF      | Set the alive cell color                                                    |
| Dead Color      | `-dead`        | 000000FF      | Set the dead cell color                                                     |
| Dying Color     | `-dying`       | 808080FF      | Set the dying cell color                                                    |
| Framerate       | `-fps`         | 10.0          | Set the framerate                                                           |
| Brian's Brain   | `-bb`          | False         | Choose the simulation                                                       |
| Seeds           | `-seeds`       | False         |                                                                             |
| Langton's Ant   | `-ant`         | False         |                                                                             |
| Ant options     |                |               |                                                                             |
| - Ant Rules     | `-ant_rules`   | RLCU          | Set the ant's directional logic                                             |
| - Ant Color     | `-ant_color`   | FF0000FF      | Set the ant's color                                                         |
| - Color List    | `-color_list`  | TBD           | Set the color list for states in Langton's Ant                              |
| - Ants          | `-ants`        | ants.txt      | Give input ant locations, color, rules, directions (Have GUI make the text file) |
| Circles         | `-c`           | False         | Draw circles instead of squares                                             |
| Cell Size       | `-s`           | 25            | Set the cell size in pixels                                                 |
| Disable Keybinds| `-nk`          | False         |                                                                             |
| No Restocking   | `-nr`          | False         |                                                                             |
| Start with Clear Board | `-clear` | False         | Includes `-nr`                                                              |
| Command Help    | `-h`           | False         | Print command line flag instructions                                        |

- Seeds: 100 iterations reset
- RGBA values for CLI

## Linux
Run `make` to build.

`./simwall -h` for command line usage. 

##  Linux Keybinds

`Ctrl-Alt-Q` Quit

`Ctrl-Alt-P` Pause

`Ctrl-Alt-A` Enter/Exit add mode

`Ctrl-Alt-D` Delete all cells

Add Mode:

    - Hold the left mouse button to add cells

## Windows

## Mac OS

## Electron GUI
Make sure `npm` is installed
`npm -v`

If it is,
`npm install electron-builder`

Move to the electron/sim_wall/ directory
`npm run build`

Once that's done, there will be an executable file in the electron/simwall/dist/ directory that will run the Electron GUI. The name of the executable changes based on OS.
