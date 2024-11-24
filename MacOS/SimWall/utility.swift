//
//  utility.swift
//  SimWall
//
//  Created by Michael Stang on 11/19/24.
//

import SwiftUI
import AppKit

func hex_string_to_color(from hex_string: String) -> Color {
    // Remove any leading '#' and whitespace characters
    let red_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 0)..<hex_string.index(hex_string.startIndex, offsetBy: 2)])
    let green_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 2)..<hex_string.index(hex_string.startIndex, offsetBy: 4)])
    let blue_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 4)..<hex_string.index(hex_string.startIndex, offsetBy: 6)])
    let alpha_hex = String(hex_string[hex_string.index(hex_string.startIndex, offsetBy: 6)..<hex_string.index(hex_string.startIndex, offsetBy: 8)])
    
    var red_val: Int
    var green_val: Int
    var blue_val: Int
    var alpha_val: Int
    
    if let value = UInt8(red_hex, radix: 16) {
        red_val = Int(value)
        if let value = UInt8(green_hex, radix: 16) {
            green_val = Int(value)
            if let value = UInt8(blue_hex, radix: 16) {
                blue_val = Int(value)
                if let value = UInt8(alpha_hex, radix: 16) {
                    alpha_val = Int(value)
                    return Color(red: Double(red_val)/255.0, green: Double(green_val)/255.0, blue: Double(blue_val)/255.0, opacity: Double(alpha_val)/255.0)
                }
            }
            
        }
    }
    return .white
    }
