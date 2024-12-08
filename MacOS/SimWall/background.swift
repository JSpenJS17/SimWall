/*
 FILE: background.swift
 AUTHORS: Michael Stang
 FUNCTION: Main file for the object that is the background
 */

import SwiftUI
import Foundation
import AppKit

// SOME VALUES THAT ARE HARDCODED BUT CHANGEABLE
let scaleFactor = NSScreen.main?.backingScaleFactor ?? 10.0
let GRID_WIDTH = Int((Int((NSScreen.main?.frame.size.width ?? 320) * scaleFactor) / cell_size))
var GRID_HEIGHT = Int((Int((NSScreen.main?.frame.size.height ?? 180) * scaleFactor) / cell_size))
let STARTING_PERCENTAGE = 30 // as int out of 100
let RESET_PERCENT = 0.05 // as double

func life_remaining(layout: [[Int]], width: Int, height: Int) -> Double {
    // Basic function that returns the percentage of life that remains in the layout
    
    var sum = 0 // Sum var to keep track of total
    for row in layout { // Goes through each row
        sum += row.reduce(0, +) // Used reduce to add together all of the values
    }
    // Then calculates the percentage by diviing the sum by the total
    return Double(sum) / Double(width * height)
}

var ants = [Ant(x: Int32(GRID_WIDTH/2), y: Int32(GRID_HEIGHT/2), direction: 0, ruleset: "RLCU", color: hex_string_to_color(from: "FF0000FF"))]


struct Background: View {
    // The actual background view object
    // It has a bunch of things it takes in, mostly flags from the command
    @State var layout: [[Int]] = makeEmptyBoard(width: GRID_WIDTH, height: GRID_HEIGHT)
    
    // The body of the view
    var body: some View {
        // A ZStack to set up the background black layer VS the tiles on top
        ZStack {
            if !(alive_alpha < 255 || dying_alpha < 255) { // If we don't have transparency in the dead or dying cells
                dead_color // make whole background color for optimization
            }

            // Use Canvas for efficient drawing
            Canvas { context, size in
                // Calculates the size of the cell
                let cellSize = size.width / CGFloat(GRID_WIDTH)
                
                // We have two modes here, one for if the user wants tiles on top of a background
                // And the other if users want their background to shine through
                if ((alive_alpha < 255 || dying_alpha < 255) && dead_alpha != 0) { // This is the mode for the background shining through // was checking for inverted before
                    for row in 0..<GRID_HEIGHT { // Loops for each row
                        for column in 0..<GRID_WIDTH { // Loops for each column
                            let cellValue = layout[row][column] // Gets the value that the cell should be (mostly 0 or 1, but sometimes 2)
                            if cellValue == 0 { // If the cell is dead we want to render it as a black cell, to create the effect that the live cells are shining through
                                let rect = CGRect( // Creates a rectangle object
                                    x: CGFloat(column) * cellSize, // With a given X
                                    y: CGFloat(row) * cellSize, // given y
                                    width: cellSize, // and a square size
                                    height: cellSize // having the same height as width
                                )

                                // Then we make a path object
                                var path: Path
                                path = Path(rect) // And set it equal to the rectangle we made above

                                let cellColor: Color = dead_color // We set the color to fill it, in this case always black, but may be white if we add dark/light mode detection
                                context.fill(path, with: .color(cellColor)) // Then we fill it in
                                context.stroke(path, with: .color(dead_color), lineWidth: 0.5) // Then we add a stroke aroudn the blocks to avoid any of the desktop from shinig through the seems
                            }
                        }
                    }
                    
                } //else { // If the user wants the classic mode of colored tiles on a background
                for row in 0..<GRID_HEIGHT { // Same as above, looping through
                    for column in 0..<GRID_WIDTH { // Loops for the width still as well
                        var ant_pos: [[Int32]] = [] // an array of coord positions of all of the ants
                        if simulation == "ant" { // If we're doing the ant simulation
                            for ant in ants { // Go through each ant
                                ant_pos.append([ant.x, ant.y]) // Add its position to the list
                            }
                        }
                        let cellValue = layout[row][column] // Gets the cell value
                        if (cellValue != 0 || ant_pos.contains([Int32(column), Int32(row)])){ // Now we care about whether the cell is alive, not whether it is dead
                            // We build our cell the exact same way as above/
                            let rect = CGRect( // Makes tile
                                x: CGFloat(column) * cellSize, // Gets x
                                y: CGFloat(row) * cellSize, // Gets y
                                width: cellSize, // sets width
                                height: cellSize // sets height
                            )

                            // We make our path object
                            var path: Path
                            // We check if the user wants circles, or if they want squares
                            if shape == "circle" { // If they want circles
                                path = Path(ellipseIn: rect) // We make the object a circle
                            } else { // Otherwise,
                                path = Path(rect) // We make the object a square
                            }

                            var cellColor: Color = .white
                            // We set the color
                            if simulation == "ant" {
                                if ant_pos.contains([Int32(column), Int32(row)]) { // Check if the cell is one that contains an ant
                                    for ant in ants { // Go through each ant
                                        if row == ant.y && column == ant.x { // Checks if that ant is here and if so,
                                            cellColor = ant.color // sets the tile to that ant's color
                                        }
                                    }
                                    
                                } else {
                                    cellColor = ant_colors[cellValue] // otherwise color it based on the ant rule color list
                                }

                            } else {
                                cellColor = (cellValue == 1) ? alive_color : dying_color // The color is according to the flags, except for cells with the "2" value which are always pink for now
                            }
                            context.fill(path, with: .color(cellColor)) // Then we fill it in
                        }
                    }
                }
                //}
            }
            .aspectRatio(16/9, contentMode: .fill) // Sets the aspect ratio for the whole thing to be 16/9 and to fit it in, even if its too small or large
            .ignoresSafeArea() // We don't care if it goes into unclickable sections since we're not able to click on it anyways
        }
        .onAppear {
            // When the background appears we have a lot of work to do
            if simulation == "ant" { // When we start, if we're doing ants
                if ants_file != "" { // And we have an ants file
                    ants = file_to_ant(path: ants_file) // read it in and get list of ants from it using utility function
                } else { // Otherwise, we're doing default
                    ants = [Ant(x: Int32(GRID_WIDTH/2), y: Int32(GRID_HEIGHT/2), direction: 0, ruleset: "LR", color: hex_string_to_color(from: "FF0000FF"))] // Base ant
                }
            }

            // If the user didn't set a starting pattern, we need to generate one randomly
            if start_with_clear_board { // If we're starting with empty board
                layout = makeEmptyBoard(width: GRID_WIDTH, height: GRID_HEIGHT) // set board to clear board
            } else { // Otherwise (most of the time)
                if path == "" { // If we don't have a path to a starting pattern
                    if simulation == "seeds" { // The seeds simulation goes from small to big, so we have a special random for it
                        layout = makeSmallBoard(width: GRID_WIDTH, height: GRID_HEIGHT) // Generates a small baord
                    } else if simulation == "ant" {
                        layout = makeEmptyBoard(width: GRID_WIDTH, height: GRID_HEIGHT) // Ants start on empty board and add on
                    } else { // Otherwise, we start big and get smaller, so we just use the C function from game_of_life to generate a random board
                        layout = cArrayToSwiftArray(pattern: golGenRandom(width: GRID_WIDTH, height: GRID_HEIGHT, percentAlive: STARTING_PERCENTAGE)!, width: GRID_WIDTH, height: GRID_HEIGHT) // Generates the board and then converts it to a Swift object
                    }
                    
                } else { // Otherwise, we read in from file
                    let grid = file_to_board(path: path, width: GRID_WIDTH, height: GRID_HEIGHT) // Use the helper function from converted_functions.swift to make get board
                    layout = grid // Set the board we got to the layout of this object
                }
            }
            // START LOOP
            var iterations = 0 // variable used to track how many iterations have happened with the current board
            
            Timer.scheduledTimer(withTimeInterval: 1/fps, repeats: true) { _ in // Timer set to update board based on passed in speed
                DispatchQueue.global(qos: .userInitiated).async { // Need to use an sync queue to ensure updates happen in the right order
                    iterations += 1 // Up the iteration count
                   
                    // For this next section we do a lot of the same thing of generating the next board
                    // By passing in the current board state into converted C functions
                    // We then check if there's enough life left, and if not regenerate
                    // Each simulation has a slightly different way of checking whether a reset is needed
                    // And is disucsesed in each sections's comments
                    // Also, if the original board came from a file, we just reset to that starting state
                   
                    if simulation == "brians_brain" { // If we're doing Brian's Brain simulation
                        if let nextPattern = bbGenNext(pattern: swiftArrayToCArray(board2D: layout, width: GRID_WIDTH, height: GRID_HEIGHT), width: GRID_WIDTH, height: GRID_HEIGHT) { // Generates next frame using function
                            DispatchQueue.main.async { // Need to queue actions in a DispatchQueue to ensure proper ordering
                                layout = cArrayToSwiftArray(pattern: nextPattern, width: GRID_WIDTH, height: GRID_HEIGHT) // Updates layout
                                if (life_remaining(layout: layout, width: GRID_WIDTH, height: GRID_HEIGHT) <= RESET_PERCENT/3) && iterations > 150 && !disable_restocking { // We need there to be a third of the reset percentage left and past a certain iteration count (meaning even less than normal since BB can often generate more tiles from low amounts) and we must have seen 150 generations to ensure smaller starting patterns get a chance to get going
                                    iterations = 0 // Resets iteration counter since we've reset
                                    if start_with_clear_board { // If we started with a clear board, when we reset we should go back to clear
                                        layout = makeEmptyBoard(width: GRID_WIDTH, height: GRID_HEIGHT)
                                    } else { // Otherwise,
                                        layout = path == ""
                                        ? cArrayToSwiftArray(pattern: golGenRandom(width: GRID_WIDTH, height: GRID_HEIGHT, percentAlive: STARTING_PERCENTAGE)!, width: GRID_WIDTH, height: GRID_HEIGHT)
                                        : file_to_board(path: path, width: GRID_WIDTH, height: GRID_HEIGHT) // or read from file if not random
                                    }
                                }
                            }
                        }
                    } else if simulation == "game_of_life" { // If we're doing Game of Life simulation
                        if let nextPattern = golGenNext(pattern: swiftArrayToCArray(board2D: layout, width: GRID_WIDTH, height: GRID_HEIGHT), width: GRID_WIDTH, height: GRID_HEIGHT) { // Generates next frame using function
                            DispatchQueue.main.async { // Need to queue actions in a DispatchQueue to ensure proper ordering
                                layout = cArrayToSwiftArray(pattern: nextPattern, width: GRID_WIDTH, height: GRID_HEIGHT) // Updates layout
                                if (life_remaining(layout: layout, width: GRID_WIDTH, height: GRID_HEIGHT) <= RESET_PERCENT) && iterations > 150 && !disable_restocking { // We need a percentage of life <= the set threshold and we must have seen 150 generations to ensure smaller starting patterns get a chance to get going
                                    iterations = 0 // Resets iteration counter since we've reset
                                    if start_with_clear_board { // If we started with a clear board, when we reset we should go back to clear
                                        layout = makeEmptyBoard(width: GRID_WIDTH, height: GRID_HEIGHT)
                                    } else { // Otherwise,
                                        layout = path == ""
                                        ? cArrayToSwiftArray(pattern: golGenRandom(width: GRID_WIDTH, height: GRID_HEIGHT, percentAlive: STARTING_PERCENTAGE)!, width: GRID_WIDTH, height: GRID_HEIGHT)
                                        : file_to_board(path: path, width: GRID_WIDTH, height: GRID_HEIGHT) // or read from file if not random
                                    }
                                }
                            }
                        }
                    } else if simulation == "seeds" { // If we're doing Seeds simulation
                        if let nextPattern = seedsGenNext(pattern: swiftArrayToCArray(board2D: layout, width: GRID_WIDTH, height: GRID_HEIGHT), width: GRID_WIDTH, height: GRID_HEIGHT) { // Generates next frame using function
                            DispatchQueue.main.async { // Need to queue actions in a DispatchQueue to ensure proper ordering
                                layout = cArrayToSwiftArray(pattern: nextPattern, width: GRID_WIDTH, height: GRID_HEIGHT) // Updates layout
                                if iterations >= 100 && !disable_restocking { // We don't care about life_count since seeds typically grows rather than shrinks, we just care about getting to 150 iterations and then resetting
                                    iterations = 0 // Resets iteration counter since we've reset
                                    if start_with_clear_board { // If we started with a clear board, when we reset we should go back to clear
                                        layout = makeEmptyBoard(width: GRID_WIDTH, height: GRID_HEIGHT)
                                    } else { // Otherwise,
                                        layout = path == "" ? makeSmallBoard(width: GRID_WIDTH, height: GRID_HEIGHT) : file_to_board(path: path, width: GRID_WIDTH, height: GRID_HEIGHT) // or read from file if not random
                                    }
                                }
                            }
                        }
                    } else if simulation == "ant" { // If we're doing ant simulation
                        if let nextPattern = antGenNext(pattern: swiftArrayToCArray(board2D: layout, width: GRID_WIDTH, height: GRID_HEIGHT), width: GRID_WIDTH, height: GRID_HEIGHT, ants: &ants) { // Use function to generate next frame given current frame and ant list
                            DispatchQueue.main.async { // Need to queue actions in a DispatchQueue to ensure proper ordering
                                layout = cArrayToSwiftArray(pattern: nextPattern, width: GRID_WIDTH, height: GRID_HEIGHT) // Just update the layout
                                
                                // This one is pretty simple due to ants not having things like restocking
                            }
                        }
                    }
                }
            }
        }
    }
}
