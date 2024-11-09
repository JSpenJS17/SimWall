import SwiftUI
import AppKit

class AppDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow!
    var clickWindow: NSWindow!

    func applicationDidFinishLaunching(_ notification: Notification) {
        // Create a 50x50 array filled with random 0s and 1s for testing

        // Create the SwiftUI view with the color array
        let contentView = Background(shape: shape, color: color, simulation: simulation)

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
        
        let clickWindowView = ClickWindow()
        
        clickWindow = NSWindow(
            contentRect: NSScreen.main?.frame ?? NSRect(x: 0, y: 0, width: 800, height: 600),
            styleMask: [.borderless],
            backing: .buffered,
            defer: false
        )
        clickWindow.isReleasedWhenClosed = false
        clickWindow.level = .init(rawValue: Int(CGWindowLevelForKey(.desktopWindow) + 100))
        clickWindow.collectionBehavior = [.canJoinAllSpaces, .stationary, .ignoresCycle]
        clickWindow.backgroundColor = .clear
        clickWindow.contentView = NSHostingView(rootView: clickWindowView)
        clickWindow.makeKeyAndOrderFront(nil)
    }
}

// Set up and run the application

var shape = "square"
var color: Color = .white
var simulation = "game_of_life"

let argument_count = CommandLine.arguments.count
for i in 1..<argument_count {
    switch CommandLine.arguments[i] {
    case "-t":
        if i != argument_count - 1 {
            if CommandLine.arguments[i + 1] == "square" {
                shape = "square"
            } else if CommandLine.arguments[i + 1] == "circle" {
                shape = "circle"
            } else {
                shape = "square"
            }
            print("shape: \(shape)")
        }
    case "-c":
        if i != argument_count - 1 {
            if CommandLine.arguments[i + 1] == "white" {
                color = .white
            } else if CommandLine.arguments[i + 1] == "red" {
                color = .red
            }  else if CommandLine.arguments[i + 1] == "orange" {
                color = .orange
            }  else if CommandLine.arguments[i + 1] == "yellow" {
                color = .yellow
            }  else if CommandLine.arguments[i + 1] == "green" {
                color = .green
            }  else if CommandLine.arguments[i + 1] == "blue" {
                color = .blue
            }  else if CommandLine.arguments[i + 1] == "purple" {
                color = .purple
            }  else if CommandLine.arguments[i + 1] == "gray" {
                color = .gray
            }  else if CommandLine.arguments[i + 1] == "black" {
                color = .black
            }
            print("color: \(color)")
        }
    case "-s":
        if i != argument_count - 1 {
            if CommandLine.arguments[i + 1] == "game_of_life" {
                simulation = "game_of_life"
            } else if CommandLine.arguments[i + 1] == "brians_brain" {
                simulation = "brians_brain"
            }
            print("simulation: \(simulation)")
        }

    default:
        print("nonvalid argument")
    }
}


let app = NSApplication.shared
let delegate = AppDelegate()
app.delegate = delegate
app.run()
