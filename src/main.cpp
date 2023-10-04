#include <iostream>
#include <chrono>
#include "chessboard.h"

using namespace std;
using chrono::milliseconds;


void test_move_generator()
{
    //ChessBoard board = ChessBoard::create_initialized_board();
    ChessBoard board = ChessBoard::create_test_board3();

    vector<vector<Move>> closed_routs;
    vector<vector<Move>> open_routs;

    auto start = chrono::steady_clock::now();

    ChessBoard::move_generator(board, 0, closed_routs, open_routs, 5);

    auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
    cout << "Elapsed time in milliseconds: " << duration << " ms\n";

    
    if (closed_routs.size())
    {
        ChessBoard::playback(closed_routs[0], board);
        ChessBoard::save_to_file("finishes.txt", closed_routs);
    }
    else
    {
        board.show();
    }
}

int main() {    
    ChessBoard b;
    vector<ChessBoard> v;
    v.reserve(100);
    v.push_back(b);
    v.push_back(b);
    v.push_back(b);
    Move m;

    cout << "Chessboard Size: " << sizeof(b) << " bytes" << endl;
    cout << "Move Size: " << sizeof(m) << " bytes" << endl;
    
    test_move_generator();

    return 0;
}
