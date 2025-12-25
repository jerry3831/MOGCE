#include <stdio.h>

typedef struct {
    long pawns;
    long rooks;
    long bishops;
    long knights;
    long queens;
    long king;
} Pieces;

static void print_pieces(const char *name, Pieces p)
{
    printf("%s: pawns=%ld rooks=%ld bishops=%ld knights=%ld queens=%ld king=%ld\n",
           name, p.pawns, p.rooks, p.bishops, p.knights, p.queens, p.king);
}

int main(void)
{
    /* Use a struct to group related piece counts/bitboards */
    Pieces white = {0};
    Pieces black = {0};

    /* initialize white pieces (example values) */
    white.pawns   = 8;
    white.rooks   = 2;
    white.bishops = 2;
    white.knights = 2;
    white.queens  = 1;
    white.king    = 1;

    print_pieces("White", white);
    print_pieces("Black", black);

    return 0;
}