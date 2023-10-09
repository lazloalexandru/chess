#include <iostream>
#include <chrono>
#include "chessboard.h"

using namespace std;
using chrono::milliseconds;


void test_move_generator()
{
    ChessBoard b = ChessBoard::create_test_board3();
    vector<ChessBoard> v;
    v.reserve(100);
    v.push_back(b);
    v.push_back(b);
    v.push_back(b);
    Move m;

    cout << "Chessboard Size: " << sizeof(b) << " bytes" << endl;
    cout << "Move Size: " << sizeof(m) << " bytes" << endl;

    vector<vector<Move>> closed_routs;
    vector<vector<Move>> open_routs;

    auto start = chrono::steady_clock::now();

    ChessBoard::move_generator(b, 0, closed_routs, open_routs, 5);

    auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
    cout << "Elapsed time in milliseconds: " << duration << " ms\n";

    
    if (closed_routs.size())
    {
        ChessBoard::playback(closed_routs[0], b);
        ChessBoard::save_to_file("finishes.txt", closed_routs);
    }
    else
    {
        b.show();
    }
}

void show_danger_routes(const ChessBoard& b, byte color)
{
    vector<vector<Move>> closed_routs;
    vector<vector<Move>> open_routs;

    auto start = chrono::steady_clock::now();

    static const int depth = 4;
    ChessBoard::move_generator(b, color, closed_routs, open_routs, depth);

    auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();

    if (closed_routs.size())
    {
        for (auto route:closed_routs)
        {
            ChessBoard::print_route(route);
            cout << endl;
        }
    }
}

void game_play()
{
    ChessBoard b = ChessBoard::create_test_board3();
    byte color = 0;
    
    while (!b.is_game_over(color))
    {
        show_danger_routes(b, color);
        b.show();
        
        bool valid_move = false;
        while (!valid_move)
        {
            string move_str;
            cout << (color ? white_white:black_white) << "Enter Move:" << ORIG_COLOR;
            getline(cin, move_str);
    
            Move m;
            valid_move = str_to_Move(move_str, m);

            if (valid_move)
            {
                m.color = color;
                valid_move = b.move(m);
            }
        }
        color = (~color) & 0x1;
    }

    b.show();
    cout << "Game Over!" << endl;
}

int main() {
    
    game_play();

    return 0;
}
