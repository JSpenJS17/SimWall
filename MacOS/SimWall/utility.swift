/*
 FILE: utility.swift
 AUTHORS: Michael Stang
 FUNCTION: File with various utility functions used throughout
 */


// Imports Needed
import SwiftUI // SwiftUI for manipulating view objects
import AppKit // AppKit used for color objects and manipulation

func makeSmallBoard(width: Int, height: Int) -> [[Int]] {
    // Function to make a board that only contains random values in the inner 6x6
    
    var board: [[Int]] = [] // Makes a blank board that we will fill in
    
    for row in 0..<height { // Iterates for each row we should end up
        board.append([]) // Appends a new row to the grid for the row
        
        for col in 0..<width { // Goes through each column that should contain a value in the row
            if (row >= ((height/2) - 3) && row <= ((height/2) + 3)) && (col >= ((width/2) - 3) && col <= ((width/2) + 3)) { // If we're in the inner 6x6
                board[row].append(Int.random(in: 0...1)) // Add either a 1 or 0
            } else { // Otherwise
                board[row].append(0) // Make it a blank spot
            }
        }
    }
    
    return board // Return the board we built
}

func makeEmptyBoard(width: Int, height: Int) -> [[Int]] {
    // Make an empty board of given height and width
    
    var board: [[Int]] = [] // New board to write values into
    
    for row in 0..<height { // Goes through each row
        board.append([]) // Adds it to the board
        for _ in 0..<width { // Goes for each value that it should have in each row
            board[row].append(0) // Adds 0s for the blank board
        }
    }
    return board // Returns the board
}

func file_to_board(path: String, width: Int, height: Int) -> [[Int]] {
    // Function to take a path to a file and output a board with the pattern in that file in the middle of the board
    
    var board: [[Int]] = [] // Board to fill in
    let read_in = read_in_file(path: path) // Board represented by file
    let patternHeight = read_in.count // The read in board's height
    let patternWidth = read_in[0].count // The read in board's width
    let rowStart = (height - patternHeight) / 2 // Calculate which row the pattern should start on in the larger grid
    let colStart = (width - patternWidth) / 2 // Calculate which column the pattern should start on in the larger grid

    for row in 0..<height { // Goes through each row
        
        var boardRow: [Int] = [] // Adds a row to the grid
        
        for col in 0..<width { // Goes through each column in each row
            if row >= rowStart && row < rowStart + patternHeight && col >= colStart && col < colStart + patternWidth { // If its within the zone that should be represented by the file's pattern
                boardRow.append(read_in[row - rowStart][col - colStart]) // Add the value from that pattern
            } else {
                boardRow.append(0) // Otherwise it should be blank
            }
        }
        
        board.append(boardRow) // Add our new row to the grid
    }
    
    return board // Return the grid
}

func read_in_file(path: String) -> [[Int]] {
    // Function to read in from a file and make a grid from it
    
    var grid: [[Int]] = [] // Grid that will hold our output
    
    do {
        // Do block used to catch issues opening file
        
        let content = try String(contentsOfFile: path) // Read the file content as a single string
        
        let lines = content.split(separator: "\n") // Split the content by lines
        
        for line in lines { // Go through each line
            
            var row: [Int] = [] // Make a row for each line
            
            for char in line { // Go through each character in the line
                if let number = Int(String(char)) { // If it can be cast to an Int
                    row.append(number) // Add it to the line
                }
            }
            
            grid.append(row) // Add the row to our grid
        }
        
    } catch { // If we error, print for debug
        
        print("Could not open file: " + path) // print error to console
        abort_start = true // abort the start
        exit(1) // end the process
        
    }
    return grid // Return our newly made grid
}

func file_to_ant(path: String) -> [Ant] {
    // Function to read in a file and conver to list of ants
    
    do {
        // 'Do' block used to catch file errors
        
        let content = try String(contentsOfFile: path)  // Gets the contents of the file at the given path
                                                        // If this errors, it will jump to the catch and will print error message and abort
        
        let lines = content.split(separator: "\n") // Takes the content from the file and splits it into an array for easy processing
        
        var ants: [Ant] = [] // Blank list that will hold our ants
        var rule = "LR" // Rule for the ants. This is the default, may be changed by future code
        
        var index_l = 0 // index for tracking which line we're looking at
        
        for line in lines {
            // Iterate through all of the lines
            
            if index_l == 0 {
                // If we're looking at the first line, it's the rule we should use
                rule = String(line) // Updates rule to given rule in the file
                
            } else if index_l == 1 {
                // If we're looking at the second line, it's the colors for the rules
                // Index 0 is color of background
        
                if String(line) == "default" {
                    // If the color is set to default, we have to calculate colors
                    
                    let num_colors = rule.count // Gets the number of elements in the rule, each rule needs a color
                    
                    for default_color in 1..<num_colors+1 {
                        // Loops a number of times equal to the number of colors we need (one for each rule element)
                        
                        let step_size: Double = Double(255) / Double((num_colors-1)) // Calculates the size of the step between each color we generate
                        
                        // Calculates the next color by using the index we're at plus the step and calculating the Boolean
                        let step_color = (Color(red: (Double(default_color) * step_size)/Double(255), green: (Double(default_color) * step_size)/Double(255), blue: (Double(default_color) * step_size)/Double(255)))
                       
                        ant_colors.append(step_color) // adds that color to our color list
                    }
                } else if line == "default_alpha" {
                    // If the color is set to default_alpha, it's the default but with a transparent background
                    // Same as default for most of this, just with a transparent background
                    
                    let num_colors = rule.count // Gets the number of elements in the rule, each rule needs a color
                    
                    for default_color in 1..<num_colors+1 {
                        // Loops a number of times equal to the number of colors we need (one for each rule element)
                        
                        let step_size: Double = Double(255) / Double((num_colors-1)) // Calculates the size of the step between each color we generate
                        
                        // Calculates the next color by using the index we're at plus the step and calculating the Boolean
                        let step_color = (Color(red: (Double(default_color) * step_size)/Double(255), green: (Double(default_color) * step_size)/Double(255), blue: (Double(default_color) * step_size)/Double(255)))
                        
                        ant_colors.append(step_color) // adds that color to our color list
                    }
                    
                    dead_color = hex_string_to_color(from: "00000000") // Sets background to transparent
                    
                } else {
                    // If it's not either of the defaults
                    
                    var arg_index = 0 // Index for tracking current color in the list since there could be any number of colors on this line
                    
                    let args = line.split(separator: " ") // Splits the line into array for easy parsing
                    
                    for arg in args {
                        // Goes through each 'color'
                        
                        if arg_index == 0 {
                            // If it's the first one, it's the background color
                            
                            dead_color = hex_string_to_color(from: String(arg)) // Updates the background color to be the given color hex using the utility function
                            
                        } else {
                            // Otherwise,
                            
                            ant_colors.append(hex_string_to_color(from: String(arg))) // Add the color to the color list
                            
                        }
                        arg_index += 1 // Iterates the index
                    }
                    
                    if ant_colors.count + 1 <= rule.count {
                        // 'If statement' to check valid number of colors
                        
                        print("Color list too short") // Prints error message
                        abort_start = true // aborts launch
                        exit(1) // ends process
                    }
                }
                
            } else {
                // Once we're not on the second line anymore, each line is an ant
                
                let args = line.split(separator: " ") // Splits the ant's parameters into array
                let ant = Ant(x: Int32(args[0])!, y: Int32(args[1])!, direction: Int32(args[2])!, ruleset: rule, color: hex_string_to_color(from: String(args[3]))) // Makes an ant from the parameters
                ants.append(ant) // Adds the ant to the ant list
                
            }
            index_l += 1 // iterate index
        }
        return ants // Return the list of ants
        
    } catch {
        // If the file doesn't open correctly
        
        print("Could not open file: " + path) // Prints error message
        abort_start = true // abort launch
        exit(1) // exit process
    }
}

func hex_string_to_color(from hex_string: String) -> Color {
    // Function to take string in and output color object (RGBA)
    
    if hex_string.count == 8 { // Makes sure that the string is exactly 8 characters long, and if not error as it's not a valid color
        
        let red_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 0)..<hex_string.index(hex_string.startIndex, offsetBy: 2)]) // Gets the first two chars for the red hex
        let green_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 2)..<hex_string.index(hex_string.startIndex, offsetBy: 4)]) // Gets the first two chars for the green hex
        let blue_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 4)..<hex_string.index(hex_string.startIndex, offsetBy: 6)]) // Gets the first two chars for the blue hex
        let alpha_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 6)..<hex_string.index(hex_string.startIndex, offsetBy: 8)]) // Gets the first two chars for the alpha hex
        
        var red_val: Int // Int to hold what the red value is out of 255
        var green_val: Int // Int to hold what the green value is out of 255
        var blue_val: Int // Int to hold what the blue value is out of 255
        var alpha_val: Int // Int to hold what the alpha value is out of 255
        
        // This section converts the string into values, but does guarded
        // If any of these fail, they will trigger the elses and cause an error for an invalid color
        
        if let value = UInt8(red_hex, radix: 16) { // Converts the red value
            red_val = Int(value) // Sets the red value
            
            if let value = UInt8(green_hex, radix: 16) { // Converts the green value
                green_val = Int(value) // Sets the green value
                
                if let value = UInt8(blue_hex, radix: 16) { // Converts the blue value
                    blue_val = Int(value) // Sets the blue value
                    
                    if let value = UInt8(alpha_hex, radix: 16) { // Converts the alpha avlue
                        alpha_val = Int(value) // Sets the alpha value
                        
                        let nsColor = NSColor(calibratedRed: Double(red_val)/255.0, green: Double(green_val)/255.0, blue: Double(blue_val)/255.0, alpha: Double(alpha_val)/255.0) // Makes the color object from the values
                        let alive_color = Color(nsColor) // Converts from NSColor object to normal Color, which was done originally
                        return alive_color // Return the color
                    } else { // If the alpha value converts incorrecetly
                        print("Invalid color: " + hex_string)
                    }
                    
                } else { // If the blue value converts incorrecetly
                    print("Invalid color: " + hex_string)
                }
                
            } else { // If the green value converts incorrecetly
                print("Invalid color: " + hex_string)
            }
            
        } else { // If the red value converts incorrecetly
            print("Invalid color: " + hex_string)
        }
        
    } else { // If the string isn't 8 characters long
        abort_start = true // Abort start
        print("Invalid color: " + hex_string) // Error to console
    }
    
    return .white // If something goes wrong, just default to .white
    }

func hex_string_to_alpha(from hex_string: String) -> Int {
    // Function to just get alpha value, lighter version of hex to color
    
    if hex_string.count == 8 {
        // Checks to make sure the string is 8 characters long
        
        let alpha_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 6)..<hex_string.index(hex_string.startIndex, offsetBy: 8)]) // Gets just the alpha part of the string
        var alpha_val: Int // Variable to hold the value that gets converted
        
        if let value = UInt8(alpha_hex, radix: 16) { // Converts from string to hex
            alpha_val = Int(value) // Sets the value
            return alpha_val // Returns it
        }
        return 255 // If it fails to convert just return opaque
        
    } else { // If its not an 8 character string
        abort_start = true // Abort start
        // Don'r print error since color will do this
        return 0 // returns 0
    }
    
}

func help_print() {
    // Function to print usage help
    
    // Just printing the lines
    print("Usage: simwall [options]\n")
    print("Options:\n")
    print("  -h, --help: Show this help message\n")
    print("  -D, -d, --daemonize: Daemonize the process\n")
    print("  -dead 000000FF: Set the dead cell color (RGBA)\n")
    print("  -alive FFFFFFFF: Set the alive cell color (RGBA)\n")
    print("  -dying 808080FF: Set the dying cell color (RGBA)\n")
    print("  -fps 10.0: Set the framerate\n")
    print("  -bb: Run Brian's Brain (BB) instead of Game of Life\n")
    print("  -seeds: Run Seeds instead of Game of Life\n")
    print("  -ant <ant_params.txt>: Run Langton's Ant instead of Game of Life.\n")
    print("                         Ant parameters are optional.\n")
    print("    -ant_params.txt: Give ant parameters in a file.\n")
    print("       Format:\n")
    print("         RULESET\n")
    print("         CELL COLOR LIST (RGBA values, space delimited)\n")
    print("         X0 Y0 START_DIRECTION ANT0_COLOR\n")
    print("         X1 Y1 START_DIRECTION ANT1_COLOR\n")
    print("         etc...\n")
    print("       For direction, 0:UP, 1:RIGHT, 2:DOWN, 3:LEFT\n")
    print("       For ruleset, R:RIGHT, L:LEFT, C:CONTINUE, U:U-TURN (ex. RLLRCU)\n")
    print("       Cell color list length must be >= to ruleset length and\n")
    print("         can be set to default values by providing the keyword \"default\"\n")
    print("         or \"default_alpha\" for a transparent background\n")
    print("  -c: Draw circles instead of a squares\n")
    print("  -s 25: Set the cell size in pixels\n")
    print("  -nk: Disable keybinds\n")
    print("  -nr: No restocking if board is too empty\n")
    print("  -clear: Start with a clear board. Includes -nr\n")
    print("Example: simwall -dead FF00FFFF -alive FFFF00FF -fps 7.5\n")
}
