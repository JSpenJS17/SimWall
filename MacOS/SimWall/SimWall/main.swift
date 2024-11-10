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
        let contentView = Background(shape: shape, color: color, simulation: simulation, speed: speed, path: path, solid_background: solid_background, invert: invert)

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

var shape = "square" // The shape of the tiles (either "square" or "circle")
var color: Color = .white // The color of the tiles (default white)
var simulation = "game_of_life" // The default simulation is game of life (options are game_of_life, brians_brain, seeds
var speed = 0.05 // Speed of the game (1/speed = FPS)
var path = "" // Path of file for loading in pattern
var solid_background = true // If the background should be solid, or if background should be wallpaper
var invert = false // If the tiles should match background in location

let argument_count = CommandLine.arguments.count // Gets the arugments
for i in 1..<argument_count { // Loops through each arugment (starting w/ 1 sincie we don't care about the ./simwall itself)
    switch CommandLine.arguments[i] { // Using a switch statemetn for flags, we don't really care if a nonflag is passed rn, we just pretend like its not there
    case "-t": // -t is used to set the tile shape
        if i != argument_count - 1 { // Makes sure there is an argument after
            if CommandLine.arguments[i + 1] == "square" { // If so, checks if it's "square"
                shape = "square" // and sets shape to "square"
            } else if CommandLine.arguments[i + 1] == "circle" { // checks if it's circle
                shape = "circle" // and sets shape to "circle"
            } else { // if it's neither
                shape = "square"  // we just put it as a square for default
            }
            print("shape: \(shape)") // prints shape to console for debug
        }
    case "-c": // -c is used to set the color of tiles
        // Just matches the primary supported colors to their swift color typers
        // Also makes sure there is an argument after
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
            print("color: \(color)") // Prints to console for debug
        }
    case "-s": // -s is used to set the simulation that's going to be used
        // Set the simulation to be used from among game_of_life, brains_brain, and seeds
        // Default is game_of_life
        // These have larger impacts in background() than most other flags
        if i != argument_count - 1 {
            if CommandLine.arguments[i + 1] == "game_of_life" {
                simulation = "game_of_life"
            } else if CommandLine.arguments[i + 1] == "brians_brain" {
                simulation = "brians_brain"
            } else if CommandLine.arguments[i + 1] == "seeds" {
                simulation = "seeds"
            }
            print("simulation: \(simulation)") // prints for debug
        }
    case "-r": // -r sets the rate/speed of the simulation
        // Takes in an FPS that the user wants it to run at, and calculates the speed factor (1/FPS)
        // Does some checks to ensure proper results, like non-negative numbers and no div by 0
        if i != argument_count - 1 {
            if let argValue = Double(CommandLine.arguments[i + 1]) {
                if argValue > 0 {
                    speed = 1/argValue
                } else {
                    speed = 0.05
                }
            } else {
                speed = 0.05
            }
            print("speed: \(speed)") // Prints for debug
        }
    case "-f": // -f indicates the user wnats a set pattern from a file rather than a random start pattern
        // -f should be followed by a path
        // the path var is used in two ways
        // 1) It tells where the file is
        // 2) It is used to check for random vs set starting pattern (via path == "")
        if i != argument_count - 1 {
            path = CommandLine.arguments[i + 1]
        }
        print("path: \(path)") // print path for debug
    case "-nbg": // -nbg flag sets a mode to remove the black background and just show the simulated tiles
        // This is just a bool that is switched
        solid_background = false
        print("no background") // Prints for debug
    case "-i": // -i sets the "inverted" mode on, where the tiles are not rendered, and where there isn't tiles is
        // Basically makes wallpaper shine through to the live tiles
        // Just a bool, handled by Background()
        invert = true
        print("inverted mode") // prints for debug
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
