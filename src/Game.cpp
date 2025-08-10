// Email: razcohenp@gmail.com

 // Game.cpp - Implementation of the Game class
 // Handles game flow, player management, turn progression and victory conditions

#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include "../include/roles/General.hpp"
#include "../include/roles/Judge.hpp"
#include "../include/roles/Merchant.hpp"
#include "../include/roles/Governor.hpp"
#include "../include/roles/Baron.hpp"
#include "../include/roles/Spy.hpp"

#include <iostream> // For console output operations
#include <stdexcept> // For exception handling
#include <algorithm> // For STL algorithms like std::find
#include <chrono> // For high-precision time-based random seeding

namespace coup {
    /**
     * Default constructor initializes an empty game state.
     * Sets up random number generation and prepares for player addition.
     */
    Game::Game() : current_player_index(0), game_started(false), last_arrested_player(nullptr) {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count(); // Get current time for randomization
        random_generator.seed(seed); // Initialize random generator with unique seed
    }

    /**
     * Copy constructor creates a deep copy of the game state.
     * Recreates all players with the same roles and states.
     */
    Game::Game(const Game& other)
    : current_player_index(other.current_player_index), game_started(other.game_started),
    last_arrested_player(nullptr), // Will be set after copying players
    random_generator(other.random_generator) {
        // Deep copy all players
        for (Player* player : other.players_list) {
            Player* new_player = nullptr;
            
            // Create appropriate derived class based on role type
            std::string role_type = player->getRoleType();
            
            if (role_type == "General") {
                new_player = new General(*this, player->getName());
            }
            else if (role_type == "Judge") {
                new_player = new Judge(*this, player->getName());
            }
            else if (role_type == "Merchant") {
                new_player = new Merchant(*this, player->getName());
            }
            else if (role_type == "Governor") {
                new_player = new Governor(*this, player->getName());
            }
            else if (role_type == "Baron") {
                new_player = new Baron(*this, player->getName());
            }
            else if (role_type == "Spy") {
                new_player = new Spy(*this, player->getName());
            }
            else {
                new_player = new Player(*this, player->getName());
            }
            
            // Copy state (but not game reference or name)
            *new_player = *player;
            players_list.push_back(new_player);
        }
        
        // Update last_arrested_player reference if it exists
        if (other.last_arrested_player) {
            size_t index = std::find(other.players_list.begin(), other.players_list.end(), 
                                    other.last_arrested_player) - other.players_list.begin();
            if (index < players_list.size()) {
                last_arrested_player = players_list[index];
            }
        }
    }
    
    /**
     * Destructor performs cleanup of game resources.
     * Clears player list to ensure proper memory management.
     */
    Game::~Game() {
        players_list.clear(); // Remove all player references from the game and automatically deletes them if they were dynamically allocated
    }

    /**
     * Copy assignment operator replaces current game with copy of other.
     * Properly cleans up existing resources before copying.
     */
    Game& Game::operator=(const Game& other) {
        if (this == &other) { // Handle self-assignment
            return *this;
        }
        
        // Clean up existing players
        for (Player* player : players_list) {
            delete player;
        }
        players_list.clear();
        
        // Copy basic state
        current_player_index = other.current_player_index;
        game_started = other.game_started;
        last_arrested_player = nullptr;
        random_generator = other.random_generator;
        
        // Deep copy all players
        for (Player* player : other.players_list) {
            Player* new_player = nullptr;
            
            std::string role_type = player->getRoleType();
            
            if (role_type == "General") {
                new_player = new General(*this, player->getName());
            }
            else if (role_type == "Judge") {
                new_player = new Judge(*this, player->getName());
            }
            else if (role_type == "Merchant") {
                new_player = new Merchant(*this, player->getName());
            }
            else if (role_type == "Governor") {
                new_player = new Governor(*this, player->getName());
            }
            else if (role_type == "Baron") {
                new_player = new Baron(*this, player->getName());
            }
            else if (role_type == "Spy") {
                new_player = new Spy(*this, player->getName());
            }
            else {
                new_player = new Player(*this, player->getName());
            }
            
            *new_player = *player;
            players_list.push_back(new_player);
        }
        
        // Update last_arrested_player reference
        if (other.last_arrested_player) {
            size_t index = std::find(other.players_list.begin(), other.players_list.end(), 
                                    other.last_arrested_player) - other.players_list.begin();
            if (index < players_list.size()) {
                last_arrested_player = players_list[index];
            }
        }
        
        return *this;
    }

    /**
     * Displays the name of the current player whose turn it is.
     * Provides console output for turn identification.
     */
    void Game::turn() const {
        if (!game_started) { // Verify game has been properly initialized
            throw std::runtime_error("Game has not started yet");
        }

        if (players_list.empty()) { // Ensure at least one player exists
            throw std::runtime_error("No players in game");
        }
        
        std::cout << players_list[current_player_index]->getName() << std::endl; // Output current player's name
    }
    
    /**
     * Generates a list of names of all currently active players.
     * Excludes players who have been eliminated from the game.
     */
    std::vector<std::string> Game::players() const {
        std::vector<std::string> names; // Container for active player names

        for (const Player* player : players_list) { // Iterate through all registered players
            if (player->isActive()) { // Check if player is still in the game
                names.push_back(player->getName()); // Add active player to result list
            }
        }
        
        return names; // Return collection of active player names
    }
    
    /**
     * Determines and returns the name of the winning player.
     * Only succeeds when exactly one player remains active.
     */
    std::string Game::winner() const {
        if (!game_started) { // Ensure game has been properly started
            throw std::runtime_error("Game has not started yet");
        }

        int active_count = 0; // Counter for remaining active players
        std::string winner_name; // Storage for the winner's name
        
        for (const Player* player : players_list) { // Count active players and identify winner
            if (player->isActive()) {
                active_count++; // Increment active player count
                winner_name = player->getName(); // Store potential winner's name
            }
        }
        
        if (active_count > 1) { // Game continues if multiple players remain
            throw std::runtime_error("Game is still active");
        }
        
        if (active_count == 0) { // Safety check for impossible state
            throw std::runtime_error("No active players found");
        }
        
        return winner_name; // Return the sole remaining player's name
    }
    
    /**
     * Adds a new player to the game during setup phase.
     * Enforces player count limits and game state restrictions.
     */
    void Game::addPlayer(Player* player) {
        if (game_started) { // Prevent player addition after game begins
            throw std::runtime_error("Cannot add players after game has started");
        }

        if (players_list.size() >= 6) { // Enforce maximum player limit
            throw std::runtime_error("Maximum 6 players allowed");
        }

                players_list.push_back(player); // Add the new player to the game roster
    }
    
    /**
     * Advances the game to the next player's turn.
     * Handles end-of-turn cleanup and special role abilities.
     */
    void Game::nextTurn() {
        if (!game_started) { // Ensure game is in progress
            throw std::runtime_error("Game has not started yet");
        }

        if (players().empty()) { // Verify active players exist
            throw std::runtime_error("No players in the game");
        }

        if(players_list[current_player_index]->isSanctioned()) { //  sanctions at turn end
            players_list[current_player_index]->setSanctionStatus(false);
        }

        if(!players_list[current_player_index]->isArrestAvailable()) { // Restore arrest availability
            players_list[current_player_index]->setArrestAvailability(true);
        }

        if(players_list[current_player_index]->isBribeUsed()) { // Reset bribe usage flag
            players_list[current_player_index]->resetBribeUsed();
        }

        int old_player_index = current_player_index; // Store current position for loop detection
        current_player_index = (current_player_index + 1) % players_list.size(); // Advance to next player index
        Player* next_player = players_list[current_player_index]; // Get reference to next player

        while(!next_player->isActive()) { // Skip eliminated players
            current_player_index = (current_player_index + 1) % players_list.size();

            if(current_player_index == old_player_index) { // Detect full loop through player list
                int active_count = 0; // Count remaining active players
                bool has_active_general_with_coins = false; // Check for coup-blocking General
                for (Player* player : players_list) {
                    if (player->isActive()) {
                        active_count++;
                        if (player->getRoleType() == "General" && player->coins() >= 5) { // General with blocking capability
                            has_active_general_with_coins = true;
                        }
                    }
                }
                
                if (active_count == 2 && has_active_general_with_coins) { // Special case: General can prevent game end
                    for (size_t i = 0; i < players_list.size(); i++) { // Find first active player
                        if (players_list[i]->isActive()) {
                            current_player_index = i; // Set as current player
                            break;
                        }
                    }
                    return; // Allow GUI to handle coup blocking decision
                }
                
                return; // End turn advancement if no valid players remain
            }

            next_player = players_list[current_player_index]; // Update next player reference
        }

        if(next_player->getRoleType() == "Merchant") { // Handle Merchant's turn-start bonus
            if (next_player->coins() >= 3) { // Merchant gains coin if wealthy enough
                next_player->addCoins(1);
            }
        }

        if(next_player->usedTaxLastAction()) { // Clear tax action tracking
            next_player->resetUsedTaxLastAction();
        }

        for(Player* player : players_list) { // Clear coup tracking for expired actions
            if (player->getCoupedBy() == next_player) {
                player->resetCoupedBy(); // Remove coup reference when window expires
            }
        }
    }
    
    /**
     * Verifies if the specified player is currently allowed to act.
     * Used for turn validation and action authorization.
     */
    bool Game::isPlayerTurn(const Player* player) const {
        if (!game_started) { // Ensure game is running
            throw std::runtime_error("Game has not started yet");
        }

        // If no players in the game
        if (players_list.empty()) {
            return false;
        }

         // Ensure it's the current player's turn
        return players_list[current_player_index] == player;
    }

    // Start the game - checks player count requirements
    void Game::startGame() {
        // Check minimum players
        if (players_list.size() < 2) {
            throw std::runtime_error("Need at least 2 players to start!");
        }
        
        // Check maximum players
        if (players_list.size() > 6) {
            throw std::runtime_error("Maximum 6 players allowed!");
        }
        
        game_started = true;
        // current_player_index = 0; // Start with first player
    }
    
    // Check if game is started
    bool Game::isGameStarted() const {
        return game_started;
    }
    
    // Get current player
    Player* Game::getCurrentPlayer() const {
        if (!game_started || players_list.empty()) {
            return nullptr;
        }
        return players_list[current_player_index];
    }
    
    // Get all players in the game
    std::vector<Player*> Game::getAllPlayers() const {
        return players_list;
    }
    
    // Get active players
    std::vector<Player*> Game::getActivePlayers() const {
        std::vector<Player*> activePlayers;
        for (Player* player : players_list) {
            if (player->isActive()) {
                activePlayers.push_back(player);
            }
        }
        return activePlayers;
    }
    
    // Track last arrested player
    void Game::setLastArrestedPlayer(Player* player) {
        last_arrested_player = player;
    }
    
    Player* Game::getLastArrestedPlayer() const {
        return last_arrested_player;
    }
    
    // Clear all players from the game (only allowed when game not started)
    void Game::clearAllPlayers() {
        // Only allow clearing players if game hasn't started
        if (game_started) {
            throw std::runtime_error("Cannot clear players after game has started");
        }
        
        // Clear the list
        players_list.clear();
        current_player_index = 0;
        last_arrested_player = nullptr;
    }
    
    // Remove a specific player from the game (only allowed when game not started)
    void Game::removePlayer(Player* player) {
        // Only allow removing players if game hasn't started
        if (game_started) {
            throw std::runtime_error("Cannot remove players after game has started");
        }
        
        // Find and remove the player
        auto it = std::find(players_list.begin(), players_list.end(), player);
        if (it != players_list.end()) {
            delete *it; // Delete the player object
            players_list.erase(it);
            
            // Reset indices if needed
            if (current_player_index >= players_list.size()) {
                current_player_index = 0;
            }
        }
    }

    // Check if a General can block coup to prevent game from ending
    bool Game::canGeneralPreventGameEnd() const {
        if (!game_started) return false;
        
        int active_count = 0;
        bool has_active_general_with_coins = false;
        
        for (Player* player : players_list) {
            if (player->isActive()) {
                active_count++;
                if (player->getRoleType() == "General" && player->coins() >= 5) {
                    has_active_general_with_coins = true;
                }
            }
        }
        
        return (active_count == 2 && has_active_general_with_coins);
    }
    
    // Methods for Role Assignment
    // Assign roles to existing players without recreating them
    void Game::assignRolesToExistingPlayers() {
        // Check if there are players to assign roles to
        if (players_list.empty()) {
            throw std::runtime_error("No players in game to assign roles");
        }
        
        // Create list of available roles (can repeat for multiple players)
        std::vector<RoleType> available_roles = {
            RoleType::GOVERNOR,
            RoleType::SPY,
            RoleType::BARON,
            RoleType::GENERAL,
            RoleType::JUDGE, 
            RoleType::MERCHANT
        };
        
        // Store existing players and their names
        std::vector<std::pair<std::string, Player*>> player_data;
        for (Player* player : players_list) {
            player_data.push_back({player->getName(), player});
        }
        
        // Clear the list temporarily
        players_list.clear();
        
        // Create new players with roles and delete the old ones
        for (const auto& data : player_data) {
            // Pick random role from available roles
            RoleType assigned_role = available_roles[random_generator() % available_roles.size()];
            
            // Delete the old player and create new one with role
            delete data.second;
            Player* player = createPlayerWithRole(data.first, assigned_role);
            players_list.push_back(player);
        }
        
        // Reset game state
        current_player_index = 0;
        last_arrested_player = nullptr;
    }
    
    // Get role name as string for display
    std::string Game::getRoleName(RoleType role) const {
        switch (role) {
            case RoleType::GOVERNOR: return "Governor";
            case RoleType::SPY: return "Spy";
            case RoleType::BARON: return "Baron";
            case RoleType::GENERAL: return "General";
            case RoleType::JUDGE: return "Judge";
            case RoleType::MERCHANT: return "Merchant";
            default: return "Unassigned";
        }
    }
    
    // Create player with specific role
    Player* Game::createPlayerWithRole(const std::string& name, RoleType role) {
        switch (role) {
            case RoleType::GOVERNOR:
                return new Governor(*this, name);
            case RoleType::SPY:
                return new Spy(*this, name);
            case RoleType::BARON:
                return new Baron(*this, name);
            case RoleType::GENERAL:
                return new General(*this, name);
            case RoleType::JUDGE:
                return new Judge(*this, name);
            case RoleType::MERCHANT:
                return new Merchant(*this, name);
            default:
                throw std::runtime_error("Invalid role type");
        }
    }
}
