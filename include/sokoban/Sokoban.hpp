// Copyright 2025
// By Nguyen Mai

#pragma once

#include <iostream>
#include <string>
#include <unordered_map>  // for the hash table, O(1) lookup, or O(n) for worst case
#include <memory>  // for shared_ptr
#include <vector>  // for TileClassifier to store shared_ptrs
#include <cstdlib>  // for random number generation
#include <stack>  // for undo and redo functions
#include <sstream>  // for reading in the level file
#include <functional>  // for std::function for gameKeyStates in main
#include <algorithm>  // for iterators & find()

#include <SFML/Graphics.hpp>

namespace SB {
enum class Direction {
    Up, Down, Left, Right
};

/*
*  Background: GROUNDS, HOLE, GROUND_OUTLINES
*  Foreground: WALLS, OUTLINES, CRATES, HOLE_CRATES, LOCKED_CRATE,
               DIM_CRATES, DIM_HOLE_CRATES, FALLING_CRATES, LOCKED_HOLE_CRATES,
               COINS, PLAYER
*/

enum class TileType {
    // ENVIRONMENT
    PLAYER = '@',
    GROUNDS = '.',
    WALLS = '#',
    CRATES = 'A',
    HOLE = 'H',
    COINS = 'C',

    // LOSE
    LOCKED_CRATE = 'L',

    // GOAL
    GROUND_OUTLINES = 'a',
    OUTLINES = 'o',
    DIM_CRATES = 'D',
    DIM_HOLE_CRATES = 'd',
    FALLING_CRATES = 'F',
    HOLE_CRATES = '1',
    LOCKED_HOLE_CRATES = 'l'
};

struct Tile {
    TileType type;
    sf::Sprite sprite;
};

class Sokoban;

class TileClassifier {
 public:
    TileClassifier() : _textureHashTable(14) {
      auto classifyAnimation = [&](Direction dir, const std::string& filename) {
         auto texture = std::make_shared<sf::Texture>();
         std::string baseName = filename.substr(filename.find_last_of('/') + 1);
         if (!texture->loadFromFile(filename) && !texture->loadFromFile("./" + baseName)) {
            sf::Image image;
            image.create(1, 1, _defaultHashTable.at(TileType::PLAYER));
            texture->loadFromImage(image);
         }
         _animationHashTable[dir].push_back(texture);
      };

      auto classifyTexture = [&](TileType type, const std::string& filename) {
         auto texture = std::make_shared<sf::Texture>();
         std::string baseName = filename.substr(filename.find_last_of('/') + 1);
         if (!texture->loadFromFile(filename) && !texture->loadFromFile("./" + baseName)) {
            sf::Image image;
            image.create(1, 1, _defaultHashTable.at(type));
            texture->loadFromImage(image);
         }
         _textureHashTable[type].push_back(texture);
      };


      // ANIMATIONS
      classifyAnimation(Direction::Down, "./sokoban/Player/player_05.png");
      classifyAnimation(Direction::Down, "./sokoban/Player/player_06.png");
      classifyAnimation(Direction::Down, "./sokoban/Player/player_07.png");
      classifyAnimation(Direction::Up, "./sokoban/Player/player_08.png");
      classifyAnimation(Direction::Up, "./sokoban/Player/player_09.png");
      classifyAnimation(Direction::Up, "./sokoban/Player/player_10.png");
      classifyAnimation(Direction::Right, "./sokoban/Player/player_17.png");
      classifyAnimation(Direction::Right, "./sokoban/Player/player_18.png");
      classifyAnimation(Direction::Right, "./sokoban/Player/player_19.png");
      classifyAnimation(Direction::Left, "./sokoban/Player/player_20.png");
      classifyAnimation(Direction::Left, "./sokoban/Player/player_21.png");
      classifyAnimation(Direction::Left, "./sokoban/Player/player_22.png");

      // HOLE CRATES
      classifyTexture(TileType::HOLE_CRATES, "./sokoban/Crates/crate_01.png");
      classifyTexture(TileType::HOLE_CRATES, "./sokoban/Crates/crate_42.png");
      classifyTexture(TileType::HOLE_CRATES, "./sokoban/Crates/crate_43.png");
      classifyTexture(TileType::HOLE_CRATES, "./sokoban/Crates/crate_44.png");
      classifyTexture(TileType::HOLE_CRATES, "./sokoban/Crates/crate_45.png");
      // HOLE CRATES BRICK-TEXTURE
      classifyTexture(TileType::WALLS, "./sokoban/Blocks/block_04.png");
      classifyTexture(TileType::WALLS, "./sokoban/Blocks/block_05.png");
      classifyTexture(TileType::WALLS, "./sokoban/Blocks/block_06.png");
      classifyTexture(TileType::WALLS, "./sokoban/Blocks/block_07.png");

      // CRATES
      classifyTexture(TileType::CRATES, "./sokoban/Crates/crate_02.png");
      classifyTexture(TileType::CRATES, "./sokoban/Crates/crate_03.png");
      classifyTexture(TileType::CRATES, "./sokoban/Crates/crate_04.png");
      classifyTexture(TileType::CRATES, "./sokoban/Crates/crate_05.png");
      classifyTexture(TileType::CRATES, "./sokoban/Crates/crate_06.png");
      // CRATES BRICK-TEXTURE
      classifyTexture(TileType::CRATES, "./sokoban/Blocks/block_01.png");
      classifyTexture(TileType::CRATES, "./sokoban/Blocks/block_02.png");
      classifyTexture(TileType::CRATES, "./sokoban/Blocks/block_03.png");
      classifyTexture(TileType::CRATES, "./sokoban/Blocks/block_08.png");

      // DIM HOLE CRATES
      classifyTexture(TileType::DIM_HOLE_CRATES, "./sokoban/Crates/crate_07.png");
      classifyTexture(TileType::DIM_HOLE_CRATES, "./sokoban/Crates/crate_08.png");
      classifyTexture(TileType::DIM_HOLE_CRATES, "./sokoban/Crates/crate_09.png");
      classifyTexture(TileType::DIM_HOLE_CRATES, "./sokoban/Crates/crate_10.png");
      classifyTexture(TileType::DIM_HOLE_CRATES, "./sokoban/Crates/crate_11.png");

      // DIM CRATES
      classifyTexture(TileType::DIM_CRATES, "./sokoban/Crates/crate_12.png");
      classifyTexture(TileType::DIM_CRATES, "./sokoban/Crates/crate_13.png");
      classifyTexture(TileType::DIM_CRATES, "./sokoban/Crates/crate_14.png");
      classifyTexture(TileType::DIM_CRATES, "./sokoban/Crates/crate_15.png");
      classifyTexture(TileType::DIM_CRATES, "./sokoban/Crates/crate_16.png");

      // LOCKED HOLE CRATES
      classifyTexture(TileType::LOCKED_HOLE_CRATES, "./sokoban/Crates/crate_17.png");
      classifyTexture(TileType::LOCKED_HOLE_CRATES, "./sokoban/Crates/crate_18.png");
      classifyTexture(TileType::LOCKED_HOLE_CRATES, "./sokoban/Crates/crate_19.png");
      classifyTexture(TileType::LOCKED_HOLE_CRATES, "./sokoban/Crates/crate_20.png");
      classifyTexture(TileType::LOCKED_HOLE_CRATES, "./sokoban/Crates/crate_21.png");

      // LOCKED CRATES
      classifyTexture(TileType::LOCKED_CRATE, "./sokoban/Crates/crate_22.png");
      classifyTexture(TileType::LOCKED_CRATE, "./sokoban/Crates/crate_23.png");
      classifyTexture(TileType::LOCKED_CRATE, "./sokoban/Crates/crate_24.png");
      classifyTexture(TileType::LOCKED_CRATE, "./sokoban/Crates/crate_25.png");
      classifyTexture(TileType::LOCKED_CRATE, "./sokoban/Crates/crate_26.png");

      // FALLING CRATES
      classifyTexture(TileType::FALLING_CRATES, "./sokoban/Crates/crate_37.png");
      classifyTexture(TileType::FALLING_CRATES, "./sokoban/Crates/crate_38.png");
      classifyTexture(TileType::FALLING_CRATES, "./sokoban/Crates/crate_39.png");
      classifyTexture(TileType::FALLING_CRATES, "./sokoban/Crates/crate_40.png");
      classifyTexture(TileType::FALLING_CRATES, "./sokoban/Crates/crate_41.png");

      // COINS
      classifyTexture(TileType::COINS, "./sokoban/Environment/environment_11.png");
      classifyTexture(TileType::COINS, "./sokoban/Environment/environment_12.png");

      // OUTLINES
      classifyTexture(TileType::OUTLINES, "./sokoban/Environment/environment_03.png");
      classifyTexture(TileType::OUTLINES, "./sokoban/Environment/environment_06.png");
      classifyTexture(TileType::OUTLINES, "./sokoban/Environment/environment_16.png");

      // HOLE
      classifyTexture(TileType::HOLE, "./sokoban/Environment/environment_15.png");

      // GROUND OUTLINES
      classifyTexture(TileType::GROUND_OUTLINES, "./sokoban/Ground/ground_04.png");
      classifyTexture(TileType::GROUND_OUTLINES, "./sokoban/Ground/ground_02.png");
      classifyTexture(TileType::GROUND_OUTLINES, "./sokoban/Ground/ground_03.png");

      // GROUNDS
      classifyTexture(TileType::GROUNDS, "./sokoban/Ground/ground_01.png");
      classifyTexture(TileType::GROUNDS, "./sokoban/Ground/ground_05.png");
      classifyTexture(TileType::GROUNDS, "./sokoban/Ground/ground_06.png");
    }

    Tile createTile(char c, std::shared_ptr<unsigned int> seed) const {
         auto tileType = static_cast<TileType>(c);
         if (tileType == TileType::PLAYER) {
            return Tile {
               tileType,
               sf::Sprite(*((_animationHashTable.at(Direction::Down))[0]))
            };
         }
         // loads default if missing texture images
         if (_textureHashTable.find(tileType) == _textureHashTable.end()) {
            auto it = _defaultTextureHashTable.find(tileType);
            if (it == _defaultTextureHashTable.end()) {
               auto defaultTexture = std::make_shared<sf::Texture>();
               sf::Image image;
               image.create(1, 1, _defaultHashTable.at(tileType));
               defaultTexture->loadFromImage(image);
               _defaultTextureHashTable[tileType] = defaultTexture;
            }
            return Tile {
               tileType,
               sf::Sprite(*(_defaultTextureHashTable.at(tileType)))
            };
         }
         unsigned int temp = *seed;
         int random_index = rand_r(&temp) % (_textureHashTable.at(tileType)).size();
         return Tile {
            tileType,
            sf::Sprite(*((_textureHashTable.at(tileType))[random_index]))
         };
    }

    Tile createTile(char c) const {
         auto tileType = static_cast<TileType>(c);
         if (tileType == TileType::PLAYER) {
            return Tile {
               tileType,
               sf::Sprite(*((_animationHashTable.at(Direction::Down))[0]))
            };
         }
         // loads default if missing texture images
         if (_textureHashTable.find(tileType) == _textureHashTable.end()) {
            auto it = _defaultTextureHashTable.find(tileType);
            if (it == _defaultTextureHashTable.end()) {
               auto defaultTexture = std::make_shared<sf::Texture>();
               sf::Image image;
               image.create(1, 1, _defaultHashTable.at(tileType));
               defaultTexture->loadFromImage(image);
               _defaultTextureHashTable[tileType] = defaultTexture;
            }
            return Tile {
               tileType,
               sf::Sprite(*(_defaultTextureHashTable.at(tileType)))
            };
         }
         return Tile {
            tileType,
            sf::Sprite(*((_textureHashTable.at(tileType))[0]))
         };
    }

    inline static Tile getAnimation(Direction dir, std::shared_ptr<unsigned int> index);

 private:
    // containing default colors in case of missing texture
    inline static std::unordered_map<TileType, sf::Color> _defaultHashTable {
      {TileType::PLAYER,               sf::Color::White},           // White
      {TileType::CRATES,               sf::Color::Yellow},          // Yellow
      {TileType::HOLE_CRATES,          sf::Color::Black},           // Black
      {TileType::DIM_HOLE_CRATES,      sf::Color(128, 128, 128)},   // Grey
      {TileType::DIM_CRATES,           sf::Color::Magenta},         // Magenta
      {TileType::LOCKED_HOLE_CRATES,   sf::Color::Cyan},            // Cyan
      {TileType::LOCKED_CRATE,         sf::Color::Red},             // Red
      {TileType::FALLING_CRATES,       sf::Color(255, 165, 0)},     // Orange
      {TileType::COINS,                sf::Color::Blue},            // Blue
      {TileType::OUTLINES,             sf::Color(128, 0, 128)},     // Purple
      {TileType::HOLE,                 sf::Color::Green},           // Green
      {TileType::GROUND_OUTLINES,      sf::Color(139, 69, 19)},     // Brown
      {TileType::GROUNDS,              sf::Color(0, 100, 0)},       // Dark Green
      {TileType::WALLS,                sf::Color(255, 215, 0)}      // Gold
    };

    // containing textures for each animation
    inline static std::unordered_map<Direction, std::vector<
                                 std::shared_ptr<
                                 sf::Texture>>> _animationHashTable{4};

    // containing textures for each tile type
    std::unordered_map<TileType, std::vector<
                                 std::shared_ptr<
                                 sf::Texture>>> _textureHashTable;

    // containing default textures for tile types of missing texture
    inline static std::unordered_map<TileType,
                  std::shared_ptr<sf::Texture>> _defaultTextureHashTable;

    // containing default animations for player of missing texture
    inline static std::unordered_map<Direction, std::vector<
                                 std::shared_ptr<
                                 sf::Texture>>> _defaultAnimationHashTable;
};

class Sokoban : public sf::Drawable {
 public:
    static const int TILE_SIZE = 64;

    Sokoban();
    explicit Sokoban(std::shared_ptr<unsigned int> seed);  // to randomize the textures of the game

    explicit Sokoban(const std::string&);  // Optional

    unsigned int pixelHeight() const { return _height * TILE_SIZE; }  // Optional
    unsigned int pixelWidth() const { return _width * TILE_SIZE; }  // Optional

    // returns the dimensions of the game board
    unsigned int height() const { return _height; }
    unsigned int width() const { return _width; }

    // returns the floor tile
    Tile floor() const { return _floor; }

    // returns the player's current position, with (0, 0) as the top-left corner
    sf::Vector2u playerLoc() const;

    // saves the player's current position, with (0, 0) as the top-left corner
    static void savePlayerLoc(sf::Vector2u position) { _playerPosition = position; }

    // saves the player's current direction
    static void savePlayerDirection(Direction dir) { _playerDirection = dir; }

    // returns true if the player has won the game
    bool isWon() const;

    // takes a Direction and moves the player in that direction
    void movePlayer(Direction dir);

    // Get the current move count
    unsigned int getMoveCount() const { return _moveCount; }

    // changing game state
    void reset();
    void undo();  // Optional XC
    void redo();  // Optional XC

    friend std::ostream& operator<<(std::ostream& out, const Sokoban& s);
    friend std::istream& operator>>(std::istream& in, Sokoban& s);

 protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 private:
    TileClassifier _tileClassifier;
    struct _gameState {
      std::vector<Tile> gameBoard;
      sf::Vector2u playerPosition;
      Direction playerDirection;
      unsigned int moveCount;
    };
    std::stack<_gameState> _stack;
    std::stack<_gameState> _undoStack;
    std::stack<_gameState> _redoStack;
    std::vector<Tile> _initialBoard;
    std::vector<Tile> _gameBoard;
    inline static sf::Vector2u _playerPosition{0, 0};
    static std::vector<sf::Vector2u> _storagePositions;
    inline static Direction _playerDirection{Direction::Down};  // helps to draw the player
    std::shared_ptr<unsigned int> _frameIndex = std::make_shared<unsigned int>(0);
    std::shared_ptr<unsigned int> _seed;
    unsigned int _height;
    unsigned int _width;
    unsigned int _moveCount{0};
    inline static unsigned int _boxCount{0};
    Tile _floor;  // helps to draw the floor of the level
    Tile _outline;  // helps to draw the ground outlines

    void _saveState() {
      _stack.push({_gameBoard, _playerPosition, _playerDirection, _moveCount});
    }
    void _saveStateUndo() {
      _undoStack.push({_gameBoard, _playerPosition, _playerDirection, _moveCount});
    }
    void _saveStateRedo() {
      _redoStack.push({_gameBoard, _playerPosition, _playerDirection, _moveCount});
    }

    friend class TileClassifier;
};

std::ostream& operator<<(std::ostream& out, const Sokoban& s);
std::istream& operator>>(std::istream& in, Sokoban& s);

inline Tile TileClassifier::getAnimation(Direction dir, std::shared_ptr<unsigned int> index) {
    if (dir == Sokoban::_playerDirection) {
         *index += 1;
    } else {
         *index = 1;
    }
    // loads default if missing animation
    if (_animationHashTable.find(dir) == _animationHashTable.end() ||
        _animationHashTable.at(dir).empty()) {
        if (_defaultAnimationHashTable.find(dir) == _defaultAnimationHashTable.end()) {
            auto defaultTexture = std::make_shared<sf::Texture>();
            sf::Image image;
            image.create(1, 1, _defaultHashTable.at(TileType::PLAYER));
            defaultTexture->loadFromImage(image);
            // store it for default use
            _defaultAnimationHashTable[dir] = {defaultTexture, defaultTexture, defaultTexture};
        }
        return Tile {
            TileType::PLAYER,
            sf::Sprite(*(_defaultAnimationHashTable.at(dir)[0]))
        };
    }
    return Tile {
         TileType::PLAYER,
         sf::Sprite(*((_animationHashTable.at(dir))[(*index) % 3]))
    };
}
}  // namespace SB
