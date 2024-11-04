import AppKit


func update_board(layout: Array<Array<Int>>) -> Array<Array<Int>> {
    var new_layout: Array<Array<Int>> = layout
    for row in 0..<layout.count {
        for col in 0..<layout[0].count {
            new_layout[row][col] = layout[row][col] == 1 ? 0 : 1
        }
    }
    return new_layout
}
