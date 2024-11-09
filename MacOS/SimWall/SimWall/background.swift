import SwiftUI
import Foundation

let GRID_WIDTH = 160
let GRID_HEIGHT = 90
let STARTING_PERCENTAGE = 30 // as int out of 100
let RESET_PERCENT = 0.05 // as double
let RANDOM_PATTERN = true

func life_remaining(layout: [[Int]], width: Int, height: Int) -> Double {
    var sum = 0
    for row in layout {
        sum += row.reduce(0, +)
    }
    return Double(sum) / Double(width * height)
}

struct Background: View {
    @State var layout: [[Int]] = cArrayToSwiftArray(pattern: golGenRandom(width: GRID_WIDTH, height: GRID_HEIGHT, percentAlive: STARTING_PERCENTAGE)!, width: GRID_WIDTH, height: GRID_HEIGHT)
    var shape: String
    var color: Color
    var simulation: String
    var speed: Double

    var body: some View {
        ZStack {
            Color.black

            // Use Canvas for efficient drawing
            Canvas { context, size in
                let cellWidth = size.width / CGFloat(GRID_WIDTH)
                let cellHeight = size.height / CGFloat(GRID_HEIGHT)

                for row in 0..<GRID_HEIGHT {
                    for column in 0..<GRID_WIDTH {
                        let cellValue = layout[row][column]
                        if cellValue != 0 {
                            let rect = CGRect(
                                x: CGFloat(column) * cellWidth,
                                y: CGFloat(row) * cellHeight,
                                width: cellWidth,
                                height: cellHeight
                            )

                            var path: Path
                            if shape == "circle" {
                                path = Path(ellipseIn: rect)
                            } else {
                                path = Path(rect)
                            }

                            let cellColor = (cellValue == 1) ? color : .pink
                            context.fill(path, with: .color(cellColor))
                        }
                    }
                }
            }
            .aspectRatio(16/9, contentMode: .fill)
            .ignoresSafeArea()
        }
        .onAppear {
            // Start a timer to update the game state every 0.01 seconds
            if RANDOM_PATTERN {
                if simulation == "seeds" {
                    layout = makeSmallBoard(width: GRID_WIDTH, height: GRID_HEIGHT)
                } else {
                    layout = cArrayToSwiftArray(pattern: golGenRandom(width: GRID_WIDTH, height: GRID_HEIGHT, percentAlive: STARTING_PERCENTAGE)!, width: GRID_WIDTH, height: GRID_HEIGHT)
                }
                
            } else {
                // read in from file
            }
            // START LOOP
            var iterations = 0
            Timer.scheduledTimer(withTimeInterval: speed, repeats: true) { _ in
                DispatchQueue.global(qos: .userInitiated).async {
                    // Generate the next state in the background
                    if simulation == "brians_brain" {
                        if let nextPattern = bbGenNext(pattern: swiftArrayToCArray(board2D: layout, width: GRID_WIDTH, height: GRID_HEIGHT), width: GRID_WIDTH, height: GRID_HEIGHT) {
                            DispatchQueue.main.async {
                                layout = cArrayToSwiftArray(pattern: nextPattern, width: GRID_WIDTH, height: GRID_HEIGHT)
                                if life_remaining(layout: layout, width: GRID_WIDTH, height: GRID_HEIGHT) <= RESET_PERCENT/3 {
                                    layout = RANDOM_PATTERN
                                        ? cArrayToSwiftArray(pattern: golGenRandom(width: GRID_WIDTH, height: GRID_HEIGHT, percentAlive: STARTING_PERCENTAGE)!, width: GRID_WIDTH, height: GRID_HEIGHT)
                                        : layout // or read from file if not random
                                }
                            }
                        }
                    } else if simulation == "game_of_life" {
                        if let nextPattern = golGenNext(pattern: swiftArrayToCArray(board2D: layout, width: GRID_WIDTH, height: GRID_HEIGHT), width: GRID_WIDTH, height: GRID_HEIGHT) {
                            DispatchQueue.main.async {
                                layout = cArrayToSwiftArray(pattern: nextPattern, width: GRID_WIDTH, height: GRID_HEIGHT)
                                if life_remaining(layout: layout, width: GRID_WIDTH, height: GRID_HEIGHT) <= RESET_PERCENT {
                                    layout = RANDOM_PATTERN
                                        ? cArrayToSwiftArray(pattern: golGenRandom(width: GRID_WIDTH, height: GRID_HEIGHT, percentAlive: STARTING_PERCENTAGE)!, width: GRID_WIDTH, height: GRID_HEIGHT)
                                        : layout // or read from file if not random
                                }
                            }
                        }
                    } else if simulation == "seeds" {
                        iterations += 1
                        if let nextPattern = seedsGenNext(pattern: swiftArrayToCArray(board2D: layout, width: GRID_WIDTH, height: GRID_HEIGHT), width: GRID_WIDTH, height: GRID_HEIGHT) {
                            DispatchQueue.main.async {
                                layout = cArrayToSwiftArray(pattern: nextPattern, width: GRID_WIDTH, height: GRID_HEIGHT)
                                if iterations >= 120 {
                                    iterations = 0
                                    layout = RANDOM_PATTERN
                                    ? makeSmallBoard(width: GRID_WIDTH, height: GRID_HEIGHT)
                                        : layout // or read from file if not random
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
