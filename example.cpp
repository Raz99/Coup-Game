// Email: razcohenp@gmail.com

// Main.cpp - Example usage of the Coup game with various roles
// This file demonstrates how to create a game, add players with different roles,
// and perform actions in a simulated game environment.

#include "include/Player.hpp"
#include "include/roles/Governor.hpp"
#include "include/roles/Spy.hpp"
#include "include/roles/Baron.hpp"
#include "include/roles/General.hpp"
#include "include/roles/Judge.hpp"
#include "include/roles/Merchant.hpp"
#include "include/Game.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace coup;

int main() {
    // Initialize the game
    Game game_1{};

    // Create players with different roles
    Governor governor(game_1, "Alice"); // Enhanced tax (3 coins), can undo tax
    Spy spy(game_1, "Bob"); // Can spy on players, block arrests
    Baron baron(game_1, "Carmel"); // Can invest, gets compensation when sanctioned
    General general(game_1, "David"); // Can block coups for 5 coins
    Judge judge(game_1, "Evyatar"); // Can block bribes
    Merchant merchant(game_1, "Freddy"); // Gets passive income, special arrest handling

    // Print all players' names
    cout << "Players in the game:" << endl;
    vector<string> players = game_1.players();
    for(string name : players){
        cout << "- " << name << endl;
    }
    cout << endl;

    // Start the game
    game_1.startGame();
    cout << "Game started! Current turn: ";
    game_1.turn();
    cout << endl;

    // --- ROUND 1 ---
    cout << "--- ROUND 1 ---" << endl;
    
    // Governor's enhanced tax (3 coins instead of 2)
    cout << "1. " << governor.getName() << " uses enhanced tax (Governor ability)" << endl;
    cout << "   " << "Coins before: " << governor.coins() << endl;
    governor.tax();
    cout << "   " << "Coins after: " << governor.coins() << " (+3 coins from Governor tax)" << endl;
    cout << endl;

    // Spy gathers
    cout << "2. " << spy.getName() << " gathers" << endl;
    cout << "   " << "Coins before: " << spy.coins() << endl;
    spy.gather();
    cout << "   " << "Coins after: " << spy.coins() << " (+1 coin)" << endl;
    cout << endl;

    // Baron gathers
    cout << "3. " << baron.getName() << " gathers" << endl;
    cout << "   " << "Coins before: " << baron.coins() << endl;
    baron.gather();
    cout << "   " << "Coins after: " << baron.coins() << " (+1 coin)" << endl;
    cout << endl;

    // General taxes
    cout << "4. " << general.getName() << " taxes" << endl;
    cout << "   " << "Coins before: " << general.coins() << endl;
    general.tax();
    cout << "   " << "Coins after: " << general.coins() << " (+2 coins)" << endl;
    cout << endl;

    // Judge gathers
    cout << "5. " << judge.getName() << " gathers" << endl;
    cout << "   " << "Coins before: " << judge.coins() << endl;
    judge.gather();
    cout << "   " << "Coins after: " << judge.coins() << " (+1 coin)" << endl;
    cout << endl;

    // Merchant gathers
    cout << "6. " << merchant.getName() << " gathers" << endl;
    cout << "   " << "Coins before: " << merchant.coins() << endl;
    merchant.gather();
    cout << "   " << "Coins after: " << merchant.coins() << " (+1 coin)" << endl;
    cout << endl;

    // --- ROUND 2 ---
    cout << "--- ROUND 2 ---" << endl;
    
    governor.tax();
    spy.tax();
    baron.tax();
    general.tax();
    judge.tax();
    merchant.tax();

    cout << "7. " << "All players have taxed and gained coins, current coin counts" << endl;
    for (Player* player : game_1.getAllPlayers()) {
        cout << "   " << "- " << player->getName() << ": " << player->coins() << " coins" << endl;
    }
    cout << endl;

    // --- ROUND 3 ---
    cout << "--- ROUND 3 ---" << endl;
    
    // Governor can undo someone's tax
    cout << "8. " << governor.getName() << " undoes " << general.getName() << "'s tax (Governor ability)" << endl;
    cout << "   " << general.getName() << " coins before undo: " << general.coins() << endl;
    governor.undo(general);
    cout << "   " << general.getName() << " coins after undo: " << general.coins() << " (-2 coins)" << endl;
    cout << endl;
    governor.gather(); // Governor can still gather normally

    // Spy can spy on someone to see their coins and block their arrest ability
    cout << "9. " << spy.getName() << " spies on " << baron.getName() << " (Spy ability)" << endl;
    spy.spy_on(baron);
    cout << "    " << spy.getName() << "spies on " << baron.getName() << ": " << baron.coins() << "coins revealed" << endl;
    cout << "   " << baron.getName() << "'s arrest ability blocked for next turn" << endl;
    cout << endl;
    spy.tax(); // Spy can still tax normally

    // Baron can invest 3 coins to get 6 coins (+3 in total)
    cout << "10. " << baron.getName() << " invests (Baron ability)" << endl;
    cout << "    " << "Coins before investment: " << baron.coins() << endl;
    baron.invest();
    cout << "    " << "Coins after investment: " << baron.coins() << " (paid 3, received 6, +3 in total)" << endl;
    cout << endl;

    // General taxes
    cout << "11. " << general.getName() << " gathers" << endl;
    cout << "    " << "Coins before: " << general.coins() << endl;
    general.tax();
    cout << "    " << "Coins after: " << general.coins() << " (+1 coin)" << endl;
    cout << endl;

    // Judge gathers
    cout << "12. " << judge.getName() << " gathers" << endl;
    cout << "    " << "Coins before: " << judge.coins() << endl;
    judge.gather();
    cout << "    " << "Coins after: " << judge.coins() << " (+1 coin)" << endl;
    cout << endl;

    // Merchant gathers
    cout << "13. " << merchant.getName() << " gathers" << endl;
    cout << "    " << "Coins before: " << merchant.coins() << endl;
    merchant.gather();
    cout << "    " << "Coins after: " << merchant.coins() << " (+1 coin)" << endl;
    cout << endl;

    // --- ROUND 4 ---
    cout << "--- ROUND 4 ---" << endl;

    // Governor uses bribe to get an extra action
    cout << "14. " << governor.getName() << " uses bribe (4 coins for extra action)" << endl;
    cout << "    " << "Coins before bribe: " << governor.coins() << endl;
    governor.bribe();
    cout << "    " << "Coins after bribe: " << governor.coins() << " (-4 coins)" << endl;
    cout << "    " << governor.getName() << " gets an additional action this turn!" << endl;
    cout << endl;

    // Governor uses the first action to tax (1/2 actions used)
    cout << "15. " << governor.getName() << " uses enhanced tax (1/2 actions used)" << endl;
    cout << "    " << "Coins before: " << governor.coins() << endl;
    governor.tax();
    cout << "    " << "Coins after: " << governor.coins() << " (+3 coins from Governor tax)" << endl;
    cout << endl;

    // Governor can still gather (2/2 actions used)
    cout << "16. " << governor.getName() << " gathers (2/2 actions used)" << endl;
    cout << "    " << "Coins before: " << governor.coins() << endl;
    governor.gather();
    cout << "    " << "Coins after: " << governor.coins() << " (+1 coin)" << endl;
    cout << endl;

    // Judge can block bribe attempts
    cout << "17. " << spy.getName() << " attempts bribe, but " << judge.getName() << " blocks it (Judge ability)" << endl;
    cout << "    " << spy.getName() << " coins before blocked bribe: " << spy.coins() << endl;
    spy.bribe();
    cout << "    " << spy.getName() << " coins after bribe: " << spy.coins() << " (-4 coins for bribe)" << endl;
    judge.block_bribe(spy);
    cout << "    Judge blocked the bribe! " << spy.getName() << " loses 4 coins but gets no extra action!" << endl;
    cout << "    " << spy.getName() << " coins after block: " << spy.coins() << endl;

    // Spy can still gather after blocked bribe
    spy.gather(); // Spy can still gather as the first and last action for this turn
    cout << "    " << spy.getName() << " can still gather as the first and last action for this turn (Updated coins: " << spy.coins() << ")" << endl;
    
    // Spy can still spy on someone since it doesn't consume a turn
    cout << "    " << spy.getName() << " can still spy on someone since it doesn't consume a turn" << endl;
    spy.spy_on(baron); // Spy can still spy on baron to know the amount of coins he has and to block arrest
    cout << "    " << spy.getName() << "spies on " << baron.getName() << ": " << baron.coins() << "coins revealed" << endl;
    cout << endl;

    // Baron arrests someone (but spy blocked baron's arrest earlier)
    cout << "18. " << baron.getName() << " attempts to arrest " << merchant.getName() << endl;
    cout << "    " << baron.getName() << " coins before: " << baron.coins() << endl;
    cout << "    " << merchant.getName() << " coins before: " << merchant.coins() << endl;
    
    // Try arrest - should fail because spy blocked baron's arrest ability
    try {
        baron.arrest(merchant);
        cout << "    Arrest successful" << endl;
    }
    catch (const std::exception &e){
        cout << "    Arrest failed: " << e.what() << endl;
        // Does tax for instead
        baron.tax();
        cout << "    " << baron.getName() << " uses tax instead (Updated coins: " << baron.coins() << ")" << endl;
    }
    cout << endl;

    // General arrests someone
    cout << "19. " << general.getName() << " arrests " << merchant.getName() << endl;
    cout << "    " << general.getName() << " coins before: " << general.coins() << endl;
    cout << "    " << merchant.getName() << " coins before: " << merchant.coins() << endl;
    general.arrest(merchant);
    cout << "    " << general.getName() << " coins after: " << general.coins() << " (no change)" << endl;
    cout << "    " << merchant.getName() << " coins after: " << merchant.coins() << " (-1 coin)" << endl;
    cout << endl;

    // Sanction with Judge costing extra
    cout << "20. " << judge.getName() << " sanctions " << baron.getName() << " (Judge costs 4 coins to sanction)" << endl;
    cout << "    " << judge.getName() << " coins before: " << judge.coins() << endl;
    cout << "    " << baron.getName() << " coins before: " << baron.coins() << endl;
    judge.sanction(baron);
    cout << "    " << judge.getName() << " coins after: " << judge.coins() << " (-4 coins to sanction Judge)" << endl;
    cout << "    " << baron.getName() << " coins after: " << baron.coins() << " (+1 coin compensation for being sanctioned)" << endl;
    cout << "    " << baron.getName() << " is now sanctioned (cannot gather/tax until next turn)" << endl;
    cout << endl;

    // Show sanction status
    cout << "21. " << merchant.getName() << " sanctions " << general.getName() << " (normal cost)" << endl;
    cout << "    " << merchant.getName() << " coins before: " << merchant.coins() << endl;
    merchant.sanction(general);
    cout << "    " << merchant.getName() << " coins after: " << merchant.coins() << " (-3 coins)" << endl;
    cout << "    " << general.getName() << " is now sanctioned (cannot gather/tax until next turn)" << endl;
    cout << endl;

    // --- ROUND 5 ---
    cout << "--- ROUND 5 ---" << endl;

    // Governor taxes (building wealth)
    cout << "22. " << governor.getName() << " uses enhanced tax" << endl;
    cout << "    " << "Coins before: " << governor.coins() << endl;
    governor.tax();
    cout << "    " << "Coins after: " << governor.coins() << " (+3 coins)" << endl;
    cout << endl;

    // Spy gathers
    cout << "23. " << spy.getName() << " gathers" << endl;
    cout << "    " << "Coins before: " << spy.coins() << endl;
    spy.gather();
    cout << "    " << "Coins after: " << spy.coins() << " (+1 coin)" << endl;
    cout << endl;

    // Baron is sanctioned, so gather/tax will fail
    cout << "24. " << baron.getName() << " tries to gather while sanctioned" << endl;
    try {
        baron.gather();
    }
    catch (const std::exception &e){
        cout << "    " << "Action failed: " << e.what() << endl;
    }
    // Baron can still invest though
    cout << "    " << baron.getName() << " invests instead (Baron ability)" << endl;
    cout << "    " << "Coins before investment: " << baron.coins() << endl;
    baron.invest();
    cout << "    " << "Coins after investment: " << baron.coins() << " (net +3)" << endl;
    cout << endl;

    // General is also sanctioned
    cout << "25. " << general.getName() << " tries to tax while sanctioned" << endl;
    try {
        general.tax();
    }
    catch (const std::exception &e){
        cout << "    " << "Action failed: " << e.what() << endl;
        // General arrests someone for instead
        cout << "    " << general.getName() << " arrests " << judge.getName() << " for instead" << endl;
        cout << "    " << general.getName() << " coins before: " << general.coins() << endl;
        cout << "    " << judge.getName() << " coins before: " << judge.coins() << endl;
        general.arrest(judge);
        cout << "    " << general.getName() << " coins after: " << general.coins() << " (no change)" << endl;
        cout << "    " << judge.getName() << " coins after: " << judge.coins() << " (-1 coin)" << endl;
    }
    cout << endl;

    // Judge gathers
    cout << "26. " << judge.getName() << " gathers" << endl;
    cout << "    " << "Coins before: " << judge.coins() << endl;
    judge.gather();
    cout << "    " << "Coins after: " << judge.coins() << " (+1 coin)" << endl;
    cout << endl;

    // Merchant gathers
    cout << "27. " << merchant.getName() << " gathers" << endl;
    cout << "    " << "Coins before: " << merchant.coins() << endl;
    merchant.gather();
    cout << "    " << "Coins after: " << merchant.coins() << " (+1 coin)" << endl;
    cout << endl;

    // --- ROUND 6 ---
    cout << "--- ROUND 6 ---" << endl;

    // Governor has enough for coup (7+ coins)
    cout << "28. " << governor.getName() << " performs coup on " << spy.getName() << " (7 coins)" << endl;
    cout << "    " << governor.getName() << " coins before coup: " << governor.coins() << endl;
    cout << "    " << spy.getName() << " status before: " << (spy.isActive() ? "Active" : "Eliminated") << endl;
    governor.coup(spy);
    cout << "    " << governor.getName() << " coins after coup: " << governor.coins() << " (-7 coins)" << endl;
    cout << "    " << spy.getName() << " status after coup: " << (spy.isActive() ? "Active" : "Eliminated") << endl;
    cout << endl;

    // General can block the coup and revive the spy (costs 5 coins)
    cout << "29. " << general.getName() << " blocks the coup and revives " << spy.getName() << " (General ability, 5 coins)" << endl;
    cout << "    " << general.getName() << " coins before: " << general.coins() << endl;
    cout << "    " << spy.getName() << " status before block: " << (spy.isActive() ? "Active" : "Eliminated") << endl;
    general.block_coup(spy);
    cout << "    " << general.getName() << " coins after: " << general.coins() << " (-5 coins)" << endl;
    cout << "    " << spy.getName() << " status after block: " << (spy.isActive() ? "Active" : "Eliminated") << endl;
    cout << "    " << spy.getName() << " has been revived by General" << endl;
    cout << endl;

    // Show current players
    cout << "Current active players:" << endl;
    players = game_1.players();
    for(string name : players){
        cout << "- " << name << endl;
    }
    cout << endl;

    // --- ROUND 7 ---
    cout << "--- ROUND 7 ---" << endl;

    // Show mandatory coup rule (if someone has 10+ coins)
    if (baron.coins() >= 10) {
        cout << "30. " << baron.getName() << " has 10+ coins and must perform coup" << endl;
        cout << "    " << baron.getName() << " coins: " << baron.coins() << endl;
        cout << "    " << "Performing mandatory coup on " << judge.getName() << endl;
        baron.coup(judge);
        cout << "    " << baron.getName() << " coins after coup: " << baron.coins() << " (-7 coins)" << endl;
        cout << "    " << judge.getName() << " status: " << (judge.isActive() ? "Active" : "Eliminated") << endl;
        cout << endl;
    }

    // Current game state
    cout << "--- GAME STATE ---" << endl;
    cout << "Active players:" << endl;
    players = game_1.players();
    for(string name : players){
        cout << "- " << name << endl;
    }
    cout << endl;

    // Show final coin counts
    cout << "Coin counts:" << endl;
    vector<Player*> allPlayers = game_1.getAllPlayers();
    for(Player* player : allPlayers) {
        cout << "- " << player->getName() << " (" << player->getRoleType() << "): " 
             << player->coins() << " coins " 
             << (player->isActive() ? "[Active]" : "[Eliminated]") << endl;
    }
    cout << endl;
    
    return 0;
}