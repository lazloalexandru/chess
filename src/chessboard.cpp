#include <iostream>
#include <fstream>
#include <sstream>
#include "chessboard.h"
#include "move.h"

using namespace std;


ChessBoard::ChessBoard() {
    memset(table, 0x0, sizeof(table));
    num_pieces.resize(FieldContent::Last + 1); 
    num_pieces[Empty] = 64;
}

ChessBoard::ChessBoard(const ChessBoard& other) {
    for (unsigned char i = 0; i < SIZE; ++i) {
        for (unsigned char j = 0; j < SIZE; ++j) {
            table[i][j] = other.table[i][j];
        }
    }

    num_pieces = other.num_pieces;
}

void ChessBoard::print_num_pieces() const
{
    cout << "Piece count: ";
    for (byte piece_count : num_pieces)
    {
        cout << dec <<  ((int)piece_count) << " ";
    }
    cout << endl;
}

bool ChessBoard::is_game_over() const
{
    return (num_pieces[wKing] == 0) && ((num_pieces[bKing] > 0));
}
    

void ChessBoard::move(byte r1, byte c1, byte r2, byte c2)
{
    byte f1 = get(r1,c1);
    byte f2 = get(r2,c2);

    set(r1,c1, FieldContent::Empty);
    set(r2,c2, f1);

    num_pieces[Empty]++;
    num_pieces[f2]--;
}

void ChessBoard::show() const
{
    cout << "  ";

    for (int i=1; i<=SIZE; i++)
    {
        cout << ORIG_COLOR << i;
    }

    cout << "\n";

    for (int i=0; i<SIZE; i++)
    {
        cout << ORIG_COLOR << (char)('A' + i) << " ";

        for (int j=0; j<SIZE; j++)
        {
            byte field = table[i][j];
            byte piece = field >> 1;

            string color;

            if ((i+j) % 2 > 0)
            {
                color = field & 0x1 ? white_black:black_black;
                    
            }
            else
            {
                color = field & 0x1 ? white_white:black_white;
            }
            
            if (field)
            {
                cout << color << fieldContentMap.at(piece);
            }
            else
            {
                cout << color << " ";
            }
        }

        cout << ORIG_COLOR << endl;
    }
}

bool ChessBoard::put(byte row, byte column, FieldContent content)
{
    row = row - 'A';
    column = column - 1;
    bool result = false;

    byte val = get(row, column);
    num_pieces[val]--;

    // cout << "Set (" << (int)row << "," << (int)column << ") -> " << (int)content << "\n";
    set(row, column, content);
    num_pieces[content]++;

    return result;
}

bool ChessBoard::is_clear_path(byte r1, byte c1, byte r2, byte c2) const
{
    byte dr = r2-r1;
    byte dc = c2-c1;
    byte d = abs(dr != 0 ? dr:dc);
    byte vr = (byte)(dr / d);
    byte vc = (byte)(dc / d);

    r1 += vr;
    c1 += vc;
    while (r1 != r2 || c1 != c2)
    {
        if (table[r1][c1]!=Empty) { return false; }

        r1 += vr;
        c1 += vc;    
    }

    return true;
}

bool ChessBoard::is_check_for(byte color) const
{
    return false;
}

bool ChessBoard::is_valid_move(byte r1, byte c1, byte r2, byte c2) const
{
    if (r1 == r2 && c1 == c2)
    {
        return false;
    }

    const byte field1 = get(r1, c1);
    const byte field2 = get(r2, c2);

    if (field2 != Empty)
    {
        if ((field1 & 0x1) == (field2 & 0x1))
        {
            return false;
        }
    }

    switch (field1)
    {
    case wPawn:
    {
        if (r2==r1+1 && c2==c1 && field2==Empty) {}
        else if (r2==r1+1 && abs(c2-c1)==1 && field2 != Empty) {}
        else if (r1==1 && (r2==r1+2) && c1==c2 && field2==Empty && table[r1+1][c1]==Empty) {}
        else { return false; }
        break;
    }
    case bPawn:
    {
        if (r2==r1-1 && c2==c1 && field2==Empty) {}
        else if (r2==r1-1 && abs(c2-c1)==1 && field2 != Empty) {}
        else if (r1==6 && (r2==r1-2) && c1==c2 && field2==Empty && table[r1-1][c1]==Empty) {}
        else { return false; }
        break;
    }
    case bKnight:
    case wKnight:
    {
        const byte dr = abs(r2-r1);
        const byte dc = abs(c2-c1);

        if ((dr==2 && dc==1) || (dr==1 && dc==2)) {} 
        else { return false; }

        break;
    }
    case bKing:
    case wKing:
    {
        if (abs(r1-r2) > 1 || abs(c1-c2) > 1) { return false; }
        break;
    }
    case bBishop:
    case wBishop:
    {
        byte dr = abs(r2-r1);
        byte dc = abs(c2-c1);

        if (dr == dc)
        {
            if (!is_clear_path(r1,c1,r2,c2)) {return false; }
        } 
        else { return false; }

        break;
    }
    case bRook:
    case wRook:
    {
        byte dr = abs(r2-r1);
        byte dc = abs(c2-c1);

        if (dr==0 || dc==0) 
        {
            if (!is_clear_path(r1,c1,r2,c2)) {return false; }
        } 
        else { return false; }

        break;
    }
    case bQueen:
    case wQueen:
    {
        byte dr = abs(r2-r1);
        byte dc = abs(c2-c1);

        if (dr == dc || dr==0 || dc==0) 
        {
            if (!is_clear_path(r1,c1,r2,c2)) {return false; }
        } 
        else { return false; }

        break;
    }
    default:
        return false;
        break;
    }

    ChessBoard board = *this;
    board.move(r1,c1,r2,c2);

    if (board.is_check_for(field1 & 0x1))
    {
        return false;
    }


    return true;
}

ChessBoard ChessBoard::create_initialized_board()
{
    ChessBoard board;

    board.put('A', 1, FieldContent::wRook);
    board.put('A', 2, FieldContent::wKnight);
    board.put('A', 3, FieldContent::wBishop);
    board.put('A', 4, FieldContent::wQueen);    
    board.put('A', 5, FieldContent::wKing);
    board.put('A', 6, FieldContent::wBishop);
    board.put('A', 7, FieldContent::wKnight);
    board.put('A', 8, FieldContent::wRook);
    

    board.put('B', 1 , FieldContent::wPawn);
    board.put('B', 2 , FieldContent::wPawn);
    board.put('B', 3 , FieldContent::wPawn);
    board.put('B', 4 , FieldContent::wPawn);
    board.put('B', 5 , FieldContent::wPawn);
    board.put('B', 6 , FieldContent::wPawn);
    board.put('B', 7 , FieldContent::wPawn);
    board.put('B', 8 , FieldContent::wPawn);
    
    board.put('G', 1, FieldContent::bPawn);
    board.put('G', 2, FieldContent::bPawn);
    board.put('G', 3, FieldContent::bPawn);
    board.put('G', 4, FieldContent::bPawn);
    board.put('G', 5, FieldContent::bPawn);
    board.put('G', 6, FieldContent::bPawn);
    board.put('G', 7, FieldContent::bPawn);
    board.put('G', 8, FieldContent::bPawn);
    board.put('H', 1, FieldContent::bRook);
    
    board.put('H', 2, FieldContent::bKnight);
    board.put('H', 3, FieldContent::bBishop);
    board.put('H', 4, FieldContent::bQueen);    
    board.put('H', 5, FieldContent::bKing);
    board.put('H', 6, FieldContent::bBishop);
    board.put('H', 7, FieldContent::bKnight);
    board.put('H', 8, FieldContent::bRook);

    return board;
}

ChessBoard ChessBoard::create_test_board()
{
    ChessBoard board;

    board.put('B', 5, FieldContent::wKing);
    board.put('C', 4 , FieldContent::wPawn);
    board.put('C', 5 , FieldContent::wPawn);
    
    
    board.put('E', 5, FieldContent::bKing);
    board.put('E', 4, FieldContent::bKnight);
    

    return board;
}

void ChessBoard::move_generator(
    ChessBoard board, 
    byte move_color, 
    vector<Move>& moves, 
    vector<vector<Move>>& closed_routs, 
    vector<vector<Move>>& open_routs, 
    byte depth)
{
    if (board.is_game_over())
    {
        vector<Move> route = moves;

        cout << closed_routs.size() << ". ";
        print_route(route);
        cout << " - Game over! \n";

        closed_routs.push_back(route);
    }
    else
    {
        if (depth < 5)
        {
            long num_moves = 0;

            for (int r1=0; r1<8; r1++)
            {
                for (int c1=0; c1<8; c1++)
                {
                    byte f = board.get(r1,c1);

                    if (f != Empty)
                    {
                        if ((move_color & 0x1) == (board.get(r1,c1) & 0x1))
                        {
                            for (int r2=0; r2<8; r2++)
                            {
                                for (int c2=0; c2<8; c2++)
                                {
                                    if (board.is_valid_move(r1,c1,r2,c2))
                                    {
                                        moves.push_back(Move(r1,c1,r2,c2));

                                        ChessBoard new_board = board;
                                        new_board.move(r1,c1,r2,c2);

                                        move_generator(new_board, ~move_color, moves, closed_routs, open_routs, depth + 1);

                                        moves.pop_back();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            vector<Move> route = moves;
            open_routs.push_back(route);
        }
    }
}

void ChessBoard::print_move(Move move)
{
    cout << (char)('A'+move.r1) << (int)move.c1 << "->" << (char)('A'+move.r2) << (int)move.c2;
}

void ChessBoard::print_route(vector<Move> route)
{
    for (const auto &move : route) {
        print_move(move);
    }
}

void ChessBoard::save_to_file(string filename, vector<vector<Move>>& routs)
{
    stringstream ss;

    for (const auto &rout : routs) {
        for (const auto &move : rout) {
            ss << "  " << (char)('A'+move.r1) << (int)move.c1 << "->" << (char)('A'+move.r2) << (int)move.c2;
        }
        ss << "\n";
    }

    std::ofstream outFile(filename);

    if (outFile.is_open()) {
        outFile << ss.str();
        outFile.close();
    } else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
}

void ChessBoard::playback(vector<Move> rout, ChessBoard board)
{
    cout << "\n-------------GAME----------------\n\n";
    board.show();

    for (const auto &move : rout) {
        ChessBoard::print_move(move);
        cout << endl;
        board.move(move.r1, move.c1, move.r2, move.c2);
        board.show();
    }
}


