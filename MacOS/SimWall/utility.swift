//
//  utility.swift
//  SimWall
//
//  Created by Michael Stang on 11/19/24.
//

import SwiftUI
import AppKit

// HELPER FUNCTIONS

func makeSmallBoard(width: Int, height: Int) -> [[Int]] {
    // Function to make a board that only contains random values in the inner 6x6
    
    var board: [[Int]] = [] // Makes a blank board
    for row in 0..<height { // Iterates for each row we should end up
        board.append([]) // Appends a new row to the grid for the row
        for col in 0..<width { // Goes through each column that should contain a value in the row
            if (row >= ((height/2) - 3) && row <= ((height/2) + 3)) && (col >= ((width/2) - 3) && col <= ((width/2) + 3)) { // If we're in the inner 6x6
                board[row].append(Int.random(in: 0...1)) // Add either a 1 or 0
            } else { // Otherwise
                board[row].append(0) // Make it blank
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
        for col in 0..<width { // Goes for each value that it should have in each row
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
    
    do { // In a do in case of errors
        
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
        print("Could not open file: " + path)
        abort_start = true
        exit(1)
    }
    return grid // Return our newly made grid
}

func file_to_ant(path: String) -> [Ant] {
    do {
        let content = try String(contentsOfFile: path)
        let lines = content.split(separator: "\n")
        
        var ants: [Ant] = []
        var rule = "LR"
        
        var index_l = 0
        for line in lines {
            if index_l == 0 {
                rule = String(line)
            } else if index_l == 1 {
                // COLORS
                if String(line) == "default" {
                    let num_colors = rule.count
                    for default_color in 1..<num_colors+1 {
                        let step_size: Double = Double(255) / Double((num_colors-1))
                        let step_color = (Color(red: (Double(default_color) * step_size)/Double(255), green: (Double(default_color) * step_size)/Double(255), blue: (Double(default_color) * step_size)/Double(255)))
                        ant_colors.append(step_color)
                    }
                } else if line == "default_alpha" {
                    let num_colors = rule.count
                    for default_color in 1..<num_colors+1 {
                        let step_size: Double = Double(255) / Double((num_colors-1))
                        let step_color = (Color(red: (Double(default_color) * step_size)/Double(255), green: (Double(default_color) * step_size)/Double(255), blue: (Double(default_color) * step_size)/Double(255)))
                        ant_colors.append(step_color)
                    }
                    dead_color = hex_string_to_color(from: "00000000")
                } else {
                    var arg_index = 0
                    let args = line.split(separator: " ")
                    for arg in args {
                        if arg_index == 0 {
                            dead_color = hex_string_to_color(from: String(arg))
                        } else {
                            ant_colors.append(hex_string_to_color(from: String(arg)))
                        }
                        arg_index += 1
                    }
                    if ant_colors.count + 1 <= rule.count {
                        print("Color list too short")
                        abort_start = true
                        exit(1)
                    }
                }
            } else {
                let args = line.split(separator: " ")
                let ant = Ant(x: Int32(args[0])!, y: Int32(args[1])!, direction: Int32(args[2])!, ruleset: rule, color: hex_string_to_color(from: String(args[3])))
                ants.append(ant)
            }
            index_l += 1
        }
        return ants
    } catch {
        print("Could not open file: " + path)
        abort_start = true
        exit(1)
    }
    return []
}

func hex_string_to_color(from hex_string: String) -> Color {
    // Get the string
    if hex_string.count == 8 {
        let red_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 0)..<hex_string.index(hex_string.startIndex, offsetBy: 2)])
        let green_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 2)..<hex_string.index(hex_string.startIndex, offsetBy: 4)])
        let blue_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 4)..<hex_string.index(hex_string.startIndex, offsetBy: 6)])
        let alpha_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 6)..<hex_string.index(hex_string.startIndex, offsetBy: 8)])
        
        var red_val: Int
        var green_val: Int
        var blue_val: Int
        var alpha_val: Int
        
        if let value = UInt8(red_hex, radix: 16) {
            red_val = Int(value)
            if let value = UInt8(green_hex, radix: 16) {
                green_val = Int(value)
                if let value = UInt8(blue_hex, radix: 16) {
                    blue_val = Int(value)
                    if let value = UInt8(alpha_hex, radix: 16) {
                        alpha_val = Int(value)
                        let nsColor = NSColor(calibratedRed: Double(red_val)/255.0, green: Double(green_val)/255.0, blue: Double(blue_val)/255.0, alpha: Double(alpha_val)/255.0)
                        let alive_color = Color(nsColor)
                        return alive_color
                    } else {
                        print("Invalid color: " + hex_string)
                    }
                } else {
                    print("Invalid color: " + hex_string)
                }
                
            } else {
                print("Invalid color: " + hex_string)
            }
        } else {
            print("Invalid color: " + hex_string)
        }
    } else {
        abort_start = true
        print("Invalid color: " + hex_string)
    }
    return .white
    }

func hex_string_to_alpha(from hex_string: String) -> Int {
    if hex_string.count == 8 {
        let alpha_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 6)..<hex_string.index(hex_string.startIndex, offsetBy: 8)])
        var alpha_val: Int
        if let value = UInt8(alpha_hex, radix: 16) {
            alpha_val = Int(value)
            return alpha_val
        }
        return 255
    } else{
        abort_start = true
        return 0
    }
    
}

func help_print() {
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
    print("       For direction, 0:UP, 1:RIGHT, 2:DOWN, 3:LEFT\n") // might change to strings that are parsed to the enum later
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
