// converted_functions.swift

import Foundation

func readStartPattern(filename: String, maxWidth: Int, maxHeight: Int) -> [Int32]? {
    // Convert Swift String to a C string
    guard let cFilename = filename.cString(using: .utf8) else {
        print("Failed to convert filename to C string")
        return nil
    }
    
    // Call the C function
    guard let patternPointer = read_start_pattern(UnsafeMutablePointer(mutating: cFilename), Int32(maxWidth), Int32(maxHeight)) else {
        print("Failed to read start pattern")
        return nil
    }
    
    let patternSize = maxWidth * maxHeight
    let bufferPointer = UnsafeBufferPointer(start: patternPointer, count: patternSize)
    let patternArray = Array(bufferPointer)
    
    // Free the allocated memory
    free(patternPointer)
    
    return patternArray
}

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

func golAddLife(pattern: inout [Int32], width: Int, height: Int, percentAlive: Int) {
    pattern.withUnsafeMutableBufferPointer { buffer in
        let patternPointer = buffer.baseAddress!
        // Call the C function
        gol_add_life(patternPointer, Int32(width), Int32(height), Int32(percentAlive))
    }
}

func golCountLiveNeighbors(pattern: [Int32], width: Int, height: Int, cellIndex: Int) -> Int32 {
    var liveNeighbors: Int32 = 0
    pattern.withUnsafeBufferPointer { buffer in
        let patternPointer = buffer.baseAddress!
        // Call the C function
        liveNeighbors = gol_count_live_neighbors(patternPointer, Int32(width), Int32(height), Int32(cellIndex))
    }
    return liveNeighbors
}

func printPattern(pattern: [Int32], width: Int, height: Int) {
    pattern.withUnsafeBufferPointer { buffer in
        let patternPointer = buffer.baseAddress!
        // Call the C function
        print_pattern(patternPointer, Int32(width), Int32(height))
    }
}

func convertPatternTo2DArray(pattern: [Int32], width: Int, height: Int) -> [[Int]] {
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

func convert2DArrayToPattern(board2D: [[Int]], width: Int, height: Int) -> [Int32] {
    var pattern = [Int32](repeating: 0, count: width * height)
    for y in 0..<height {
        for x in 0..<width {
            let index = y * width + x
            pattern[index] = Int32(board2D[y][x])
        }
    }
    return pattern
}
