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

// ANT

import SwiftUI

struct Ant {
    var x: Int32
    var y: Int32
    var direction: Int32
    var ruleset: String
    var color : Color
}

func antGenNext(pattern: [Int32], width: Int, height: Int, ants: inout [Ant]) -> [Int32]? {
    guard pattern.count == width * height else {
        print("Invalid grid dimensions")
        return nil
    }
    
    // Make a mutable copy of the grid
    var newGrid = pattern

    // Iterate over each ant
    for i in 0..<ants.count {
        let ant = ants[i]
        let currentIndex = Int(ant.y) * width + Int(ant.x)
        
        // Ensure the ant is within bounds
        guard currentIndex >= 0 && currentIndex < newGrid.count else {
            print("Ant is out of bounds")
            return nil
        }

        // Apply the ruleset to determine the ant's new state
        let currentCellState = newGrid[currentIndex]
        let ruleIndex = Int(currentCellState) % ant.ruleset.count
        let currentRule = ant.ruleset[ant.ruleset.index(ant.ruleset.startIndex, offsetBy: ruleIndex)]
        
        // Update the grid state
        newGrid[currentIndex] = (currentCellState + 1) % Int32(ant.ruleset.count)

        // Update the ant's direction based on the rule
        switch currentRule {
        case "L":
            ants[i].direction = (ants[i].direction + 3) % 4 // Turn left
        case "R":
            ants[i].direction = (ants[i].direction + 1) % 4 // Turn right
        case "U":
            ants[i].direction = (ants[i].direction + 2) % 4 // Turn around
        default:
            break
        }

        // Move the ant
        switch ants[i].direction {
        case 0: ants[i].y -= 1 // Move up
        case 1: ants[i].x += 1 // Move right
        case 2: ants[i].y += 1 // Move down
        case 3: ants[i].x -= 1 // Move left
        default:
            print("Invalid direction")
        }

        // Wrap the ant's position around the grid edges
        ants[i].x = (ants[i].x + Int32(width)) % Int32(width)
        ants[i].y = (ants[i].y + Int32(height)) % Int32(height)
    }

    return newGrid
}
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


