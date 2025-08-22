// Copyright 2025
// By Nguyen Mai

#include <fstream>  // for ifs
#include "sokoban/Sokoban.hpp"

namespace SB {
std::vector<sf::Vector2u> Sokoban::_storagePositions;

Sokoban::Sokoban() : _tileClassifier(),
_frameIndex(std::make_shared<unsigned int>(0)),
_seed(std::make_shared<unsigned int>(0)),
_height(0), _width(0),
_floor(_tileClassifier.createTile('.'))
{}
Sokoban::Sokoban(std::shared_ptr<unsigned int> seed) : _tileClassifier(),
_frameIndex(std::make_shared<unsigned int>(0)),
_seed(seed),
_height(0), _width(0),
_floor(_tileClassifier.createTile('.', seed))
{}

Sokoban::Sokoban(const std::string& filename) : _tileClassifier(),
_frameIndex(std::make_shared<unsigned int>(0)),
_seed(std::make_shared<unsigned int>(0)),
_height(0), _width(0),
_floor(_tileClassifier.createTile('.')) {
    std::ifstream ifs(filename, std::ifstream::in);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open " + filename);
    }
    ifs >> *this;
}

void Sokoban::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    auto drawTile = [&](size_t x, size_t y, const Tile& tile) {
        sf::Sprite qTile = tile.sprite;
        // find the image size and scale it to match given TILE_SIZE
        // ideally, the images should be the size of TILE_SIZE
        qTile.scale(TILE_SIZE / qTile.getLocalBounds().height,
                    TILE_SIZE / qTile.getLocalBounds().width);
        qTile.setPosition(x * TILE_SIZE, y * TILE_SIZE);
        // if (tile.type == TileType::WIN) {
        //     qTile.setColor(sf::Color::Green);
        // }
        return qTile;
    };

    auto isStorageLocation = [&](unsigned int x, unsigned int y) {
        sf::Vector2u pos(x, y);
        for (const auto& storagePos : _storagePositions) {
            if (storagePos == pos) {
                return true;
            }
        }
        return false;
    };

    // row-major order
    for (unsigned int y = 0; y < height(); y++) {
        for (unsigned int x = 0; x < width(); x++) {
            unsigned int i = y * width() + x;
            // check if a background tile is already drawn
            // if none, then draw one
            bool isNotBackground = _gameBoard[i].type != TileType::GROUNDS &&
                                _gameBoard[i].type != TileType::HOLE &&
                                _gameBoard[i].type != TileType::GROUND_OUTLINES;
            // if player is on storage location, draw outline instead of floor
            if (_gameBoard[i].type == TileType::PLAYER && isStorageLocation(x, y)) {
                target.draw(drawTile(x, y, _outline), states);
            } else if (isNotBackground) {
                target.draw(drawTile(x, y, _floor), states);
            }
            target.draw(drawTile(x, y, _gameBoard[i]), states);
        }
    }
}

sf::Vector2u Sokoban::playerLoc() const {
    sf::Vector2u playerLocation;
    bool playerLocated = false;
    for (size_t i = 0; i < _gameBoard.size(); i++) {
        if (_gameBoard[i].type == TileType::PLAYER) {
            playerLocation = {
                // row-major order
                static_cast<unsigned int>(i % width()),
                static_cast<unsigned int>(i / width())
            };
            playerLocated = true;
        }
    }
    if (!playerLocated) {
        throw std::runtime_error("No player found");
    }

    return playerLocation;
}

void Sokoban::movePlayer(Direction dir) {
    auto getNewPos = [](Direction dir, sf::Vector2u currentPos) -> sf::Vector2u {
        switch (dir) {
            case Direction::Up:
                return {currentPos.x, currentPos.y - 1};
            case Direction::Down:
                return {currentPos.x, currentPos.y + 1};
            case Direction::Left:
                return {currentPos.x - 1, currentPos.y};
            case Direction::Right:
                return {currentPos.x + 1, currentPos.y};
        }
        return currentPos;
    };

    auto isOnTopOfStorageLocation = [&](sf::Vector2u pos) {
        for (auto it = _storagePositions.begin(); it != _storagePositions.end(); it++) {
            if (*it == pos) {
                return true;
            }
        }
        return false;
    };

    auto isNotValidMove = [&](sf::Vector2u pos) {
        return pos.x >= width() || pos.y >= height();
    };

    auto vectorToIndex = [&](sf::Vector2u pos) {
        return pos.y * width() + pos.x;
    };

    sf::Vector2u playerPos = playerLoc();
    sf::Vector2u newPlayerPos = getNewPos(dir, playerPos);
    if (isNotValidMove(newPlayerPos)) {
        return;
    }
    size_t indexPlayer = vectorToIndex(playerPos);
    size_t indexNewPlayer = vectorToIndex(newPlayerPos);

    // if player moves, then moveMade is true
    // otherwise, if player runs into a wall, then
    // moveMade is false
    bool moveMade = false;

    // new tile contains a crate
    if (_gameBoard[indexNewPlayer].type == TileType::CRATES ||
        _gameBoard[indexNewPlayer].type == TileType::HOLE_CRATES) {
        sf::Vector2u newBoxPos = getNewPos(dir, newPlayerPos);
        if (isNotValidMove(newBoxPos)) {
            return;
        }
        size_t indexNewBox = vectorToIndex(newBoxPos);
        if (_gameBoard[indexNewBox].type != TileType::WALLS &&
            _gameBoard[indexNewBox].type != TileType::LOCKED_CRATE &&
            _gameBoard[indexNewBox].type != TileType::CRATES &&
            _gameBoard[indexNewBox].type != TileType::HOLE_CRATES) {
            // if crate on storage location
            if (isOnTopOfStorageLocation(newBoxPos)) {
                // use HOLE_CRATES type when pushing crate onto a storage location
                _gameBoard[indexNewBox] =
                    _tileClassifier.createTile(static_cast<char>(TileType::HOLE_CRATES), _seed);
            } else {
                // regular crate on regular floor
                _gameBoard[indexNewBox] =
                    _tileClassifier.createTile(static_cast<char>(TileType::CRATES), _seed);
            }

            _gameBoard[indexNewPlayer] = TileClassifier::getAnimation(dir, _frameIndex);

            // check if we pushed a crate that was on a storage location (HOLE_CRATES)
            // if so, restore the storage location, otherwise use regular floor
            if (isOnTopOfStorageLocation(playerPos)) {
                _gameBoard[indexPlayer] = _outline;
            } else {
                _gameBoard[indexPlayer] = _floor;
            }
            savePlayerLoc(newPlayerPos);
            savePlayerDirection(dir);
            moveMade = true;
        }
    } else if (_gameBoard[indexNewPlayer].type == TileType::WALLS) {
        // cannot move into a wall
        return;
    } else {
        // no objects in the way
        _gameBoard[indexNewPlayer] = TileClassifier::getAnimation(dir, _frameIndex);
        // Check if player was standing on storage location and restore it if needed
        if (isOnTopOfStorageLocation(playerPos)) {
            _gameBoard[indexPlayer] = _outline;
        } else {
            _gameBoard[indexPlayer] = _floor;
        }
        savePlayerLoc(newPlayerPos);
        savePlayerDirection(dir);
        moveMade = true;
    }

    // count moves when player moves
    if (moveMade) {
        _moveCount++;
        _saveState();
    }

    while (!_redoStack.empty()) {
        _redoStack.pop();
    }
}

bool Sokoban::isWon() const {
    if (_storagePositions.empty()) {
        return true;
    }
    if (_boxCount == 0) {
        return true;
    }
    unsigned int matchedCount = 0;
    for (const auto& storagePos : _storagePositions) {
        if (_gameBoard[storagePos.y * width() + storagePos.x].type == TileType::HOLE_CRATES) {
            matchedCount += 1;
        }
    }
    if (_boxCount >= _storagePositions.size()) {
        return matchedCount == _storagePositions.size();
    } else {
        return matchedCount == _boxCount;
    }
}

void Sokoban::reset() {
    _gameBoard = _initialBoard;
    _moveCount = 0;
    while (!_undoStack.empty()) {
        _undoStack.pop();
    }
    while (!_redoStack.empty()) {
        _redoStack.pop();
    }
    _saveState();
}

void Sokoban::undo() {
    if (_stack.size() == 1) {
        return;
    }
    _saveStateRedo();
    _stack.pop();
    _undoStack = _stack;
    if (_undoStack.empty()) {
        return;
    }
    _gameBoard = _undoStack.top().gameBoard;
    savePlayerLoc(_undoStack.top().playerPosition);
    savePlayerDirection(_undoStack.top().playerDirection);
    _moveCount = _undoStack.top().moveCount;
    _undoStack.pop();
}

void Sokoban::redo() {
    if (_redoStack.empty()) {
        return;
    }
    _saveStateUndo();
    _gameBoard = _redoStack.top().gameBoard;
    savePlayerLoc(_redoStack.top().playerPosition);
    savePlayerDirection(_redoStack.top().playerDirection);
    _moveCount = _redoStack.top().moveCount;
    _saveState();
    _redoStack.pop();
}

std::istream& operator>>(std::istream& in, Sokoban& game) {
    game._storagePositions.clear();
    game._gameBoard.clear();
    game._boxCount = 0;

    std::string line;
    std::getline(in, line);
    std::istringstream iss(line);
    iss >> game._height >> game._width;

    if (game.width() <= 0 || game.height() <= 0) {
        throw std::runtime_error("Invalid dimensions");
    }
    game._gameBoard.resize(game.width() * game.height());

    unsigned int lineCount = 0;
    while (std::getline(in, line) && lineCount < game.height()) {
        for (unsigned int i = 0; i < line.size() && i < game.width(); i++) {
            Tile tile = game._tileClassifier.createTile(line[i], game._seed);
            if (tile.type == TileType::PLAYER) {
                game.savePlayerLoc({i, lineCount});
                game.savePlayerDirection(Direction::Down);
            }
            if (tile.type == TileType::GROUND_OUTLINES) {
                game._outline = tile;
                game._storagePositions.push_back({i, lineCount});
            }
            if (tile.type == TileType::HOLE_CRATES) {
                game._storagePositions.push_back({i, lineCount});
                game._boxCount++;
            }
            if (tile.type == TileType::CRATES) {
                game._boxCount++;
            }
            game._gameBoard[lineCount * game.width() + i] = tile;
        }
        lineCount++;
    }
    game._initialBoard = game._gameBoard;
    game._moveCount = 0;
    game._saveState();
    return in;
}

std::ostream& operator<<(std::ostream& out, const Sokoban& game) {
    out << game.height() << " " << game.width() << std::endl;
    for (unsigned int y = 0; y < game.height(); y++) {
        for (unsigned int x = 0; x < game.width(); x++) {
            unsigned int i = y * game.width() + x;
            out << static_cast<char>(game._gameBoard[i].type);
        }
        out << std::endl;
    }
    return out;
}
}  // namespace SB
