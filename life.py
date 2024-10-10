import numpy as np
import time
import os
import sys

# Set default grid size
# GRID_SIZE = (monitor.height//10, monitor.width // 10) # old monitor display size code, will be used later
# get terminal size for grid size
rows, columns = os.popen('stty size', 'r').read().split()
GRID_SIZE = (int(rows)+1, int(columns)//2)
# Set default cell size
CELL_SIZE = 10

def set_grid_size(size):
    global GRID_SIZE
    GRID_SIZE = size

def set_cell_size(size):
    global CELL_SIZE
    CELL_SIZE = size

# Initialize the grid (randomly filled)
def initialize_grid(p = 0.2):
    return np.random.choice([0, 1], size=GRID_SIZE, p=[1-p, p])

# Compute the next generation of the grid
def next_generation(grid):
    new_grid = np.zeros(GRID_SIZE)
    for x in range(GRID_SIZE[0]):
        for y in range(GRID_SIZE[1]):
            alive_neighbors = np.sum(grid[x-1:x+2, y-1:y+2]) - grid[x, y]
            if grid[x, y] == 1 and not (2 <= alive_neighbors <= 3):
                new_grid[x, y] = 0  # Cell dies
            elif grid[x, y] == 0 and alive_neighbors == 3:
                new_grid[x, y] = 1  # Cell becomes alive
            else:
                new_grid[x, y] = grid[x, y]
    return new_grid

def clear_screen():
    print("\033[H\033[J", end="")

# Main loop
def run_game_of_life():
    grid = initialize_grid(.2)
    running = True

    while running:
        grid = next_generation(grid)  # Get the next generation

        clear_screen()

        # Display the grid
        out_str = ""
        for x in range(GRID_SIZE[0]):
            for y in range(GRID_SIZE[1]):
                if grid[x, y] == 1:
                    # print white for alive cells
                    out_str += "██"
                else:
                    out_str += "  "

        print(out_str, end="\r")
        time.sleep(.1)


if __name__ == "__main__":
    run_game_of_life()

