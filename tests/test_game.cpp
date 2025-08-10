// Email: razcohenp@gmail.com

/**
 * Comprehensive tests for the Game class
 * Tests game initialization, player management, turn progression, and victory conditions
 * Covers the core game loop mechanics of the Coup card game including:
 * - Player registration and validation (2-6 players required)
 * - Turn-based gameplay with proper cycling
 * - Winner determination when only one player remains active
 * - Game state management and error handling
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <stdexcept>
#include <algorithm>
#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include "../include/roles/Governor.hpp"
#include "../include/roles/General.hpp"
#include "../include/roles/Judge.hpp"
#include "../include/roles/Baron.hpp"
#include "../include/roles/Spy.hpp"
#include "../include/roles/Merchant.hpp"

using namespace coup;

TEST_CASE("Game Construction and Initialization") {
    SUBCASE("Default constructor creates empty game") {
        Game game;
        CHECK_FALSE(game.isGameStarted()); // Game should not be started initially
        CHECK(game.players().empty()); // No players registered yet
        CHECK_THROWS_AS(game.turn(), std::runtime_error); // Cannot get current turn before game starts
        CHECK_THROWS_AS(game.winner(), std::runtime_error); // Cannot determine winner before game starts
    }
}

TEST_CASE("Player Addition and Management") {
    Game game{}; // Create new game instance for testing
    
    SUBCASE("Adding valid players") {
        Governor p1(game, "Alice"); // Add first player with Governor role
        Spy p2(game, "Bob"); // Add second player with Spy role
        
        auto player_names = game.players(); // Get list of registered players
        CHECK(player_names.size() == 2); // Should have exactly 2 players
        CHECK(std::find(player_names.begin(), player_names.end(), "Alice") != player_names.end()); // Alice should be in the list
        CHECK(std::find(player_names.begin(), player_names.end(), "Bob") != player_names.end()); // Bob should be in the list
    }
    
    SUBCASE("Maximum player limit enforcement") {
        // Add 6 players (maximum allowed in Coup)
        Governor p1(game, "Player1"); // First player
        Spy p2(game, "Player2"); // Second player
        Baron p3(game, "Player3"); // Third player
        General p4(game, "Player4"); // Fourth player
        Judge p5(game, "Player5"); // Fifth player
        Merchant p6(game, "Player6"); // Sixth player (maximum)
        
        CHECK(game.players().size() == 6); // Should have maximum 6 players
        
        // Adding 7th player should throw exception
        CHECK_THROWS_AS(Governor p7(game, "Player7"), std::runtime_error); // Exceeds maximum player limit
    }
    
    SUBCASE("Cannot add players after game starts") {
        Merchant p1(game, "Alice"); // Add first player
        Baron p2(game, "Bob"); // Add second player (minimum required)
        
        game.startGame(); // Start the game - no more players can be added
        CHECK_THROWS_AS(General p3(game, "Charlie"), std::runtime_error); // Cannot add players after game starts
    }
}

TEST_CASE("Game Start Validation") {
    Game game; // Create game for testing start conditions
    
    SUBCASE("Cannot start with insufficient players") {
        CHECK_THROWS_AS(game.startGame(), std::runtime_error); // Need at least 2 players to start
        
        Player p1(game, "Alice"); // Add only one player
        CHECK_THROWS_AS(game.startGame(), std::runtime_error); // Still insufficient - need minimum 2 players
    }
    
    SUBCASE("Valid game start with minimum players") {
        Player p1(game, "Alice"); // First player
        Player p2(game, "Bob"); // Second player (minimum requirement met)
        
        CHECK_NOTHROW(game.startGame()); // Should start successfully with 2 players
        CHECK(game.isGameStarted()); // Game state should be started
    }
    
    SUBCASE("Valid game start with maximum players") {
        Player p1(game, "Player1"); // All 6 players for maximum capacity test
        Player p2(game, "Player2");
        Player p3(game, "Player3");
        Player p4(game, "Player4");
        Player p5(game, "Player5");
        Player p6(game, "Player6");
        
        CHECK_NOTHROW(game.startGame()); // Should start successfully with maximum players
        CHECK(game.isGameStarted()); // Game state should be started
    }
}

TEST_CASE("Turn Management") {
    Game game; // Create game for turn testing
    Player p1(game, "Alice"); // First player in turn order
    Player p2(game, "Bob"); // Second player in turn order
    Player p3(game, "Charlie"); // Third player in turn order
    
    SUBCASE("Turn progression before game starts") {
        CHECK_THROWS_AS(game.turn(), std::runtime_error); // Cannot get turn before game starts
        CHECK_THROWS_AS(game.nextTurn(), std::runtime_error); // Cannot advance turn before game starts
    }
    
    SUBCASE("Turn progression after game starts") {
        game.startGame(); // Start game to enable turn management
        
        // First player's turn
        CHECK_NOTHROW(game.turn()); // Should be able to get current turn
        CHECK(game.isPlayerTurn(&p1)); // Alice should be the current player
        CHECK_FALSE(game.isPlayerTurn(&p2)); // Bob should not be current player
        CHECK_FALSE(game.isPlayerTurn(&p3)); // Charlie should not be current player
        
        // Advance to second player
        game.nextTurn(); // Move to next player in sequence
        CHECK(game.isPlayerTurn(&p2)); // Bob should now be current player
        CHECK_FALSE(game.isPlayerTurn(&p1)); // Alice should no longer be current
        CHECK_FALSE(game.isPlayerTurn(&p3)); // Charlie should still not be current
        
        // Advance to third player
        game.nextTurn(); // Move to next player in sequence
        CHECK(game.isPlayerTurn(&p3)); // Charlie should now be current player
        CHECK_FALSE(game.isPlayerTurn(&p1)); // Alice should not be current
        CHECK_FALSE(game.isPlayerTurn(&p2)); // Bob should no longer be current
        
        // Cycle back to first player
        game.nextTurn(); // Should wrap around to first player
        CHECK(game.isPlayerTurn(&p1)); // Alice should be current again (circular turn order)
    }
    
    SUBCASE("Turn skipping eliminated players") {
        game.startGame(); // Start game for elimination testing
        
        // Eliminate second player
        p2.setActivityStatus(false); // Bob is eliminated (couped out)
        
        // Should skip from player 1 to player 3
        CHECK(game.isPlayerTurn(&p1)); // Alice is current player
        game.nextTurn(); // Should skip inactive Bob and go to Charlie
        CHECK(game.isPlayerTurn(&p3)); // Charlie should be current (Bob was skipped)
        
        // Should cycle back to player 1 (skipping inactive player 2)
        game.nextTurn(); // Should skip inactive Bob again
        CHECK(game.isPlayerTurn(&p1)); // Alice should be current again
    }
}

TEST_CASE("Winner Determination") {
    Game game; // Create game for winner testing
    Player p1(game, "Alice"); // Potential winner
    Player p2(game, "Bob"); // Player to be eliminated
    Player p3(game, "Charlie"); // Player to be eliminated
    
    SUBCASE("Winner before game starts") {
        CHECK_THROWS_AS(game.winner(), std::runtime_error); // Cannot determine winner before game starts
    }
    
    SUBCASE("No winner with multiple active players") {
        game.startGame(); // Start game with multiple active players
        CHECK_THROWS_AS(game.winner(), std::runtime_error); // Game still in progress - no winner yet
    }
    
    SUBCASE("Winner with single active player") {
        game.startGame(); // Start the game
        
        // Eliminate two players
        p2.setActivityStatus(false); // Bob is eliminated
        p3.setActivityStatus(false); // Charlie is eliminated
        
        CHECK(game.winner() == "Alice"); // Alice should be the winner (last player standing)
    }
    
    SUBCASE("No winner with all players eliminated") {
        game.startGame(); // Start the game
        
        // Eliminate all players
        p1.setActivityStatus(false); // Alice eliminated
        p2.setActivityStatus(false); // Bob eliminated  
        p3.setActivityStatus(false); // Charlie eliminated
        
        CHECK_THROWS_AS(game.winner(), std::runtime_error); // No winner if all players eliminated
    }
}

TEST_CASE("Active Players Management") {
    Game game; // Create game for active player testing
    Player p1(game, "Alice"); // Active player
    Player p2(game, "Bob"); // Player to be eliminated
    Player p3(game, "Charlie"); // Active player
    
    game.startGame(); // Start game to test active player management
    
    SUBCASE("All players initially active") {
        auto active = game.getActivePlayers(); // Get list of active players
        CHECK(active.size() == 3); // All 3 players should be active initially
        
        auto names = game.players(); // Get player names
        CHECK(names.size() == 3); // Should return all 3 player names
    }
    
    SUBCASE("Active count after eliminations") {
        p2.setActivityStatus(false); // Eliminate Bob
        
        auto active = game.getActivePlayers(); // Get updated active player list
        CHECK(active.size() == 2); // Should have 2 active players remaining
        
        auto names = game.players(); // Get active player names only
        CHECK(names.size() == 2); // Should return only active player names
        CHECK(std::find(names.begin(), names.end(), "Bob") == names.end()); // Bob should not be in the list
    }
}

TEST_CASE("Last Arrested Player Tracking") {
    Game game; // Create game for arrest tracking
    Player p1(game, "Alice"); // Potential arrester
    Player p2(game, "Bob"); // Potential arrest target
    
    SUBCASE("Initial state") {
        CHECK(game.getLastArrestedPlayer() == nullptr); // No arrests initially
    }
    
    SUBCASE("Setting and getting last arrested player") {
        game.setLastArrestedPlayer(&p1); // Set Alice as last arrested
        CHECK(game.getLastArrestedPlayer() == &p1); // Should return Alice
        
        game.setLastArrestedPlayer(&p2); // Set Bob as last arrested
        CHECK(game.getLastArrestedPlayer() == &p2); // Should return Bob
        
        game.setLastArrestedPlayer(nullptr); // Clear arrest tracking
        CHECK(game.getLastArrestedPlayer() == nullptr); // Should be null again
    }
}

TEST_CASE("Game State Validation") {
    Game game; // Create game for state validation testing
    
    SUBCASE("Operations before game starts") {
        CHECK_FALSE(game.isGameStarted()); // Game should not be started
        CHECK(game.getCurrentPlayer() == nullptr); // No current player before start
        CHECK_THROWS_AS(game.turn(), std::runtime_error); // Cannot get turn before start
        CHECK_THROWS_AS(game.nextTurn(), std::runtime_error); // Cannot advance turn before start
    }
        
    SUBCASE("Operations after game starts") {
        Player p1(game, "Alice"); // Add first player
        Player p2(game, "Bob"); // Add second player
        game.startGame(); // Start the game
        
        CHECK(game.isGameStarted()); // Verify game is started
        CHECK(game.getCurrentPlayer() != nullptr); // Should have a current player after start
        CHECK_NOTHROW(game.turn()); // Should be able to get current turn's player name
        CHECK_NOTHROW(game.nextTurn()); // Should be able to next turn
    }
}

TEST_CASE("Edge Cases and Boundary Conditions") {
    SUBCASE("Empty player name") {
        Game game;
        CHECK_THROWS_AS(Player p(game, ""), std::invalid_argument);
    }
    
    SUBCASE("Very long player name") {
        Game game;
        CHECK_THROWS_AS(Player p(game, "VeryLongPlayerNameThatExceedsLimit"), std::invalid_argument);
    }
    
    SUBCASE("Turn progression with no active players") {
        Game game;
        Player p1(game, "Alice");
        Player p2(game, "Bob");
        game.startGame();
        
        // Eliminate all players
        p1.setActivityStatus(false);
        p2.setActivityStatus(false);
        
        // Turn progression should handle empty active player list
        CHECK_THROWS_AS(game.nextTurn(), std::runtime_error);
    }
}

TEST_CASE("Memory Management and Cleanup") {
    SUBCASE("Game destructor cleanup") {
        Game* game = new Game();
        Player* p1 = new Player(*game, "Alice");
        Player* p2 = new Player(*game, "Bob");
        
        // Game destructor should clean up player objects
        delete game; // This should delete associated players
    }
    
    SUBCASE("Clear all players functionality") {
        Game game;
        Player p1(game, "Alice");
        Player p2(game, "Bob");
        
        CHECK(game.players().size() == 2);
        
        // Can clear before game starts
        game.clearAllPlayers();
        CHECK(game.players().empty());
        
        // Cannot clear after game starts
        Player p3(game, "Charlie");
        Player p4(game, "David");
        game.startGame();
        CHECK_THROWS_AS(game.clearAllPlayers(), std::runtime_error);
    }
}

TEST_CASE("Role Assignment and Factory Methods") {
    Game game;
    
    SUBCASE("Role type enumeration") {
        CHECK(game.getRoleName(RoleType::GOVERNOR) == "Governor");
        CHECK(game.getRoleName(RoleType::GENERAL) == "General");
        CHECK(game.getRoleName(RoleType::JUDGE) == "Judge");
        CHECK(game.getRoleName(RoleType::BARON) == "Baron");
        CHECK(game.getRoleName(RoleType::SPY) == "Spy");
        CHECK(game.getRoleName(RoleType::MERCHANT) == "Merchant");
    }
    
    SUBCASE("Player creation with specific roles") {
        Player* governor = game.createPlayerWithRole("Gov", RoleType::GOVERNOR);
        Player* general = game.createPlayerWithRole("Gen", RoleType::GENERAL);
        Player* judge = game.createPlayerWithRole("Judge", RoleType::JUDGE);
        
        CHECK(governor->getRoleType() == "Governor");
        CHECK(general->getRoleType() == "General");
        CHECK(judge->getRoleType() == "Judge");
    }
}