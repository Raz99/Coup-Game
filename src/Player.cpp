// Email: razcohenp@gmail.com

 // Player.cpp - Implementation of the base Player class
 // Provides core functionality for all player actions and game mechanics

#include "../include/Player.hpp"
#include "../include/Game.hpp"
#include <stdexcept> // For exception handling

namespace coup {
    /**
     * Constructor initializes a player and adds them to the game.
     * Validates input parameters and sets up initial game state.
     */
    Player::Player(Game& game, const std::string& name)
    : game(game), name(name), coin_count(0), active(true), sanctioned(false), arrest_available(true),
    bribe_used(false), used_tax_last_action(false), couped_by(nullptr) {
        if (&game == nullptr) { // Validate game reference is not null
            throw std::invalid_argument("Game reference cannot be null");
        }
        
        if (name.empty()) { // Ensure player has a valid name
            throw std::invalid_argument("Player name cannot be empty");
        }

        if (name.length() >= 10) { // Enforce name length limit for display purposes
            throw std::invalid_argument("Player name cannot exceed 9 characters");
        }

        game.addPlayer(this); // Register this player with the game instance
    }

    /**
     * Copy constructor creates a new player with copied state.
     * Used for cloning players.
     */
    Player::Player(const Player& other)
        : game(other.game), name(other.name + "_copy"), coin_count(other.coin_count), active(other.active),
        sanctioned(other.sanctioned), arrest_available(other.arrest_available), bribe_used(other.bribe_used),
        used_tax_last_action(other.used_tax_last_action), couped_by(other.couped_by) {
        // I don't call game.addPlayer(this) here to avoid automatic registration
    }

    /**
     * Copy assignment operator copies state from another player.
     * Maintains current game registration and handles self-assignment.
     */
    Player& Player::operator=(const Player& other) {
        // Handle self-assignment
        if (this == &other) {
            return *this;
        }
        
        game = other.game;
        coin_count = other.coin_count;
        active = other.active;
        sanctioned = other.sanctioned;
        arrest_available = other.arrest_available;
        bribe_used = other.bribe_used;
        used_tax_last_action = other.used_tax_last_action;
        couped_by = other.couped_by;
        
        return *this;
    }

    /**
     * Returns the player's display name for identification.
     * Used throughout the game for player recognition.
     */
    std::string Player::getName() const {
        return name; // Return the player's assigned name
    }

    /**
     * Returns the player's current coin count.
     * Essential for action validation and game state display.
     */
    int Player::coins() const {
        return coin_count; // Return current financial resources
    }

    /**
     * Checks if the player is still participating in the game.
     * Inactive players have been eliminated and cannot perform actions.
     */
    bool Player::isActive() const {
        return active; // Return current participation status
    }

    /**
     * Checks if the player is currently sanctioned.
     * Sanctioned players cannot perform economic actions like gather or tax.
     */
    bool Player::isSanctioned() const {
        return sanctioned; // Return current sanction status
    }

    /**
     * Checks if this player can be targeted by arrest actions.
     * Some roles or effects can temporarily prevent arrests.
     */
    bool Player::isArrestAvailable() const {
        return arrest_available; // Return arrest vulnerability status
    }

    /**
     * Checks if the player has used bribe action this turn.
     * Bribe allows additional actions within the same turn.
     */
    bool Player::isBribeUsed() const {
        return bribe_used; // Return bribe usage status for current turn
    }

    /**
     * Checks if tax was the player's most recent action.
     * Used by Governor role to determine if undo action is valid.
     */
    bool Player::usedTaxLastAction() const {
        return used_tax_last_action; // Return tax action tracking status
    }

    /**
     * Gets reference to the player who performed coup on this player.
     * Used for General's coup blocking ability within time window.
     */
    Player* Player::getCoupedBy() const {
        return couped_by; // Return reference to couping player
    }

    /**
     * Gather action - basic economic action to gain 1 coin.
     * Available to all players unless sanctioned or under special conditions.
     */
    void Player::gather() {
        if (!game.isGameStarted()) { // Ensure game is in progress
            throw std::runtime_error("Game has not started yet");
        }

        if (!game.isPlayerTurn(this)) { // Verify it's this player's turn
            throw std::runtime_error("Not your turn");
        }

        if (!active) { // Ensure player is still in the game
            throw std::runtime_error("Player is eliminated");
        }

        if (coin_count >= 10 && !bribe_used) { // Enforce mandatory coup rule
            throw std::runtime_error("You have 10 or more coins, must perform coup");
        }

        if (sanctioned) { // Check if economic actions are blocked
            throw std::runtime_error("Player is sanctioned");
        }

        addCoins(1); // Award 1 coin for gather action

        if(bribe_used) { // If player used bribe, allow continued play
            bribe_used = false; // Reset bribe flag for next action
        }
        
        else { // Normal turn progression
            game.nextTurn(); // Advance to next player's turn
        }
    }

    /**
     * Tax action - economic action to gain 2 coins from treasury.
     * Virtual method as some roles modify the coin amount received.
     */
    void Player::tax() {
        if (!game.isGameStarted()) { // Ensure game is in progress
            throw std::runtime_error("Game has not started yet");
        }

        if (!game.isPlayerTurn(this)) { // Verify it's this player's turn
            throw std::runtime_error("Not your turn");
        }

        if (!active) { // Ensure player is still in the game
            throw std::runtime_error("Player is eliminated");
        }

        if (coin_count >= 10 && !bribe_used) { // Enforce mandatory coup rule
            throw std::runtime_error("You have 10 or more coins, must perform coup");
        }

        if (sanctioned) { // Check if economic actions are blocked
            throw std::runtime_error("Player is sanctioned");
        }

        addCoins(2); // Award 2 coins for tax action

        if(bribe_used) { // If player used bribe, allow continued play
            bribe_used = false; // Reset bribe flag for next action
        }
        
        else { // Normal turn progression
            used_tax_last_action = true; // Mark tax as last action for Governor undo
            game.nextTurn(); // Advance to next player's turn
        }
    }

    /**
     * Bribe action - pays 4 coins to gain an additional action this turn.
     * Allows strategic flexibility by enabling multiple actions per turn.
     */
    void Player::bribe() {
        if (!game.isGameStarted()) { // Ensure game is in progress
            throw std::runtime_error("Game has not started yet");
        }

        if (!game.isPlayerTurn(this)) { // Verify it's this player's turn
            throw std::runtime_error("Not your turn");
        }

        if (!active) { // Ensure player is still in the game
            throw std::runtime_error("Player is eliminated");
        }

        if (coin_count >= 10 && !bribe_used) { // Enforce mandatory coup rule
            throw std::runtime_error("You have 10 or more coins, must perform coup");
        }

        if (coin_count < 4) { // Verify player has sufficient funds
            throw std::runtime_error("Not enough coins for bribe");
        }

        removeCoins(4); // Pay the bribe cost
        bribe_used = true; // Mark bribe as used for this turn
        // Note: No nextTurn() call as player gets another action
    }

    /**
     * Arrest action - takes 1 coin from target player.
     * Cannot target the same player consecutively to prevent harassment.
     */
    void Player::arrest(Player& target) {
        if (!game.isGameStarted()) { // Ensure game is in progress
            throw std::runtime_error("Game has not started yet");
        }
        
        if (!game.isPlayerTurn(this)) { // Verify it's this player's turn
            throw std::runtime_error("Not your turn");
        }

        if (!active) { // Ensure player is still in the game
            throw std::runtime_error("Player is eliminated");
        }

        if (!arrest_available) { // Check if arrest is blocked by Spy
            throw std::runtime_error("Arrest action is not available");
        }

        if (coin_count >= 10 && !bribe_used) { // Enforce mandatory coup rule
            throw std::runtime_error("You have 10 or more coins, must perform coup");
        }

        if (&target == this) { // Prevent self-targeting
            throw std::runtime_error("An action against yourself is not allowed");
        }

        if (!target.isActive()) { // Ensure target is still in game
            throw std::runtime_error("Target player is eliminated");
        }

        if (game.getLastArrestedPlayer() == &target) { // Prevent consecutive arrests
            throw std::runtime_error("This player was the last player to be arrested (consecutive arrest is not allowed)");
        }

        if (target.coins() >= 1) { // Only proceed if target has coins to lose
            // Merchant special ability - pays treasury
            if(target.getRoleType() == "Merchant") {
                if(target.coins() >= 2) {
                    target.removeCoins(2); // Merchant loses 2 coins to treasury instead
                }

                else {
                    target.removeCoins(1); // Merchant loses what they have
                }
            }

            // Standard arrest - transfer coin
            else if(target.getRoleType() != "General") {
                target.removeCoins(1); // Take 1 coin from target
                addCoins(1); // Give coin to arresting player
            }
        }
        
        game.setLastArrestedPlayer(&target); // Record arrest for consecutive prevention
        
        if(bribe_used) { // If player used bribe, allow continued play
            bribe_used = false; // Reset bribe flag for next action
        }
        
        else { // Normal turn progression
            game.nextTurn(); // Advance to next player's turn
        }
    }

    /**
     * Sanction action - blocks target's economic actions for one turn.
     * Costs 3 coins and prevents gather/tax until target's next turn.
     */
    void Player::sanction(Player& target) {
        if (!game.isGameStarted()) { // Ensure game is in progress
            throw std::runtime_error("Game has not started yet");
        }

        if (!game.isPlayerTurn(this)) { // Verify it's this player's turn
            throw std::runtime_error("Not your turn");
        }

        if (!active) { // Ensure player is still in the game
            throw std::runtime_error("Player is eliminated");
        }

        if (coin_count >= 10 && !bribe_used) { // Enforce mandatory coup rule
            throw std::runtime_error("You have 10 or more coins, must perform coup");
        }

        if (&target == this) { // Prevent self-targeting
            throw std::runtime_error("An action against yourself is not allowed");
        }

        if (!target.isActive()) { // Ensure target is still in game
            throw std::runtime_error("Target player is eliminated");
        }

        // Ensure player has enough coins
        if (coin_count < 3) {
            throw std::runtime_error("Not enough coins for sanction");
        }
        
        // If target is a judge, the player must pay 4 coins
        if(target.getRoleType() == "Judge") {
            if (coin_count < 4) {
                throw std::runtime_error("Not enough coins for sanction (higher fee)");
            }

            removeCoins(1); // Pay 1 coin now and 3 coins later (4 coins in total)
        }

        removeCoins(3); // Pay 3 coins

        target.setSanctionStatus(true); // Mark target as sanctioned
        
        // If player used bribe, then let him play another turn
        if(bribe_used) {
            bribe_used = false; // Reset bribe used flag
        }
        
        // If player did not use bribe, then move to next player's turn
        else {
            game.nextTurn(); // Move to next player's turn
        }
    }

    // Coup action - eliminate target for 7 coins
    void Player::coup(Player& target) {
        // Check if game has started
        if (!game.isGameStarted()) {
            throw std::runtime_error("Game has not started yet");
        }
        
        // Ensure it's the player's turn
        if (!game.isPlayerTurn(this)) {
            throw std::runtime_error("Not your turn");
        }

        // Ensure player is active
        if (!active) {
            throw std::runtime_error("Player is eliminated");
        }

        // Ensure target is not the current player
        if (&target == this) {
            throw std::runtime_error("An action against yourself is not allowed");
        }

        // Ensure target is active
        if (!target.isActive()) {
            throw std::runtime_error("Target player is eliminated");
        }

        // Ensure player has enough coins
        if (coin_count < 7) {
            throw std::runtime_error("Not enough coins for coup");
        }

        removeCoins(7); // Decrease coin count
        target.couped_by = this; // Mark this player as the one who performed the coup
        target.setActivityStatus(false); // Eliminate target
        
        // If player used bribe, then let him play another turn
        if(bribe_used) {
            bribe_used = false; // Reset bribe used flag
        }
        
        // If player did not use bribe, then move to next player's turn
        else {
            game.nextTurn(); // Move to next player's turn
        }
    }

    // Helper methods
    // Add coins to player
    void Player::addCoins(int amount) {
        // Ensure the amount is non-negative
        if (amount < 0) {
            throw std::invalid_argument("Cannot add negative coins");
        }

        coin_count += amount; // Increase coin count
    }


    // Remove coins from player
    void Player::removeCoins(int amount) {
        // Ensure the amount is non-negative
        if (amount < 0) {
            throw std::invalid_argument("Cannot remove negative coins");
        }

        // Ensure player has enough coins
        if (coin_count < amount) {
            throw std::runtime_error("Not enough coins");
        }

        coin_count -= amount; // Decrease coin count
    }

    // Set player's activity status
    void Player::setActivityStatus(bool value) {
        active = value;
    }

    // Set player as sanctioned or not-sanctioned
    void Player::setSanctionStatus(bool value) {
        sanctioned = value; // Mark player as sanctioned
    }

    /**
     * Sets whether this player can be arrested.
     * Used by Spy role to block arrests temporarily.
     */
    void Player::setArrestAvailability(bool value) {
        arrest_available = value; // Update arrest action status
    }

    /**
     * Sets the player who performed coup on this player.
     */
    void Player::setCoupedBy(Player* player) {
        couped_by = player; // Track who performed coup on this player
    }

    /**
     * Resets the bribe used flag to false.
     * Called at end of turn cleanup or after bribe action.
     */
    void Player::resetBribeUsed() {
        bribe_used = false; // Clear bribe usage flag
    }

    /**
     * Resets the tax last action tracking flag.
     * Called when Governor undoes tax or at turn end.
     */
    void Player::resetUsedTaxLastAction() {
        used_tax_last_action = false; // Clear tax action tracking
    }

    /**
     * Clears the reference to who performed coup on this player.
     * Called when coup blocking window expires.
     */
    void Player::resetCoupedBy() {
        couped_by = nullptr; // Remove coup relationship tracking
    }
}
