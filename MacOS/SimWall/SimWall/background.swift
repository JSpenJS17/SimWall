import SwiftUI
import Foundation

let GRID_WIDTH = 160
let GRID_HEIGHT = 90
let STARTING_PERCENTAGE = 30 // as int out of 100
let RESET_PERCENT = 0.05 // as double
let RANDOM_PATTERN = true

func life_remaininig(layout: [[Int]], width: Int, height: Int) -> Double {
    var sum = 0
    for row in layout {
        sum += row.reduce(0, +)
    }
    return Double(sum) / Double(width * height)
}

struct Tile: View {
    var color: Color
    var shape: String
    
    var body: some View {
        if shape == "circle" {
            Circle()
                .foregroundStyle(color)
                .aspectRatio(1, contentMode: .fit)
                .padding(0)
        } else if shape == "square" {
            Rectangle()
                .foregroundStyle(color)
                .aspectRatio(1, contentMode: .fit)
                .padding(0)
        }
    }
}

struct Background: View {
    @State var layout: Array<Array<Int>> = [[]]
    var shape: String
    var color: Color
    var simulation: String
    
    
    var body: some View {
        let rows = layout.count
        let columns = layout[0].count
        ZStack {
            Color.black
            
            // Rows
            VStack(spacing: 0) {
                ForEach(0..<rows, id: \.self) { row in
                    
                    // Columns
                    HStack(spacing: 0) {
                        ForEach(0..<columns, id: \.self) { column in
                            Tile(color: layout[row][column] == 0 ? .black : color, shape: shape)
                                .onTapGesture {
                                    print("Tapped on the background view!")
                                }
                        }
                    }
                    
                }
            }
            .aspectRatio(16/9, contentMode: .fill)
        }
        .onAppear {
            // Start a timer to update the game state every 0.1 seconds
            if RANDOM_PATTERN {
                layout = convertPatternTo2DArray(pattern: (golGenRandom(width:GRID_WIDTH, height:GRID_HEIGHT, percentAlive: STARTING_PERCENTAGE))!, width: GRID_WIDTH, height: GRID_HEIGHT)
            } else {
                // read in from file
            }
            // START LOOP
            Timer.scheduledTimer(withTimeInterval: 0.1, repeats: true) { _ in
                DispatchQueue.main.async {
                    if let nextPattern = golGenNext(
                        pattern: convert2DArrayToPattern(board2D: layout, width: GRID_WIDTH, height: GRID_HEIGHT),
                        width: GRID_WIDTH,
                        height: GRID_HEIGHT
                    )
                    {
                        layout = convertPatternTo2DArray(pattern: nextPattern, width: GRID_WIDTH, height: GRID_HEIGHT)
                        if (life_remaininig(layout: layout, width: GRID_WIDTH, height: GRID_HEIGHT) <= RESET_PERCENT) { // If we have too little remaining live cells
                            if RANDOM_PATTERN {
                                layout = convertPatternTo2DArray(pattern: (golGenRandom(width:GRID_WIDTH, height:GRID_HEIGHT, percentAlive: STARTING_PERCENTAGE))!, width: GRID_WIDTH, height: GRID_HEIGHT)
                            } else {
                                // read in from file
                            }
                            
                        }
                    }
                }
            }
        }
    }
}
