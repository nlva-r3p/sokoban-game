// Copyright 2025
// By Nguyen Mai

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <sstream>
#include <boost/test/unit_test.hpp>

#include "Sokoban.hpp"


BOOST_AUTO_TEST_CASE(testLevelLoading) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << "#####\n";
    ss << "#..A#\n";
    ss << "#.@a#\n";
    ss << "#...#\n";
    ss << "#####\n";

    SB::Sokoban game;
    ss >> game;

    BOOST_REQUIRE_EQUAL(game.height(), 5);
    BOOST_REQUIRE_EQUAL(game.width(), 5);
    BOOST_REQUIRE_EQUAL(game.pixelHeight(), 5 * SB::Sokoban::TILE_SIZE);
    BOOST_REQUIRE_EQUAL(game.pixelWidth(), 5 * SB::Sokoban::TILE_SIZE);
}

BOOST_AUTO_TEST_CASE(testPlayerPosition) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << "#####\n";
    ss << "#..A#\n";
    ss << "#.@.#\n";
    ss << "#a..#\n";
    ss << "#####\n";

    SB::Sokoban game;
    ss >> game;

    sf::Vector2u playerPos = game.playerLoc();
    BOOST_REQUIRE_EQUAL(playerPos.x, 2);
    BOOST_REQUIRE_EQUAL(playerPos.y, 2);
}

BOOST_AUTO_TEST_CASE(testWallCollision) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << "#####\n";
    ss << "#####\n";
    ss << "##@##\n";
    ss << "#####\n";
    ss << "#####\n";

    SB::Sokoban game;
    ss >> game;

    sf::Vector2u initialPos = game.playerLoc();

    game.movePlayer(SB::Direction::Up);
    BOOST_REQUIRE_EQUAL(game.playerLoc().x, initialPos.x);
    game.movePlayer(SB::Direction::Left);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, initialPos.y);
    game.movePlayer(SB::Direction::Down);
    BOOST_REQUIRE_EQUAL(game.playerLoc().x, initialPos.x);
    game.movePlayer(SB::Direction::Right);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, initialPos.y);
}

BOOST_AUTO_TEST_CASE(testCrateBlockedByWall) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << "...a.\n";
    ss << ".@...\n";
    ss << "..A..\n";
    ss << "..#..\n";
    ss << ".....\n";

    SB::Sokoban game;
    ss >> game;

    BOOST_REQUIRE_EQUAL(game.playerLoc().x, 1);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, 1);

    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Down);
    game.movePlayer(SB::Direction::Left);
    BOOST_REQUIRE_EQUAL(game.playerLoc().x, 1);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, 1);
}

BOOST_AUTO_TEST_CASE(testCrateBlockedByCrate) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << "...a.\n";
    ss << ".@...\n";
    ss << "..A..\n";
    ss << "..A..\n";
    ss << ".....\n";

    SB::Sokoban game;
    ss >> game;

    BOOST_REQUIRE_EQUAL(game.playerLoc().x, 1);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, 1);

    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Down);
    game.movePlayer(SB::Direction::Left);
    BOOST_REQUIRE_EQUAL(game.playerLoc().x, 1);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, 1);
}

BOOST_AUTO_TEST_CASE(testWinCondition) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << ".....\n";
    ss << ".@...\n";
    ss << "..A..\n";
    ss << "..a..\n";
    ss << ".....\n";

    SB::Sokoban game;
    ss >> game;

    BOOST_REQUIRE_EQUAL(game.isWon(), false);

    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Down);
    BOOST_REQUIRE_EQUAL(game.playerLoc().x, 2);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, 2);
    BOOST_REQUIRE_EQUAL(game.isWon(), true);
}

BOOST_AUTO_TEST_CASE(testMultipleStorageWin) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << ".....\n";
    ss << ".@A.a\n";
    ss << "...A.\n";
    ss << "...a.\n";
    ss << ".....\n";

    SB::Sokoban game;
    ss >> game;

    BOOST_REQUIRE_EQUAL(game.isWon(), false);

    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Down);

    BOOST_REQUIRE_EQUAL(game.isWon(), true);
}

BOOST_AUTO_TEST_CASE(testReset) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << ".....\n";
    ss << ".....\n";
    ss << "..@Aa\n";
    ss << ".....\n";
    ss << ".....\n";

    SB::Sokoban game;
    ss >> game;

    sf::Vector2u initialPos = game.playerLoc();

    game.movePlayer(SB::Direction::Right);

    BOOST_REQUIRE_EQUAL(game.isWon(), true);

    game.reset();

    BOOST_REQUIRE_EQUAL(game.playerLoc().x, initialPos.x);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, initialPos.y);
    BOOST_REQUIRE_EQUAL(game.isWon(), false);
}

BOOST_AUTO_TEST_CASE(testUndoRedo) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << "a...A\n";
    ss << ".....\n";
    ss << "..@..\n";
    ss << ".....\n";
    ss << ".....\n";

    SB::Sokoban game;
    ss >> game;

    sf::Vector2u initialPos = game.playerLoc();

    game.movePlayer(SB::Direction::Left);
    game.movePlayer(SB::Direction::Up);
    BOOST_REQUIRE_EQUAL(game.playerLoc().x, initialPos.x - 1);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, initialPos.y - 1);

    game.undo();
    game.undo();
    BOOST_REQUIRE_EQUAL(game.playerLoc().x, initialPos.x);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, initialPos.y);

    game.redo();
    game.redo();
    BOOST_REQUIRE_EQUAL(game.playerLoc().x, initialPos.x - 1);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, initialPos.y - 1);
}

BOOST_AUTO_TEST_CASE(testMoveOff) {
    std::stringstream ss;
    ss << "6 5\n";
    ss << ".....\n";
    ss << ".....\n";
    ss << "..aA.\n";
    ss << "..a..\n";
    ss << "..A..\n";
    ss << "..@..\n";

    SB::Sokoban game;
    ss >> game;

    sf::Vector2u initialPos = game.playerLoc();

    BOOST_REQUIRE_EQUAL(game.playerLoc().x, initialPos.x);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, initialPos.y);
    game.movePlayer(SB::Direction::Down);
    BOOST_REQUIRE_EQUAL(game.playerLoc().x, initialPos.x);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, initialPos.y);
}

BOOST_AUTO_TEST_CASE(testManyCrates) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << "...a.\n";
    ss << ".aAA.\n";
    ss << "..@Aa\n";
    ss << ".AAA.\n";
    ss << ".....\n";

    SB::Sokoban game;
    ss >> game;

    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Up);
    game.movePlayer(SB::Direction::Left);
    BOOST_REQUIRE_EQUAL(game.isWon(), true);
}

BOOST_AUTO_TEST_CASE(testManyTargets) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << "...a.\n";
    ss << ".aAA.\n";
    ss << "..@Aa\n";
    ss << ".aaa.\n";
    ss << ".....\n";

    SB::Sokoban game;
    ss >> game;

    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Up);
    game.movePlayer(SB::Direction::Left);
    BOOST_REQUIRE_EQUAL(game.isWon(), true);
}

BOOST_AUTO_TEST_CASE(testPushOffscreen) {
    std::stringstream ss;
    ss << "5 5\n";
    ss << "...a.\n";
    ss << ".aAA.\n";
    ss << "..@Aa\n";
    ss << ".aaaA\n";
    ss << ".....\n";

    SB::Sokoban game;
    ss >> game;

    sf::Vector2u initialPos = game.playerLoc();

    game.movePlayer(SB::Direction::Down);
    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Right);

    BOOST_REQUIRE_EQUAL(game.playerLoc().x, initialPos.x + 1);
    BOOST_REQUIRE_EQUAL(game.playerLoc().y, initialPos.y + 1);
}

BOOST_AUTO_TEST_CASE(testBoardSymbols) {
    std::stringstream ss;
    ss << "7 7\n";
    ss << "#######\n";
    ss << "#...a.#\n";
    ss << "#.aAA.#\n";
    ss << "#..@Aa#\n";
    ss << "#.aaaA#\n";
    ss << "#1....#\n";
    ss << "#######\n";

    SB::Sokoban game;
    ss >> game;

    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Up);
    game.movePlayer(SB::Direction::Left);
    game.movePlayer(SB::Direction::Down);

    game.movePlayer(SB::Direction::Down);
    game.movePlayer(SB::Direction::Right);
    game.movePlayer(SB::Direction::Right);

    ss.str("");
    ss.clear();
    ss << game;
    std::string actualString = ss.str();

    std::stringstream expected;
    expected << "7 7\n";
    expected << "#######\n";
    expected << "#...1.#\n";
    expected << "#.1...#\n";
    expected << "#....1#\n";
    expected << "#.aa@A#\n";
    expected << "#1....#\n";
    expected << "#######\n";

    std::string expectedString = expected.str();
    BOOST_REQUIRE_EQUAL(actualString, expectedString);
}
