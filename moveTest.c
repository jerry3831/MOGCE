#include <stdio.h>
#include "Engine.c"

int main() 
{
    Position pos;

    init_knight_attacks();
    init_postion(&pos);

    debug_knight_pos(&pos);

    return 0;
}