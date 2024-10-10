Implements a Conway's Game of Life wallpaper.

Display made in Python using Pygame.

## UPDATE TO TECH DEMO:
Can run `make` to build
`./simwall` will run process
`./simwall -D` for daemon
to kill: `killall simwall`

## For Linux:
    - Install dependencies
`sudo apt-get install xorg-dev build-essential libx11-dev x11proto-xext-dev libxrender-dev libxext-dev
git clone https://github.com/mmhobi7/xwinwrap.git
cd xwinwrap
make
sudo make install
make clean
pip install pygame screeninfo`
    - Run start.sh
    - Click your desktop to display icons above the game

### To run in Daemon mode:
`tmux
./start.sh
Ctrl+b d`
This will start a tmux session, then you run start.sh, then you daemonize the tmux session.
We'll add a daemon mode in a future release.


