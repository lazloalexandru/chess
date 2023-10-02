#include <iostream>
#include <chrono>
#include "chessboard.h"

using namespace std;
using chrono::milliseconds;


void test_move_generator()
{
    ChessBoard board = ChessBoard::create_test_board();

    vector<Move> moves;
    vector<vector<Move>> closed_routs;
    vector<vector<Move>> open_routs;

    auto start = chrono::steady_clock::now();

    ChessBoard::move_generator(board, 1, moves, closed_routs, open_routs);

    auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
    cout << "Elapsed time in milliseconds: " << duration << " ms\n";

    ChessBoard::playback(closed_routs[459], board);

}

int main() {
    test_move_generator();
    
    return 0;
}
