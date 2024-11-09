import SwiftUI

struct ClickWindow: View {
    var body: some View {
        ZStack {
            // Overlay that detects taps without blocking
            Color.clear
                .contentShape(Rectangle()) // Makes the transparent area tappable
                .simultaneousGesture(
                    DragGesture(minimumDistance: 0)
                        .onEnded { value in
                            let location = value.location
                            print("Tap detected at: \(location)")
                        }
                )
        }
    }
}
