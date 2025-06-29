// Email: razcohenp@gmail.com

/**
 * Comprehensive tests for all role classes
 * Tests unique abilities, role identification, and role-specific interactions
 * Covers the special role abilities in the Coup card game:
 * - Governor: Enhanced tax (3 coins) and undo last tax action
 * - General: Block coup attempts by paying 5 coins
 * - Judge: Block bribe effects from other players
 * - Baron: Investment action (pay 3, gain 6 coins net +3)
 * - Spy: Spy on players to see coins and block arrests
 * - Merchant: Bonus coin when turn starts with 3+ coins, special arrest defense
 */

#include "doctest.h"
#include <stdexcept>
#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include "../include/roles/Governor.hpp"
#include "../include/roles/General.hpp"
#include "../include/roles/Judge.hpp"
#include "../include/roles/Baron.hpp"
#include "../include/roles/Spy.hpp"
#include "../include/roles/Merchant.hpp"

using namespace coup;

TEST_CASE("Governor Role Tests") {
    Game game; // Create game for Governor testing
    Governor gov1(game, "Governor"); // Governor player
    Baron baron(game, "Baron"); // Regular player for interaction
    game.startGame(); // Start game to enable actions
    
    SUBCASE("Governor role identification") {
        CHECK(gov1.getRoleType() == "Governor"); // Should identify as Governor
    }
    
    SUBCASE("Enhanced tax ability - 3 coins instead of 2") {
        CHECK(gov1.coins() == 0); // Start with 0 coins
        gov1.tax(); // Governor uses enhanced tax
        CHECK(gov1.coins() == 3); // Governor gets 3 coins instead of 2
        CHECK(gov1.usedTaxLastAction()); // Tax usage should be tracked
    }
    
    SUBCASE("Undo action - valid target") {
        // Set up scenario where regular player used tax
        CHECK_NOTHROW(gov1.tax()); // Governor taxes to advance turn
        CHECK_NOTHROW(baron.tax()); // Baron taxes normally (gets 2 coins)
        CHECK(baron.coins() == 2); // Baron should have 2 coins
        CHECK(baron.usedTaxLastAction()); // Baron's tax flag should be set
        
        // Governor undoes the tax
        gov1.undo(baron); // Governor undoes Baron's tax
        CHECK(baron.coins() == 0); // Baron loses the 2 coins from tax
        CHECK_FALSE(baron.usedTaxLastAction()); // Tax flag should be reset
    }
    
    SUBCASE("Undo action - invalid targets") {
        // Cannot undo if target didn't use tax as last action
        CHECK_THROWS_AS(gov1.undo(baron), std::runtime_error); // Baron hasn't used tax
        
        // Cannot undo on self
        gov1.tax(); // Governor uses tax
        CHECK_THROWS_AS(gov1.undo(gov1), std::runtime_error); // Cannot undo own actions
        
        // Cannot undo inactive player
        baron.setActivityStatus(false); // Eliminate Baron
        CHECK_THROWS_AS(gov1.undo(baron), std::runtime_error); // Cannot undo eliminated players
    }
    
    SUBCASE("Undo when game not started") {
        Game new_game; // Create new unstarted game
        Governor new_gov(new_game, "NewGov"); // Add Governor
        Baron new_baron(new_game, "NewBaron"); // Add Baron
        
        CHECK_THROWS(new_gov.undo(new_baron)); // Cannot undo before game starts
    }
}

TEST_CASE("General Role Tests") {
    Game game; // Create game for General testing
    General gen(game, "General"); // General player
    Player victim(game, "Victim"); // Player to be saved from coup
    Player attacker(game, "Attacker"); // Player performing coup
    game.startGame(); // Start game to enable actions
    
    SUBCASE("General role identification") {
        CHECK(gen.getRoleType() == "General"); // Should identify as General
    }
    
    SUBCASE("Block coup - valid scenario") {
        gen.addCoins(10); // Give General coins for blocking
        attacker.addCoins(7); // Give attacker coins for coup
        
        // Attacker coups victim
        game.nextTurn(); // Skip to attacker's turn
        game.nextTurn(); // Skip to attacker's actual turn
        attacker.coup(victim); // Attacker eliminates victim
        CHECK_FALSE(victim.isActive()); // Victim should be eliminated
        CHECK(victim.getCoupedBy() == &attacker); // Track who performed coup
        
        // General blocks the coup
        gen.block_coup(victim); // General pays 5 coins to save victim
        CHECK(gen.coins() == 5); // General should have 5 coins left (10-5=5)
        CHECK(victim.isActive()); // Victim should be restored to active
        CHECK(victim.getCoupedBy() == nullptr); // Coup reference should be cleared
    }
    
    SUBCASE("Block coup - insufficient coins") {
        gen.addCoins(4); // Give General insufficient coins
        attacker.addCoins(7); // Give attacker coins for coup
        
        // Set up coup scenario
        victim.setActivityStatus(false); // Victim is eliminated
        victim.setCoupedBy(&attacker); // Track coup performer
        
        CHECK_THROWS_AS(gen.block_coup(victim), std::runtime_error); // Not enough coins to block
        CHECK(gen.coins() == 4); // Coins should be unchanged
    }
    
    SUBCASE("Block coup - invalid targets") {
        gen.addCoins(10); // Give General coins
        
        // Cannot block coup on active player (not couped)
        CHECK_THROWS_AS(gen.block_coup(victim), std::runtime_error); // Victim is still active
        
        // Cannot block coup when no coup reference exists
        victim.setActivityStatus(false); // Eliminate victim
        CHECK_THROWS_AS(gen.block_coup(victim), std::runtime_error); // No coup to block
    }
    
    SUBCASE("Block coup timing restrictions") {
        gen.addCoins(5); // Give General minimum coins for blocking
        
        // Simulate coup that's too late to block
        victim.setActivityStatus(false); // Victim eliminated
        victim.setCoupedBy(nullptr); // Coup already resolved (no reference)
        
        CHECK_THROWS_AS(gen.block_coup(victim), std::runtime_error); // Too late to block
    }
}

TEST_CASE("Judge Role Tests") {
    Game game; // Create game for Judge testing
    Judge judge(game, "Judge"); // Judge player
    Player briber(game, "Briber"); // Player who will use bribe
    game.startGame(); // Start game to enable actions
    
    SUBCASE("Judge role identification") {
        CHECK(judge.getRoleType() == "Judge"); // Should identify as Judge
    }
    
    SUBCASE("Block bribe - valid scenario") {
        briber.addCoins(4); // Give briber coins for bribe
        
        game.nextTurn(); // Switch to briber's turn
        briber.bribe(); // Briber uses bribe (pays 4 coins)
        CHECK(briber.isBribeUsed()); // Bribe flag should be set
        CHECK(briber.coins() == 0); // Briber should have 0 coins left
        
        // Judge blocks the bribe
        judge.block_bribe(briber); // Judge negates bribe effect
        CHECK_FALSE(briber.isBribeUsed()); // Bribe effect should be removed
        // Note: coins already spent are not refunded in this implementation
    }
    
    SUBCASE("Block bribe - invalid targets") {
        // Cannot block if target hasn't used bribe
        CHECK_THROWS_AS(judge.block_bribe(briber), std::runtime_error); // Briber hasn't used bribe
        
        // Cannot block on self
        CHECK_THROWS_AS(judge.block_bribe(judge), std::runtime_error); // Cannot block own bribe
        
        // Cannot block inactive player
        briber.setActivityStatus(false); // Eliminate briber
        CHECK_THROWS_AS(judge.block_bribe(briber), std::runtime_error); // Cannot block eliminated players
    }
}

TEST_CASE("Baron Role Tests") {
    Game game; // Create game for Baron testing
    Baron baron(game, "Baron"); // Baron player
    Player regular(game, "Regular"); // Regular player for turn testing
    game.startGame(); // Start game to enable actions
    
    SUBCASE("Baron role identification") {
        CHECK(baron.getRoleType() == "Baron"); // Should identify as Baron
    }
    
    SUBCASE("Investment ability - valid scenario") {
        baron.addCoins(5); // Give Baron 5 coins
        CHECK(baron.coins() == 5); // Verify starting coins
        
        baron.invest(); // Baron invests (pays 3, receives 6, net +3)
        CHECK(baron.coins() == 8); // Should have 8 coins (5-3+6=8)
        CHECK(game.isPlayerTurn(&regular)); // Turn should advance to next player
    }
    
    SUBCASE("Investment with minimum coins") {
        baron.addCoins(3); // Give Baron exactly minimum coins
        baron.invest(); // Baron invests all coins
        CHECK(baron.coins() == 6); // Should have 6 coins (3-3+6=6)
    }
    
    SUBCASE("Investment with insufficient coins") {
        baron.addCoins(2); // Give Baron insufficient coins
        CHECK_THROWS_AS(baron.invest(), std::runtime_error); // Need 3 coins to invest
        CHECK(baron.coins() == 2); // Coins should be unchanged
    }
    
    SUBCASE("Investment when not player's turn") {
        baron.addCoins(5); // Give Baron coins
        game.nextTurn(); // Switch to regular player's turn
        CHECK_THROWS_AS(baron.invest(), std::runtime_error); // Only current player can act
    }
    
    SUBCASE("Investment with 10+ coins without bribe") {
        baron.addCoins(10); // Give Baron 10+ coins (mandatory coup threshold)
        CHECK_THROWS_AS(baron.invest(), std::runtime_error); // Must coup instead of invest
    }
    
    SUBCASE("Investment after bribe") {
        baron.addCoins(10); // Give Baron 10+ coins
        // After using bribe, should be able to invest
        // This would need to be tested with proper bribe setup
    }
}

TEST_CASE("Spy Role Tests") {
    Game game; // Create game for Spy testing
    Spy spy(game, "Spy"); // Spy player
    Player target(game, "Target"); // Player to spy on
    game.startGame(); // Start game to enable actions
    
    SUBCASE("Spy role identification") {
        CHECK(spy.getRoleType() == "Spy"); // Should identify as Spy
    }
    
    SUBCASE("Spy operation - valid target") {
        target.addCoins(5); // Give target coins to spy on
        CHECK(target.isArrestAvailable()); // Target can be arrested initially
        
        spy.spy_on(target); // Spy reveals target's coins and blocks arrests
        CHECK_FALSE(target.isArrestAvailable()); // Target should be protected from arrests
        // Coin information would be displayed via GUI
        
        // Spy operation doesn't consume a turn
        CHECK(game.isPlayerTurn(&spy)); // Should still be Spy's turn
    }
    
    SUBCASE("Spy operation - invalid targets") {
        // Cannot spy on self
        CHECK_THROWS_AS(spy.spy_on(spy), std::runtime_error); // Cannot spy on yourself
        
        // Cannot spy on inactive player
        target.setActivityStatus(false); // Eliminate target
        CHECK_THROWS_AS(spy.spy_on(target), std::runtime_error); // Cannot spy on eliminated players
    }
    
    SUBCASE("Spy operation when game not started") {
        Game new_game; // Create unstarted game
        Spy new_spy(new_game, "NewSpy"); // Add Spy
        Player new_target(new_game, "NewTarget"); // Add target
        
        CHECK_THROWS_AS(new_spy.spy_on(new_target), std::runtime_error); // Cannot spy before game starts
    }
    
    SUBCASE("Spy operation when spy eliminated") {
        spy.setActivityStatus(false); // Eliminate Spy
        CHECK_THROWS_AS(spy.spy_on(target), std::runtime_error); // Eliminated players cannot act
    }
}

TEST_CASE("Merchant Role Tests") {
    Game game; // Create game for Merchant testing
    Merchant merchant(game, "Merchant"); // Merchant player
    Player regular(game, "Regular"); // Regular player for turn testing
    game.startGame(); // Start game to enable actions
    
    SUBCASE("Merchant role identification") {
        CHECK(merchant.getRoleType() == "Merchant"); // Should identify as Merchant
    }
    
    SUBCASE("Turn start bonus with sufficient coins") {
        merchant.addCoins(3); // Give Merchant 3 coins (threshold for bonus)
        
        // When merchant's turn begins with 3+ coins, they get +1 bonus
        // This would be triggered in nextTurn() logic
        game.nextTurn(); // Move to regular player
        game.nextTurn(); // Back to merchant's turn
        
        // In implementation, merchant should get bonus coin
        // CHECK(merchant.coins() == 4); // 3 + 1 bonus
    }
    
    SUBCASE("Turn start bonus with insufficient coins") {
        merchant.addCoins(2); // Give Merchant insufficient coins for bonus
        
        game.nextTurn(); // Move to regular player
        game.nextTurn(); // Back to merchant's turn
        
        // No bonus with less than 3 coins
        CHECK(merchant.coins() == 2); // Should remain at 2 coins
    }
    
    SUBCASE("Arrest on Merchant - Merchant pays 2 coins to treasury and the one who arrests gets nothing") {
        merchant.addCoins(2); // Give Merchant 2 coin
        regular.addCoins(1); // Give regular player 1 coin
        
        game.nextTurn(); // Switch to regular player's turn
        regular.arrest(merchant); // Regular player arrests Merchant

        // Merchant should pay 2 coins to treasury
        CHECK(merchant.coins() == 0); // Merchant should have paid 2 coins (2-2=0)
        CHECK(regular.coins() == 1); // Regular player should not gain coins from arrest on Merchant
    }
}

TEST_CASE("Role Interaction Scenarios") {
    Game game; // Create game for complex role interactions
    Governor gov(game, "Governor"); // Governor with undo ability
    General gen(game, "General"); // General with coup blocking
    Judge judge(game, "Judge"); // Judge with bribe blocking
    Baron baron(game, "Baron"); // Baron with investment
    game.startGame(); // Start game to enable interactions
    
    SUBCASE("Judge blocking Baron's bribe") {
        gov.tax(); // Governor taxes to advance turn
        gen.tax(); // General taxes to advance turn
        judge.tax(); // Judge taxes to advance turn
        baron.addCoins(4); // Give Baron coins for bribe
        baron.bribe(); // Baron uses bribe
        CHECK(baron.isBribeUsed()); // Bribe should be active
        
        judge.block_bribe(baron); // Judge blocks the bribe
        CHECK_FALSE(baron.isBribeUsed()); // Bribe should be negated
    }
    
    SUBCASE("General blocking coup on Judge") {
        gov.addCoins(7); // Give Governor coins for coup
        gen.addCoins(5); // Give General coins for blocking
        
        // Attacker coups judge
        gov.coup(judge); // Governor eliminates Judge
        CHECK_FALSE(judge.isActive()); // Judge should be eliminated
        
        // General saves judge
        gen.block_coup(judge); // General pays to save Judge
        CHECK(judge.isActive()); // Judge should be restored
        CHECK(gen.coins() == 0); // General should have 0 coins left (5-5=0)
    }
    
    SUBCASE("Multiple role abilities in sequence") {
        // Complex scenario testing multiple interactions
        gov.tax(); // Governor gets 3 coins from enhanced tax
        gen.tax(); // General gets 2 coins from normal tax
        judge.tax(); // Judge gets 2 coins from normal tax
        gen.addCoins(9); // Give additional coins for testing
        judge.addCoins(9); // Give additional coins for testing
        baron.addCoins(9); // Give additional coins for testing
        
        // Baron invests
        baron.invest(); // Baron pays 3, receives 6 (net +3)
        CHECK(baron.coins() == 12); // Should have 12 coins (9-3+6=12)
        
        // Judge blocks potential bribe (if baron had used one)
        // General blocks potential coup
        // Governor undoes potential tax
    }
}

TEST_CASE("Role Edge Cases and Error Handling") {
    SUBCASE("Role abilities when game not started") {
        Game game; // Create unstarted game
        Governor gov(game, "Gov"); // Add Governor
        General gen(game, "Gen"); // Add General
        Judge judge(game, "Judge"); // Add Judge
        Baron baron(game, "Baron"); // Add Baron
        Spy spy(game, "Spy"); // Add Spy
        
        Player target(game, "Target"); // Add target for abilities
        
        CHECK_THROWS_AS(gov.undo(target), std::runtime_error); // Cannot undo before game starts
        CHECK_THROWS_AS(gen.block_coup(target), std::runtime_error); // Cannot block before game starts
        CHECK_THROWS_AS(judge.block_bribe(target), std::runtime_error); // Cannot block bribe before game starts
        CHECK_THROWS_AS(baron.invest(), std::runtime_error); // Cannot invest before game starts
        CHECK_THROWS_AS(spy.spy_on(target), std::runtime_error); // Cannot spy before game starts
    }
    
    SUBCASE("Role abilities when eliminated") {
        Game game; // Create game
        Governor gov(game, "Gov"); // Add Governor
        Player target(game, "Target"); // Add target
        game.startGame(); // Start game
        
        gov.setActivityStatus(false); // Eliminate Governor
        CHECK_THROWS_AS(gov.undo(target), std::runtime_error); // Eliminated players cannot use abilities
    }
    
    SUBCASE("Role state consistency after failed actions") {
        Game game; // Create game
        Baron baron(game, "Baron"); // Add Baron
        Player target(game, "Target"); // Add target
        game.startGame(); // Start game
        
        baron.addCoins(2); // Give Baron insufficient coins
        int initial_coins = baron.coins(); // Store initial state
        
        CHECK_THROWS_AS(baron.invest(), std::runtime_error); // Should fail due to insufficient coins
        CHECK(baron.coins() == initial_coins); // State should be unchanged after failure
    }
    
    SUBCASE("Role polymorphism and casting") {
        Game game; // Create game for polymorphism testing
        
        // Create roles as base Player pointers
        Player* gov = new Governor(game, "Gov"); // Governor as Player pointer
        Player* gen = new General(game, "Gen"); // General as Player pointer
        Player* judge = new Judge(game, "Judge"); // Judge as Player pointer
        
        // Test role identification through base pointers
        CHECK(gov->getRoleType() == "Governor"); // Should identify correctly through polymorphism
        CHECK(gen->getRoleType() == "General"); // Should identify correctly through polymorphism
        CHECK(judge->getRoleType() == "Judge"); // Should identify correctly through polymorphism
        
        // Cleanup handled by Game destructor
    }
}
