/*
 FILE: converted_function.swift
 AUTHORS: Michael Stang
 FUNCTION: Contains functions needed for operation
 */


import Foundation

// GENERATION FUNCTIONS

/*
 These first three functions are the functions used to generate the next phase of life for each simulation
 They are not written here and instead do a bunch of swift weirdness to convert things to the correct types
 They are ALL the same, except for the function they call, so I've commented the seeds one only
 These are using the functions from the .c files in this folder and their associated headers
 A lot of this is kinda janky, but it works and allows for easy use of same backend across C and Swift
*/

// SEEDS

func seedsGenNext(pattern: [Int32], width: Int, height: Int) -> [Int32]? {
    // Generates the next stage of life givena  current state and height/width
    let patternSize = width * height // Calculates the pattern's size
    var nextPatternArray: [Int32] = [] // Gets a variable ready to hold next pattern
    
    pattern.withUnsafeBufferPointer { patternBuffer in // Sets up pointers to be used with the C functions
        let patternPointer = patternBuffer.baseAddress! // Gets the address of the patternBuffer
        // Call the C function
        if let nextPatternPointer = seeds_gen_next(patternPointer, Int32(width), Int32(height)) { // Actually does the function call, but does a lot of the safety things swift wants since its running C code
            let bufferPointer = UnsafeBufferPointer(start: nextPatternPointer, count: patternSize) // Sets the buffer pointer
            nextPatternArray = Array(bufferPointer) // Gets the output and puts it into an Array
            free(nextPatternPointer) // Free the allocated memory
        } else {
            print("Failed to generate next pattern")
        }
    }
    
    return nextPatternArray.isEmpty ? nil : nextPatternArray // Returns the value if it had no errors
}

// BRIANS BRAIN (same as seeds, just bb_gen_next)

func bbGenNext(pattern: [Int32], width: Int, height: Int) -> [Int32]? {
    let patternSize = width * height
    var nextPatternArray: [Int32] = []
    
    pattern.withUnsafeBufferPointer { patternBuffer in
        let patternPointer = patternBuffer.baseAddress!
        // Call the C function
        if let nextPatternPointer = bb_gen_next(patternPointer, Int32(width), Int32(height)) {
            let bufferPointer = UnsafeBufferPointer(start: nextPatternPointer, count: patternSize)
            nextPatternArray = Array(bufferPointer)
            free(nextPatternPointer) // Free the allocated memory
        } else {
            print("Failed to generate next pattern")
        }
    }
    
    return nextPatternArray.isEmpty ? nil : nextPatternArray
}

// GAME OF LIFE (same as seeds, just gol_gen_next)

func golGenNext(pattern: [Int32], width: Int, height: Int) -> [Int32]? {
    let patternSize = width * height
    var nextPatternArray: [Int32] = []
    
    pattern.withUnsafeBufferPointer { patternBuffer in
        let patternPointer = patternBuffer.baseAddress!
        // Call the C function
        if let nextPatternPointer = gol_gen_next(patternPointer, Int32(width), Int32(height)) {
            let bufferPointer = UnsafeBufferPointer(start: nextPatternPointer, count: patternSize)
            nextPatternArray = Array(bufferPointer)
            free(nextPatternPointer) // Free the allocated memory
        } else {
            print("Failed to generate next pattern")
        }
    }
    
    return nextPatternArray.isEmpty ? nil : nextPatternArray
}

// GENERAL RANDOM

func golGenRandom(width: Int, height: Int, percentAlive: Int) -> [Int32]? {
    // Call the C function to generate random grid
    guard let randomPatternPointer = gol_gen_random(Int32(width), Int32(height), Int32(percentAlive)) else { // Same concept as above using pointers and swift safety
        print("Failed to generate random pattern") // Prints for debug
        return nil // Doesn't return anything if it fails
    }
    
    // Same calculations as above functions to get grid
    let patternSize = width * height
    let bufferPointer = UnsafeBufferPointer(start: randomPatternPointer, count: patternSize)
    let randomPatternArray = Array(bufferPointer)
    
    free(randomPatternPointer) // Free the allocated memory
    
    return randomPatternArray // Returns our array that we generated
}

// CONVERSION FUNCTIONS

func cArrayToSwiftArray(pattern: [Int32], width: Int, height: Int) -> [[Int]] {
    // Function to take c Array objects we get from C functions into Swift [[Int]] objects
    var board2D = [[Int]]() // We set a board to write into
    for y in 0..<height { // We go through the C board
        let rowStartIndex = y * width // We have to cacluate starting and end indexes for each row
        let rowEndIndex = rowStartIndex + width // Calculates end
        let rowSlice = pattern[rowStartIndex..<rowEndIndex] // Creates the slice from start to ed
        let row = rowSlice.map { Int($0) } // Maps each value to a Swift Int
        board2D.append(row) // Appends that row we made to our Swift [[Int]]
    }
    return board2D // Then returns that board
}

func swiftArrayToCArray(board2D: [[Int]], width: Int, height: Int) -> [Int32] {
    // Function to take Swift [[Int]] and turn it into C [Int32] array
    var pattern = [Int32](repeating: 0, count: width * height) // Create blank [Int32]
    for y in 0..<height { // Loop through each row
        for x in 0..<width { // Loop through each column
            let index = y * width + x // Find the index
            pattern[index] = Int32(board2D[y][x]) // Set the index to the right value
        }
    }
    return pattern // Return the [Int32] object
}

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
        print("Error reading file: \(error)")
    }
    return grid // Return our newly made grid
}
