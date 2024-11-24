/*
 FILE: main.swift
 AUTHORS: Michael Stang
 FUNCTION: Starting point for the CLI for SimWall, starts a background view and sets it to background
 */

import SwiftUI
import AppKit

class AppDelegate: NSObject, NSApplicationDelegate {
    // Class used to set up app that opens
    
    var window: NSWindow! // The window var we will set to the background

    func applicationDidFinishLaunching(_ notification: Notification) { // Function that is called when window opens

        // Creates the background object from background.swift
        let contentView = Background()

        // Creates a window that object
        window = NSWindow(
            // Sets the various properties we need in order for it funtion well as a background wallpaper
            // Things like borderless are key to make the background look good
            contentRect: NSScreen.main?.frame ?? NSRect(x: 0, y: 0, width: 800, height: 600),
            styleMask: [.borderless],
            backing: .buffered,
            defer: false
        )
        
        // We then set some important properties for the window
        // A lot of these are the recommended settings from Apple's documentation
        window.isReleasedWhenClosed = false // Used to make funactionality work properly
        window.level = .init(rawValue: Int(CGWindowLevelForKey(.desktopWindow) - 1)) // This line sets the window to the background
        window.collectionBehavior = [.canJoinAllSpaces, .stationary, .ignoresCycle] // Needed behavior for window collections
        window.backgroundColor = .clear // Sets the background of the window to be clear, as we wantt to control background through Background()
        window.contentView = NSHostingView(rootView: contentView) // Sets what the window contains, which is our Background() object from above
        window.makeKeyAndOrderFront(nil) // Another needed property
    
    }
}

// Set up and run the application

// These are the various parameters that can be controlled by flags, and their default vaulues if they're not changed

var daemonize = false
var alive_color: Color = .white
var dead_color: Color = .black
var dying_color: Color = .gray
var fps = 10.0
var simulation = "game_of_life" // The default simulation is game of life (options are game_of_life, brians_brain, seeds
var ant_rules = "RL"
var ant_color: Color = .red
var shape = "square"
var cell_size = 25
var disable_keybinds = false
var disable_restocking = false
var start_with_clear_board = false


var speed = 0.05 // Speed of the game (1/speed = FPS)
var path = "" // Path of file for loading in pattern

let argument_count = CommandLine.arguments.count // Gets the arugments
for i in 1..<argument_count { // Loops through each arugment (starting w/ 1 sincie we don't care about the ./simwall itself)
    switch CommandLine.arguments[i] { // Using a switch statemetn for flags, we don't really care if a nonflag is passed rn, we just pretend like its not there
    case "-d", "-D" : // -t is used to set the tile shape
        daemonize = true
    case "-alive":
        if i != argument_count - 1 {
            alive_color = Color(hex_string_to_color(from: CommandLine.arguments[i + 1]))
        }
    case "-dead":
        if i != argument_count - 1 {
            dead_color = Color(hex_string_to_color(from: CommandLine.arguments[i + 1]))
        }
    case "-dying":
        if i != argument_count - 1 {
            dying_color = Color(hex_string_to_color(from: CommandLine.arguments[i + 1]))
        }
    case "-fps":
        if i != argument_count - 1 {
            if let value = Double(CommandLine.arguments[i + 1]) {
                fps = value
            } else {
                fps = 10.0
            }
        }
    case "-bb":
        simulation = "brians_brain"
    case "-seeds":
        simulation = "seeds"
    case "-ant":
        simulation = "ant"
    case "-ant_rules":
        if i != argument_count - 1 {
            ant_rules = CommandLine.arguments[i + 1]
        }
    case "-c":
        shape = "circle"
    case "-s":
        if let value = Int(CommandLine.arguments[i + 1]) {
            cell_size = value
        } else {
            cell_size = 25
        }
    case "-nk":
        disable_keybinds = true
    case "-nr":
        disable_restocking = true
    case "-clear":
        start_with_clear_board = true
    case "-f": // -f indicates the user wnats a set pattern from a file rather than a random start pattern
        // -f should be followed by a path
        // the path var is used in two ways
        // 1) It tells where the file is
        // 2) It is used to check for random vs set starting pattern (via path == "")
        if i != argument_count - 1 {
            path = CommandLine.arguments[i + 1]
        }
        print("path: \(path)") // print path for debug
    case "-h":
        print("HELP GUIDE HERE")
    default: // If it's not a flag, just do nothing
        print("") // Swift requires a statement here
    }
}

// Starts up the app by setting up our NSApplication
// Then sets a delegate
// And runs the app
let app = NSApplication.shared
let delegate = AppDelegate()
app.delegate = delegate
app.run()
