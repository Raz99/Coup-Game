# Email: razcohenp@gmail.com

# Makefile for Coup Game
# This file automates the build process for the game and its components

# Compiler and flags
CXX = g++ # C++ compiler
CXXFLAGS =  -g -std=c++17 # Compiler flags: debug info, C++17 standard
INCLUDES = -Iinclude # Include directory for header files
LIBS = -lsfml-graphics -lsfml-window -lsfml-system # SFML libraries for graphics and windowing
DOCTEST_INCLUDE = -Itests # Include directory for Doctest framework

# Executable names
GUI_EXEC = coup_game # Main executable name for GUI version
EXAMPLE_EXEC = example # Main executable name for example file
TEST_EXEC = test_coup # Test executable

# Object files
MAIN_OBJS = Game.o Player.o # Main object files
ROLE_OBJS = Governor.o Spy.o Baron.o General.o Judge.o Merchant.o # Role object files
TEST_OBJS = test_game.o test_player.o test_roles.o # Test object files

# Declare targets that don't create files
.PHONY: all GUI Main test valgrind clean

# Default target builds the GUI executable
all: $(GUI_EXEC)

# GUI
# Build main GUI executable
$(GUI_EXEC): MainGUI.o GameGUI.o $(MAIN_OBJS) $(ROLE_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Build main GUI object files
MainGUI.o: MainGUI.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

GameGUI.o: src/GameGUI.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Pattern rule to build main object files from source
$(MAIN_OBJS): %.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Pattern rule to build role object files from role sources
$(ROLE_OBJS): %.o: src/roles/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the main GUI application
GUI: $(GUI_EXEC)
	./$(GUI_EXEC)

# Main - Example file
# Build example executable
$(EXAMPLE_EXEC): example.o $(MAIN_OBJS) $(ROLE_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Build example object file
example.o: example.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the example executable
Main: $(EXAMPLE_EXEC)
	./$(EXAMPLE_EXEC)

# Test
# Build and run tests
test: $(TEST_OBJS) $(MAIN_OBJS) $(ROLE_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DOCTEST_INCLUDE) -o $(TEST_EXEC) $^ $(LIBS)
	./$(TEST_EXEC)

# Pattern rule to build test object files
$(TEST_OBJS): %.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DOCTEST_INCLUDE) -c $< -o $@

# Valgrind - Memory check on example and test executables
valgrind: $(EXAMPLE_EXEC) $(TEST_EXEC)
	valgrind --leak-check=full ./$(EXAMPLE_EXEC) ./$(TEST_EXEC)

 # Clean - Remove all generated files
clean:
	rm -f $(GUI_EXEC) $(EXAMPLE_EXEC) $(TEST_EXEC) *.o
