import SwiftUI
import AppKit

class AppDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow!

    func applicationDidFinishLaunching(_ notification: Notification) {
        // Create a 50x50 array filled with random 0s and 1s for testing

        // Create the SwiftUI view with the color array
        let contentView = Background(layout: [[1,1,1,1,1], [0,0,0,0,0], [0,1,0,0,0], [0,1,0,0,0], [0,0,0,0,0]])

        // Create an NSWindow with a transparent background
        window = NSWindow(
            contentRect: NSScreen.main?.frame ?? NSRect(x: 0, y: 0, width: 800, height: 600),
            styleMask: [.borderless],
            backing: .buffered,
            defer: false
        )
        
        window.isReleasedWhenClosed = false
        window.level = .init(rawValue: Int(CGWindowLevelForKey(.desktopWindow) - 1))
        window.collectionBehavior = [.canJoinAllSpaces, .stationary, .ignoresCycle]
        window.backgroundColor = .clear
        window.contentView = NSHostingView(rootView: contentView)
        window.makeKeyAndOrderFront(nil)
    }
}

// Set up and run the application

let app = NSApplication.shared
let delegate = AppDelegate()
app.delegate = delegate
app.run()
