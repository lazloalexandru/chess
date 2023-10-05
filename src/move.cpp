#include "move.h"
#include <algorithm>
#include <iostream>

using namespace std;

bool str_to_Move(string s, Move& out) 
{   
    if (s.length()!=4) { return false; }

    transform(s.begin(), s.end(), s.begin(), ::toupper);
    
    if (s[0] < 'A'  || 'H' < s[0]) return false;
    if (s[2] < 'A'  || 'H' < s[2]) return false;
    if (s[1] < '1'  || '8' < s[1]) return false;
    if (s[3] < '1'  || '8' < s[3]) return false;

    out = Move(s[0]-'A',s[1]-'1',s[2]-'A',s[3]-'1',0,0,0);

    return true;
}