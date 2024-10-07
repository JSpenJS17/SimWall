import numpy as np
from screeninfo import get_monitors
from PIL import Image

monitor = None
for m in get_monitors():
    if m.is_primary:
        monitor = m
        break

if monitor is None:
    raise Exception("No primary monitor found")

# Set grid size and cell size
# Have it be a single array concatenated by rows
GRID_SIZE = (monitor.height//10, monitor.width // 10)
CELL_SIZE = 10

# Initialize the grid (randomly filled)
def initialize_grid():
    return np.random.choice([0, 1], size=GRID_SIZE, p=[0.8, 0.2])

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

# Main loop
def run_game_of_life():
    grid = initialize_grid()
    running = True
    print(grid.shape)

    count = 0
    while count < 60:
        grid = next_generation(grid)  # Get the next generation

        # color mapping
        image_data = grid * 255 
        image_data = image_data.astype(np.uint8)

        # make sure that the resolution is correct
        image = Image.fromarray(image_data, mode='L')

        new_size = (grid.shape[1] * CELL_SIZE, grid.shape[0] * CELL_SIZE)

        # Resize the image to the desired size
        image = image.resize(new_size, Image.NEAREST)

        # make sure they are alphabetical numerically
        image.save(f'artifacts/{count:02d}.png')

        print(f"Generation {count}")
        count += 1


if __name__ == "__main__":
    run_game_of_life()

