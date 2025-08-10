// Email: razcohenp@gmail.com

/**
 * Game.hpp
 * Main game controller class.
 * Manages players, turns, game state and victory conditions.
 * Supports 2-6 players with role assignment and turn-based gameplay.
 */

#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <random> // Random number generation for role assignment

namespace coup {
    class Player; // Forward declaration to avoid circular dependency

    /**
     * Enumeration of all available character roles in the game.
     * Each role has unique abilities and gameplay mechanics.
     */
    enum class RoleType {
        GOVERNOR, // Takes 3 coins on tax, can block other players' tax
        SPY, // Can see coins and block arrests
        BARON, // Can invest coins and gets compensation when sanctioned
        GENERAL, // Can block coups and recovers from arrests
        JUDGE, // Can block bribes and penalizes sanctioners
        MERCHANT, // Gets bonus coins and pays treasury when arrested
        PLAYER // Default base player with no special abilities
    };
    /**
     * Main game controller class for the Coup card game.
     * Manages all aspects of gameplay including player management,
     * turn progression, role assignment and victory conditions.
     */
    class Game {
    private:
        std::vector<Player*> players_list; // Collection of all players in the game
        int current_player_index; // Index of the player whose turn it is
        bool game_started; // Flag indicating if the game has begun
        Player* last_arrested_player; // Reference to prevent consecutive arrests
        std::mt19937 random_generator; // Pseudorandom number generator for fair role distribution
        
    public:
        /**
         * Default constructor initializes an empty game.
         * Sets up random number generator and prepares for player addition.
         */
        Game();

        /**
         * Copy constructor - creates a deep copy of the game state.
         * Copies all players and maintains game relationships.
         */
        Game(const Game& other);
        
        /**
         * Destructor cleans up game resources.
         * Ensures proper memory management for player objects.
         */
        ~Game();

        /**
         * Copy assignment operator - replaces current game with copy of other.
         * Handles self-assignment and proper cleanup of existing resources.
         */
        Game& operator=(const Game& other);
        
        /**
         * Displays the name of the current player whose turn it is.
         * Throws exception if game hasn't started or no players exist.
         */
        void turn() const;
        
        /**
         * Returns a list of names of all currently active players.
         * Active players are those who haven't been eliminated by coup.
         */
        std::vector<std::string> players() const;
        
        /**
         * Returns the name of the winning player.
         * Only one player should remain active for this to succeed.
         * Throws exception if game is still ongoing or hasn't started.
         */
        std::string winner() const;
        
        /**
         * Adds a new player to the game before it starts.
         * Maximum of 6 players allowed, minimum of 2 required to start.
         */
        void addPlayer(Player* player);
        
        /**
         * Advances the game to the next player's turn.
         * Handles end-of-turn cleanup and special role abilities.
         */
        void nextTurn();

        /**
         * Checks if it's the specified player's turn to act.
         * Used for validating player actions and turn order.
         */
        bool isPlayerTurn(const Player* player) const;

        /**
         * Initiates the game with current players.
         * Requires between 2-6 players to be valid.
         * Assigns roles randomly and prepares for first turn.
         */
        void startGame();
        
        /**
         * Returns whether the game has been started.
         * Used to prevent invalid operations during setup phase.
         */
        bool isGameStarted() const;
        
        /**
         * Gets a pointer to the player whose turn it currently is.
         * Returns null if no current player exists.
         */
        Player* getCurrentPlayer() const;
        
        /**
         * Returns all players regardless of active status.
         * Includes both active and eliminated players.
         */
        std::vector<Player*> getAllPlayers() const;
        
        /**
         * Returns only players who are still in the game.
         * Excludes players who have been eliminated by coup.
         */
        std::vector<Player*> getActivePlayers() const;
        
        /**
         * Records which player was most recently arrested.
         * Used to enforce the rule preventing consecutive arrests.
         */
        void setLastArrestedPlayer(Player* player);

        /**
         * Gets the player who was most recently arrested.
         * Returns null if no arrests have occurred recently.
         */
        Player* getLastArrestedPlayer() const;
        
        /**
         * Removes all players from the game.
         * Only allowed when game hasn't started yet.
         */
        void clearAllPlayers();
        
        /**
         * Removes a specific player from the game.
         * Only allowed during setup phase before game starts.
         */
        void removePlayer(Player* player);

        /**
         * Checks if a General player can prevent game from ending.
         * Used to determine if coup blocking is possible.
         */
        bool canGeneralPreventGameEnd() const;

        /**
         * Provides access to the random number generator.
         * Used for deterministic role assignment in derived classes.
         */
        std::mt19937& getRandomGenerator() { return random_generator; }

        /**
         * Assigns random roles to existing players.
         * Transforms base Player objects into role-specific derived classes.
         */
        void assignRolesToExistingPlayers();
        
        /**
         * Converts a role enumeration to its display name.
         * Used for GUI and debugging purposes.
         */
        std::string getRoleName(RoleType role) const;
        
        /**
         * Factory method to create players with specific roles.
         * Used internally for role assignment during game setup.
         */
        Player* createPlayerWithRole(const std::string& name, RoleType role);
    };
}
#endif