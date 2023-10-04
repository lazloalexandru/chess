#include <iostream>
#include <fstream>
#include <sstream>
#include "chessboard.h"
#include "move.h"

using namespace std;


ChessBoard::ChessBoard() {
    memset(table, 0x0, sizeof(table));
}


bool ChessBoard::is_game_over() const
{
    return false;
}
    

void ChessBoard::move(byte r1, byte c1, byte r2, byte c2)
{
    byte f1 = get(r1,c1);
    byte f2 = get(r2,c2);

    set(r1,c1, FieldContent::Empty);
    set(r2,c2, f1);
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
            byte field = get(i,j);
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

inline byte ChessBoard::get(byte row, byte column) const
{
    return (table[row] >> (4 * column)) & 0xF;
}

inline void ChessBoard::set(byte row, byte column, byte content) 
{
    unsigned long mask = ~(0xF << (column * 4));
    table[row] = (table[row] & mask) | (content & 0xF) << (column * 4);

    switch (content)
    {
    case wKing:
        {
            w_king_pos = ((row & 0xF) << 4) | (column & 0xF);
            //cout << "WKing (" << (int)row << "," << (int)column << ") -> " << (int)content << "\n";
            break;
        }
    case bKing:
        {
            b_king_pos = ((row & 0xF) << 4) | (column & 0xF);
            //cout << "BKing (" << (int)row << "," << (int)column << ") -> " << (int)content << "\n";            
            break;
        }
    default:
        break;
    }
}

bool ChessBoard::put(byte row, byte column, FieldContent content)
{
    row = row - 'A';
    column = column - 1;
    bool result = false;

    byte val = get(row, column);

    set(row, column, content);

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
        if (get(r1,c1)!=Empty) { return false; }

        r1 += vr;
        c1 += vc;    
    }

    return true;
}

bool ChessBoard::is_check_for(byte color) const
{
    byte rk, ck;

    if (color & 0x1)
    {
        rk = (b_king_pos >> 4) & 0xF;
        ck = b_king_pos & 0xF;
    }
    else
    {
        rk = (w_king_pos >> 4) & 0xF;
        ck = w_king_pos & 0xF;
    }

    color = (~color) & 0x1;
    
    for (byte r=0; r<ChessBoard::SIZE; r++)
    {
        for (byte c=0; c<ChessBoard::SIZE; c++)
        {
            byte field = get(r,c);

            if (field != Empty)
            {
                if ((color & 0x1) == (field & 0x1))
                {
                    if (is_valid_move(r,c,rk,ck))
                    {
                        Move m(r,c,rk,ck,0,0,0);
                        /*
                        cout << "      ";
                        print_move(m);
                        cout << " Check\n";
                        */
                        return true;
                    }
                }
            }
        }
    }

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
        else if (r1==1 && (r2==r1+2) && c1==c2 && field2==Empty && get(r1+1,c1)==Empty) {}
        else { return false; }
        break;
    }
    case bPawn:
    {
        if (r2==r1-1 && c2==c1 && field2==Empty) {}
        else if (r2==r1-1 && abs(c2-c1)==1 && field2 != Empty) {}
        else if (r1==6 && (r2==r1-2) && c1==c2 && field2==Empty && get(r1-1,c1)==Empty) {}
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

ChessBoard ChessBoard::create_test_board3()
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
    board.put('C', 5 , FieldContent::wPawn);
    board.put('B', 6 , FieldContent::wPawn);
    board.put('B', 7 , FieldContent::wPawn);
    board.put('B', 8 , FieldContent::wPawn);
    
    board.put('G', 1, FieldContent::bPawn);
    board.put('G', 2, FieldContent::bPawn);
    board.put('G', 3, FieldContent::bPawn);
    board.put('G', 4, FieldContent::bPawn);
    board.put('G', 5, FieldContent::bPawn);
    board.put('G', 6, FieldContent::bPawn);
    board.put('E', 7, FieldContent::bPawn);
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

ChessBoard ChessBoard::create_test_board1()
{
    ChessBoard board;

    board.put('B', 6, FieldContent::wRook);
    board.put('B', 5, FieldContent::wKing);
    board.put('C', 4 , FieldContent::wPawn);
    board.put('C', 5 , FieldContent::wPawn);
    
    
    board.put('E', 5, FieldContent::bKing);
    board.put('E', 4, FieldContent::bKnight);
    

    return board;
}


ChessBoard ChessBoard::create_test_board2()
{
    ChessBoard board;

    board.put('A', 7, FieldContent::wRook);
    board.put('B', 6, FieldContent::wRook);
    board.put('B', 5, FieldContent::wKing);
    
    board.put('E', 8, FieldContent::bKing);

    return board;
}


void ChessBoard::move_generator(
    ChessBoard initial_board, 
    byte move_color, 
    vector<vector<Move>>& closed_routs, 
    vector<vector<Move>>& open_routs, 
    byte max_depth)
{
    unsigned long MAX_STATES = 10000000;
    
    static unsigned int Mb = 1024*1024;

    cout << endl <<  "Move generator ... " << endl;
    cout << "Allocating " << (long) ((MAX_STATES * sizeof(ChessBoard)) / Mb) << "Mb for board." << endl;
    cout << "Allocating " << (long) ((MAX_STATES * sizeof(Move)) / Mb) << "Mb for moves." << endl;

    vector<ChessBoard> bv;
    bv.reserve(MAX_STATES);

    vector<Move> mv;
    mv.reserve(MAX_STATES);

    cout << endl <<  "Started ... " << endl;
    byte step = 0;
    
    bv.push_back(initial_board);
    mv.push_back(Move(0,0,0,0,step,(~move_color) & 0x1,0));


    unsigned long i = 0;
    while (i < bv.size() && step < max_depth)
    {
        ChessBoard board = bv.at(i);
        Move move = mv.at(i);
        /*
        cout << i << " step(" << (int)step << " - " << (int)move.step << ") - " << " color: " << (int)move_color << " ";
        print_move(move);
        cout << " " << endl;
        */

        if (move.step == step)
        {
            bool move_made = false;

            for (byte r1=0; r1<ChessBoard::SIZE; r1++)
            {
                for (byte c1=0; c1<ChessBoard::SIZE; c1++)
                {
                    byte field = board.get(r1,c1);

                    if (field != Empty)
                    {
                        if ((move_color & 0x1) == (field & 0x1))
                        {
                            for (byte r2=0; r2<ChessBoard::SIZE; r2++)
                            {
                                for (byte c2=0; c2<ChessBoard::SIZE; c2++)
                                {
                                    if (mv.size() < MAX_STATES)
                                    {
                                        if (board.is_valid_move(r1,c1,r2,c2))
                                        {
                                            ChessBoard new_board = board;
                                            new_board.move(r1,c1,r2,c2);

                                            if (!new_board.is_check_for(move_color))
                                            {
                                                Move m = Move(r1,c1,r2,c2,step+1,move_color,i);
                                                mv.push_back(m);
                                                bv.push_back(new_board);
                                                /*
                                                cout << "    " << mv.size()-1 << " step(" << (int)(step + 1) << ") - " << " color: " << (int)move_color;
                                                cout << "    ";
                                                print_move(m);
                                                cout << endl;
                                                */

                                                move_made = true;
                                            }
                                        }
                                    }                                    
                                    else
                                    {
                                        cout << "Warning: Max capacity reached while searching!" << endl;
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }

            }

            if (!move_made)
            {
                vector<Move> moves;
                moves.push_back(move);
                unsigned long prev_move_index = move.origin;

                while (prev_move_index > 0)
                {
                    move = mv[prev_move_index];
                    moves.push_back(move);
                    prev_move_index = move.origin;
                }
                
                closed_routs.push_back(moves);

                /*
                print_route(moves);                
                cout << "Game Over" << endl;
                */
            }

            i++;
        }
        else
        {
            step++;
            move_color = 0x1&(~move_color);
        }
    }

    cout << "Move generator ... exit" << endl;
}


void ChessBoard::print_move(Move move)
{
    cout << (char)('A'+move.r1) << (int)(move.c1+1) << "->" << (char)('A'+move.r2) << (int)(move.c2+1);
}

void ChessBoard::print_route(vector<Move> route)
{
    for(auto it = route.rbegin(); it != route.rend(); ++it) {
        auto move = *it;
        print_move(move);
        cout << " ";
    }
}

void ChessBoard::save_to_file(string filename, const vector<vector<Move>>& routs)
{
    stringstream ss;

    for (const auto &route : routs) {
        for(auto it = route.rbegin(); it != route.rend(); ++it) {
            auto move = *it;
            ss << "  " << (char)('A'+move.r1) << (int)(move.c1+1) << "->" << (char)('A'+move.r2) << (int)(move.c2+1);
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

    for(auto it = rout.rbegin(); it != rout.rend(); ++it) {
        auto move = *it;
    
        ChessBoard::print_move(move);
        cout << endl;
        board.move(move.r1, move.c1, move.r2, move.c2);
        board.show();
    }
}


