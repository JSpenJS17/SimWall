import SwiftUI

struct Tile: View {
    var color: Color
    
    var body: some View {
        Circle()
            .foregroundStyle(color)
            .aspectRatio(1, contentMode: .fit)
            .padding(0)
    }
}

struct Background: View {
    @State var layout: Array<Array<Int>>
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
                            Tile(color: layout[row][column] == 0 ? .black : .white)
                        }
                    }
                    
                }
            }
            .aspectRatio(16/9, contentMode: .fill)
        }
        .onAppear {
                    // Start a timer to update the game state every 0.5 seconds
            layout = convertPatternTo2DArray(pattern: (golGenRandom(width:160, height:90, percentAlive: 30))!, width: 160, height: 90)
            // START LOOP
            Timer.scheduledTimer(withTimeInterval: 0.000005, repeats: true) { _ in
                DispatchQueue.main.async {
                    //layout = update_board(layout: layout)
                    //layout = convertPatternTo2DArray(pattern: (golGenRandom(width:10, height:10, percentAlive: 10))!, width: 10, height: 10)
                    if let nextPattern = golGenNext(
                                pattern: convert2DArrayToPattern(board2D: layout, width: 160, height: 90),
                                width: 160,
                                height: 90
                            ) {
                                layout = convertPatternTo2DArray(pattern: nextPattern, width: 160, height: 90)
                            }                }
            }
                }
    }
}

#Preview {
    Background(layout: [[0,0,0,0,0], [0,0,0], [0,0,0]])
}
