import pygame
import numpy as np
import time
from screeninfo import get_monitors
import sys
import os
import argparse

# Set up argument parser
parser = argparse.ArgumentParser(description="Conway's Game of Life in Pygame.")
parser.add_argument('-w', '--window', '-wid', type=str, help='Window ID to display in.')
args = parser.parse_args()

# If a window ID is provided, set it in the environment
if args.window:
    os.environ['SDL_WINDOWID'] = args.window
    print(f"Using window ID: {os.environ['SDL_WINDOWID']}")
else:
    print("No window ID provided, dying")
    sys.exit(1)

# Get monitor for its dimensions later
monitor = None
for m in get_monitors():
    if m.is_primary:
        monitor = m
        break

# Die if there is no monitor
if monitor is None:
    print("No primary monitor found, dying")
    sys.exit(2)

# Set grid size and cell size
CELL_SIZE = 25
GRID_SIZE = (monitor.width//CELL_SIZE, monitor.height//CELL_SIZE)

# Initialize Pygame
pygame.init()
# make full screen
screen = pygame.display.set_mode((0,0), pygame.FULLSCREEN)
pygame.display.set_caption("Conway's Game of Life")

# Colors
ALIVE_COLOR = (255, 255, 255)  # White for alive cells
DEAD_COLOR = (0, 0, 0)         # Black for dead cells

# Initialize the grid (randomly filled)
def initialize_grid():
    return np.random.choice([0, 1], size=GRID_SIZE, p=[0.8, 0.2])

# Compute the next generation of the grid
def next_generation(grid):
    new_grid = np.copy(grid)
    for x in range(GRID_SIZE[0]):
        for y in range(GRID_SIZE[1]):
            alive_neighbors = np.sum(grid[x-1:x+2, y-1:y+2]) - grid[x, y]
            if grid[x, y] == 1 and not (2 <= alive_neighbors <= 3):
                new_grid[x, y] = 0  # Cell dies
            elif grid[x, y] == 0 and alive_neighbors == 3:
                new_grid[x, y] = 1  # Cell becomes alive
    return new_grid

# Draw the grid on the screen
def draw_grid(screen, grid):
    for x in range(GRID_SIZE[0]):
        for y in range(GRID_SIZE[1]):
            color = ALIVE_COLOR if grid[x, y] == 1 else DEAD_COLOR
            pygame.draw.rect(screen, color, (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE))

# Main loop
def run_game_of_life():
    grid = initialize_grid()
    clock = pygame.time.Clock()
    running = True

    while running:
        screen.fill(DEAD_COLOR)  # Clear screen
        grid = next_generation(grid)  # Get the next generation
        draw_grid(screen, grid)  # Draw the grid
        pygame.display.flip()  # Update the display

        # Event handling
        for event in pygame.event.get():
            # On escape press, die
            # will need to be removed later prolly
            if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False

        clock.tick(5)  # Control frame rate

    pygame.quit()

if __name__ == "__main__":
    run_game_of_life()

