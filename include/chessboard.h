#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <string.h>
#include <map>
#include <vector>
#include "utils.h"
#include "move.h"

using namespace std;

enum FieldContent
{
Empty = 0,
wPawn = 2,
bPawn,
wRook,
bRook,
wKnight,
bKnight,
wBishop,
bBishop,
wQueen,
bQueen,
wKing,
bKing,
Last = bKing
};

static const std::map<byte, std::string> fieldContentMap = 
{
    { 1, "\xe2\x99\x99" },     // Black Pawn
    { 2, "\xe2\x99\x9c" },     // Black Rook
    { 3, "\xe2\x99\x9e" },   // Black Knight
    { 4, "\xe2\x99\x9d" },   // Black Bishop
    { 5, "\xe2\x99\x9b" },    // Black Queen
    { 6, "\xe2\x99\x9a" }      // Black King
};


class ChessBoard {    
public:
    static constexpr unsigned char SIZE = 8;
private:    
    byte table[SIZE][SIZE];
    std::vector<byte> num_pieces;

private:

    inline void set(byte row, byte column, byte content) 
    {
        table[row][column] = content;
    }

public:
    
    ChessBoard();
    ChessBoard(const ChessBoard& r);

    bool is_clear_path(byte r1, byte c1, byte r2, byte c2) const;
    bool is_valid_move(byte r1, byte c1, byte r2, byte c2) const;
    bool is_check_for(byte color) const;
    bool is_game_over() const;

    bool put(byte row, byte column, FieldContent content);
    void move(byte r1, byte c1, byte r2, byte c2);

    void print_num_pieces() const;
    void show() const;
    
    inline byte get(byte row, byte column) const
    {
        return table[row][column];
    }

public:

    static ChessBoard create_initialized_board();
    static ChessBoard create_test_board();
    
    static void move_generator(
        ChessBoard board, 
        byte move_color, 
        vector<Move>& moves, 
        vector<vector<Move>>& closed_routs, 
        vector<vector<Move>>& open_routs, 
        byte depth = 0
    );

    static void print_move(Move move);
    static void print_route(vector<Move> route);
    static void playback(vector<Move> rout, ChessBoard board);
    void save_to_file(string filename, vector<vector<Move>>& routs);
};

#endif