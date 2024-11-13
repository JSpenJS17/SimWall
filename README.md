Implements a Conway's Game of Life and Brian's Brain as a desktop wallpaper.

Display made with X11 libraries in C.

## TODO:
Add subdirectories for Linux, Mac, Windows w/ common library

Add display for paused vs add mode

Link Langton's Ant

## Linux
Run `make` to build.

`./simwall -h` for command line usage. 

## Electron GUI
Make sure `npm` is installed
`npm -v`

If it is,
`npm install electron-builder`

Move to the electron/sim_wall/ directory
`npm run build`

Once that's done, there will be an executable file in the electron/simwall/dist/ directory that will run the Electron GUI. The name of the executable changes based on OS.

## Keybinds

`Ctrl-Alt-Q` Quit

`Ctrl-Alt-P` Pause

`Ctrl-Alt-A` Enter/Exit add mode

`Ctrl-Alt-D` Delete all cells

Add Mode:

    - Hold the left mouse button to add cells