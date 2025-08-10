// Email: razcohenp@gmail.com

// Main.cpp - Main entry point for the Coup game application
// This file launches the graphical user interface and handles application lifecycle

#include "include/GameGUI.hpp"
#include <iostream>

int main() {
    try {
        // Starting COUP Game with GUI
        // Initialize the main game interface for user interaction
        // GameGUI handles all visual elements and user input processing
        coup::GameGUI gui;
        
        // Attempt to load fonts and initialize the graphics subsystem
        if (!gui.initialize()) {
            std::cout << "Error! Something is wrong." << std::endl;
            return 1; // Exit with error code if initialization fails
        }
        
        // GUI initialized successfully
        // Enter the main game loop - this blocks until user exits
        gui.run();
    }
    
    catch (const std::exception& e) {
        std::cout << "Error! Something is wrong." << std::endl;
        return 1; // Exit with error code if an exception occurs
    }
    
    std::cout << "Thanks for playing COUP!" << std::endl;
    return 0; // Successful application exit
}
