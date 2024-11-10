// converted_functions.swift

import Foundation

// SEEDS

func seedsGenNext(pattern: [Int32], width: Int, height: Int) -> [Int32]? {
    let patternSize = width * height
    var nextPatternArray: [Int32] = []
    
    pattern.withUnsafeBufferPointer { patternBuffer in
        let patternPointer = patternBuffer.baseAddress!
        // Call the C function
        if let nextPatternPointer = seeds_gen_next(patternPointer, Int32(width), Int32(height)) {
            let bufferPointer = UnsafeBufferPointer(start: nextPatternPointer, count: patternSize)
            nextPatternArray = Array(bufferPointer)
            free(nextPatternPointer) // Free the allocated memory
        } else {
            print("Failed to generate next pattern")
        }
    }
    
    return nextPatternArray.isEmpty ? nil : nextPatternArray
}

// BRIANS BRAIN

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

// GAME OF LIFE NEEDED

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
    // Call the C function
    guard let randomPatternPointer = gol_gen_random(Int32(width), Int32(height), Int32(percentAlive)) else {
        print("Failed to generate random pattern")
        return nil
    }
    
    let patternSize = width * height
    let bufferPointer = UnsafeBufferPointer(start: randomPatternPointer, count: patternSize)
    let randomPatternArray = Array(bufferPointer)
    
    // Free the allocated memory
    free(randomPatternPointer)
    
    return randomPatternArray
}

// CONVERSION FUNCTIONS

func cArrayToSwiftArray(pattern: [Int32], width: Int, height: Int) -> [[Int]] {
    var board2D = [[Int]]()
    for y in 0..<height {
        let rowStartIndex = y * width
        let rowEndIndex = rowStartIndex + width
        let rowSlice = pattern[rowStartIndex..<rowEndIndex]
        let row = rowSlice.map { Int($0) }
        board2D.append(row)
    }
    return board2D
}

func swiftArrayToCArray(board2D: [[Int]], width: Int, height: Int) -> [Int32] {
    var pattern = [Int32](repeating: 0, count: width * height)
    for y in 0..<height {
        for x in 0..<width {
            let index = y * width + x
            pattern[index] = Int32(board2D[y][x])
        }
    }
    return pattern
}

func makeSmallBoard(width: Int, height: Int) -> [[Int]] {
    var board: [[Int]] = []
    for row in 0..<height {
        board.append([])
        for col in 0..<width {
            if (row >= ((height/2) - 3) && row <= ((height/2) + 3)) && (col >= ((width/2) - 3) && col <= ((width/2) + 3)) {
                board[row].append(Int.random(in: 0...1))
            } else {
                board[row].append(0)
            }
        }
    }
    return board
}

func makeEmptyBoard(width: Int, height: Int) -> [[Int]] {
    var board: [[Int]] = []
    for row in 0..<height {
        board.append([])
        for col in 0..<width {
            board[row].append(0)
        }
    }
    return board
}

func file_to_board(path: String, width: Int, height: Int) -> [[Int]] {
    var board: [[Int]] = []
    let read_in = read_in_file(path: path)
    let patternHeight = read_in.count
    let patternWidth = read_in[0].count
    let rowStart = (height - patternHeight) / 2
    let colStart = (width - patternWidth) / 2

    for row in 0..<height {
        var boardRow: [Int] = []
        for col in 0..<width {
            if row >= rowStart && row < rowStart + patternHeight && col >= colStart && col < colStart + patternWidth {
                boardRow.append(read_in[row - rowStart][col - colStart])
            } else {
                boardRow.append(0)
            }
        }
        board.append(boardRow)
    }
    return board
}

func read_in_file(path: String) -> [[Int]] {
    var grid: [[Int]] = []
    
    do {
        // Read the file content as a single string
        let content = try String(contentsOfFile: path)
        
        // Split the content by lines
        let lines = content.split(separator: "\n")
        
        for line in lines {
            // Convert each character to an Int by iterating over the characters in the line
            var row: [Int] = []
            for char in line {
                if let number = Int(String(char)) {
                    row.append(number)
                }
            }
            grid.append(row)
        }
    } catch {
        print("Error reading file: \(error)")
    }
    return grid
}
