#include "TicTacToe.h"

// -----------------------------------------------------------------------------
// TicTacToe.cpp
// -----------------------------------------------------------------------------
// This file is intentionally *full of comments* and gentle TODOs that guide you
// through wiring up a complete Tic‑Tac‑Toe implementation using the game engine’s
// Bit / BitHolder grid system.
//
// Rules recap:
//  - Two players place X / O on a 3x3 grid.
//  - Players take turns; you can only place into an empty square.
//  - First player to get three-in-a-row (row, column, or diagonal) wins.
//  - If all 9 squares are filled and nobody wins, it’s a draw.
//
// Notes about the provided engine types you'll use here:
//  - Bit              : a visual piece (sprite) that belongs to a Player
//  - BitHolder        : a square on the board that can hold at most one Bit
//  - Player           : the engine’s player object (you can ask who owns a Bit)
//  - Game options     : let the mouse know the grid is 3x3 (rowX, rowY)
//  - Helpers you’ll see used: setNumberOfPlayers, getPlayerAt, startGame, etc.
//
// I’ve already fully implemented PieceForPlayer() for you. Please leave that as‑is.
// The rest of the routines are written as “comment-first” TODOs for you to complete.
// -----------------------------------------------------------------------------

const int AI_PLAYER   = 1;      // index of the AI player (O)
const int HUMAN_PLAYER= 0;      // index of the human player (X)

TicTacToe::TicTacToe()
{
}

TicTacToe::~TicTacToe()
{
}

// -----------------------------------------------------------------------------
// make an X or an O
// -----------------------------------------------------------------------------
// DO NOT CHANGE: This returns a new Bit with the right texture and owner
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 1 ? "x.png" : "o.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

//
// setup the game board, this is called once at the start of the game
//
void TicTacToe::setUpBoard()
{
    // Set up 2 players
    setNumberOfPlayers(2);
    
    // Set grid dimensions to 3x3
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;
    
    // Initialize the 3x3 grid
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            ImVec2 position(100.0f + x * 100.0f, 100.0f + y * 100.0f);
            _grid[y][x].initHolder(position, "square.png", x, y);
        }
    }
    
    // Start the game
    startGame();
}

bool TicTacToe::actionForEmptyHolder(BitHolder *holder)
{
    // Guard clause: if holder is nullptr, fail fast
    if (!holder) return false;
    
    // Is it actually empty?
    if (holder->bit()) return false;
    
    // Place the current player's piece on this holder
    int currentPlayerIndex = getCurrentPlayer()->playerNumber();
    Bit *newBit = PieceForPlayer(currentPlayerIndex);
    newBit->setPosition(holder->getPosition());
    holder->setBit(newBit);
    
    return true;
}

bool TicTacToe::canBitMoveFrom(Bit *bit, BitHolder *src)
{
    // You can't move anything in tic tac toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst)
{
    // You can't move anything in tic tac toe
    return false;
}

void TicTacToe::stopGame()
{
    // Clear out the board
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            _grid[y][x].destroyBit();
        }
    }
}

Player* TicTacToe::ownerAt(int index) const
{
    // Convert index to x,y coordinates
    int y = index / 3;
    int x = index % 3;
    
    // Check if there's a bit at that location
    if (_grid[y][x].bit() == nullptr) {
        return nullptr;
    }
    
    // Return the owner of the bit
    return _grid[y][x].bit()->getOwner();
}

Player* TicTacToe::checkForWinner()
{
    const int winningTriples[8][3] = {
        {0, 1, 2}, 
        {3, 4, 5}, 
        {6, 7, 8},
        {0, 3, 6},
        {1, 4, 7}, 
        {2, 5, 8}, 
        {0, 4, 8}, 
        {2, 4, 6} 
    };
    
    // Check each winning combination
    for (int i = 0; i < 8; i++) {
        Player* p1 = ownerAt(winningTriples[i][0]);
        Player* p2 = ownerAt(winningTriples[i][1]);
        Player* p3 = ownerAt(winningTriples[i][2]);
        if (p1 != nullptr && p1 == p2 && p2 == p3) {
            return p1;
        }
    }
    
    return nullptr;
}

bool TicTacToe::checkForDraw()
{
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (_grid[y][x].bit() == nullptr) {
                return false; //if any empty square, return cuz its not a draw yet
            }
        }
    }
    
    return true;
}

std::string TicTacToe::initialStateString()
{
    return "000000000";
}

std::string TicTacToe::stateString() const
{
    std::string state = "";
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (_grid[y][x].bit() == nullptr) {
                state += "0";
            } else {
                int playerNum = _grid[y][x].bit()->getOwner()->playerNumber();
                state += std::to_string(playerNum + 1); // +1 to convert 0-based to 1-based
            }
        }
    }
    
    return state;
}

void TicTacToe::setStateString(const std::string &s)
{
    // empty the board
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            _grid[y][x].destroyBit();
        }
    }
    
    // use the state string
    for (int i = 0; i < 9 && i < (int)s.length(); i++) {
        int y = i / 3;
        int x = i % 3;
        int playerNumber = s[i] - '0';
        
        if (playerNumber == 1 || playerNumber == 2) {
            Bit* piece = PieceForPlayer(playerNumber - 1); 
            piece->setPosition(_grid[y][x].getPosition());
            _grid[y][x].setBit(piece);
        }
    }
}

int negamax(TicTacToe* game, int playerNumber, int depth, int alpha, int beta)
{
    // Check terminal
    Player* winner = game->checkForWinner();
    if (winner != nullptr) {
        // Return score as current player - if somethings broken its prob this
        if (winner->playerNumber() == playerNumber) {
            return 10 - depth;  
        } else {
            return -10 + depth; 
        }
    }
    
    // Check for draw
    if (game->checkForDraw()) {
        return 0;
    }
    
    int bestScore = -1000;
    int nextPlayer = 1 - playerNumber;
    
    // Iterate through all
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            BitHolder& holder = game->getHolderAt(x, y);
            if (holder.bit() == nullptr) {
                Bit* tempPiece = new Bit();
                //tempPiece->LoadTextureFromFile(playerNumber == 1 ? "x.png" : "o.png"); DONT LOAD THE GRAPHICS OF A TEMP PEICE SMART ONE maybe thats why it took 2 years to run
                tempPiece->setOwner(game->getPlayerAt(playerNumber));
                tempPiece->setPosition(holder.getPosition());
                holder.setBit(tempPiece);
                
                int score = -negamax(game, nextPlayer, depth + 1, -beta, -alpha);
                
                holder.destroyBit();
                
                bestScore = std::max(bestScore, score);
                alpha = std::max(alpha, score);
                
                if (alpha >= beta) {
                    return bestScore;
                }
            }
        }
    }
    
    return bestScore;
}

void TicTacToe::updateAI() 
{
    if (getCurrentPlayer()->playerNumber() != AI_PLAYER) {
        return;
    }
    
    int bestScore = -1000;
    int bestX = -1;
    int bestY = -1;
    int nextPlayer = HUMAN_PLAYER;
    
    // Try all possible moves
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            BitHolder& holder = getHolderAt(x, y);
            if (holder.bit() == nullptr) {
                Bit* tempPiece = PieceForPlayer(AI_PLAYER);
                tempPiece->setPosition(holder.getPosition());
                holder.setBit(tempPiece);
                
                int score = -negamax(this, nextPlayer, 1, -1000, 1000);
                
                holder.destroyBit();
                
                //Update best move
                if (score > bestScore) {
                    bestScore = score;
                    bestX = x;
                    bestY = y;
                    
                    if (bestScore >= 10) {
                        break;
                    }
                }
            }
        }
        //exit if inning move
        if (bestScore >= 10) {
            break;
        }
    }
    
    if (bestX != -1 && bestY != -1) {
        BitHolder& holder = getHolderAt(bestX, bestY);
        if (actionForEmptyHolder(&holder)) {
            endTurn();
        }
    }
}