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
var alive_alpha: Int = 255
var dead_alpha: Int = 255
var dying_alpha: Int = 255
var fps = 10.0
var simulation = "game_of_life" // The default simulation is game of life (options are game_of_life, brians_brain, seeds
var ant_rules = "RL"
var ants_file = ""
var shape = "square"
var cell_size = 25
var disable_keybinds = false
var disable_restocking = false
var start_with_clear_board = false
var ant_colors: [Color] = [.yellow]
var abort_start = false


var speed = 0.05 // Speed of the game (1/speed = FPS)
var path = "" // Path of file for loading in pattern

let argument_count = CommandLine.arguments.count // Gets the arugments

for i in 1..<argument_count { // Loops through each arugment (starting w/ 1 sincie we don't care about the ./simwall itself)
    switch CommandLine.arguments[i] { // Using a switch statemetn for flags, we don't really care if a nonflag is passed rn, we just pretend like its not there
    
    case "-d", "-D", "--daemonize": // if its the daemonize flag
        daemonize = true // enable daemonization, even though this doesn't do anything on MacOS because its not needed but its here for consistancy
    
    case "-alive": // set the alive cell color
        if i != argument_count - 1 { // Checks that there is another argument after for safety
            alive_color = Color(hex_string_to_color(from: CommandLine.arguments[i + 1])) // Takes the next element and converts it to a color
            alive_alpha = hex_string_to_alpha(from: CommandLine.arguments[i + 1]) // Gets the alpha of that color as well
            
        } else { // If there's no argument after, then its improperly formatted
            print("Not enough arguments for -alive") // Prints error
            abort_start = true // Aborts start
            help_print() // prints help statement
        }
        
    case "-dead": // set the dead cell color
        if i != argument_count - 1 { // Checks that there is another argument after for safety
            dead_color = Color(hex_string_to_color(from: CommandLine.arguments[i + 1])) // Takes the next element and converts it to a color/
            dead_alpha = hex_string_to_alpha(from: CommandLine.arguments[i + 1]) // Gets the alpha of that color as well
            
        } else { // If there's no argument after, then its improperly formatted
            print("Not enough arguments for -dead") // Prints error
            abort_start = true // Aborts start
            help_print() // prints help statement
        }
        
    case "-dying": // update dying cell color
        if i != argument_count - 1 { // Checks that there is another argument after for safety
            dying_color = Color(hex_string_to_color(from: CommandLine.arguments[i + 1])) // Takes the next element and converts it to a color
            dying_alpha = hex_string_to_alpha(from: CommandLine.arguments[i + 1]) // Gets the alpha of that color as well
        } else {
            print("Not enough arguments for -dying") // Prints error
            abort_start = true // Aborts start
            help_print() // prints help statement
        }
        
    case "-fps": // update fps
        if i != argument_count - 1 { // Checks that there is another argument after for safety
            
            if let value = Double(CommandLine.arguments[i + 1]) { // Converts string of next value into double
                fps = value // updates FPS value
            } else {
                print("-s requires a number as input") // Prints error
                abort_start = true // Abort the start
                help_print() // print help menu
            }
        } else {
            print("Not enough arguments for -fps") // Print error
            abort_start = true // aborts the start
            help_print() // print help menu
        }
        
    case "-bb": // sets simulation to brian's brain
        simulation = "brians_brain" // Set var to brians brain
        
    case "-seeds": // sets simulation to seeds
        simulation = "seeds" // Set var to seeds
        
    case "-ant": // sets simulation to ant and takes in details
        simulation = "ant" // Set var to langton's ant
        if i != argument_count - 1 { // Checks that there is another argument after for safety
            if !CommandLine.arguments[i + 1].starts(with: "-") { // If the next thing is a file and not a flag, take in file path
                ants_file = CommandLine.arguments[i + 1] // Updates ant_file var
            }
        }
        
    case "-c": // sets cell to circle instead of square
        shape = "circle" // updates var to circle
        
    case "-s": // update size of cells
        if i != argument_count - 1 { // Checks that there is another argument after for safety
            if let value = Int(CommandLine.arguments[i + 1]) { // Convert string to int for size
                cell_size = value // Update cell size variable
            } else { //
                print("-s requires a whole number as input") // error about that
                abort_start = true // abort start
                help_print() // print help for user
            }
        } else { // If there's no next argument
            print("Not enough arguments for -s") // print error
            abort_start = true // abort start
            help_print() // print help for user
        }
    case "-nk": // disable keybinds
        disable_keybinds = true // updates bool for keybinds
        
    case "-nr": // disable restocking
        disable_restocking = true // updates bool for restocking
        
    case "-clear": // enable starting with a clear board
        start_with_clear_board = true // updates bool for starting with clear board
        
    case "-f": // -f indicates the user wnats a set pattern from a file rather than a random start pattern
        // -f should be followed by a path
        // the path var is used in two ways
        // 1) It tells where the file is
        // 2) It is used to check for random vs set starting pattern (via path == "")
        if i != argument_count - 1 { // Checks that there is another argument after for safety
            path = CommandLine.arguments[i + 1]
        } else {
            print("Not enough arguments for -f") // prints error
            abort_start = true // aborts start
            help_print() // prints help
        }
        print("path: \(path)") // print path for debug
        
    case "-h", "--help": // Not for simulation, brings up help menu
        help_print() // prints menu
        abort_start = true // aborts the start since its just a help page
        
    default: // If it's not a flag, just do nothing
        if CommandLine.arguments[i].starts(with: "-") { // Checks if its a flag
            print("Invalid flag: " + CommandLine.arguments[i]) // If it is, yell at user about wrong flag
            abort_start = true // Abort start
            help_print() // print help for user
        }
    }
}

if (simulation == "ant" && ants_file == "") { // Checks if we're doing the ant simulation and haven't set an ant file
    
    let num_colors = ants[0].ruleset.count // Figures out rule size
    
    // Calculates colors for the rule
    for default_color in 1..<num_colors+1 {
        // Loops for each color
        
        let step_size: Double = Double(255) / Double((num_colors-1)) // Calculates step
        let step_color = (Color(red: (Double(default_color) * step_size)/Double(255), green: (Double(default_color) * step_size)/Double(255), blue: (Double(default_color) * step_size)/Double(255))) // Calculates color from index and step
        
        ant_colors.append(step_color) // Append color
    }
}

// Starts up the app by setting up our NSApplication
// Then sets a delegate
// And runs the app
if !abort_start {
    let app = NSApplication.shared
    let delegate = AppDelegate()
    app.delegate = delegate
    app.run()
}

