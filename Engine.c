#include <stdio.h>
#include <stdbool.h>
#include <stdbit.h>>
#define SET_BIT(bb, sq)   ((bb) |= (1ULL << (sq)))
#define CLEAR_BIT(bb, sq) ((bb) &= ~(1ULL << (sq)))
#define GET_BIT(bb, sq)   ((bb) & (1ULL << (sq)))
#define NOT_A_FILE 0xfefefefefefefefeULL
#define NOT_H_FILE 0x7f7f7f7f7f7f7f7fULL
#define NOT_AB_FILE 0xfcfcfcfcfcfcfcfcULL
#define NOT_GH_FILE 0X3f3f3f3f3f3f3f3fULL

typedef struct pieceTypes
{
    // white pieces
    long whitePawns;
    long whiteKnights;
    long whiteBishops;
    long whiteRooks;
    long whiteQueens;
    long whiteKing;

    long blackPawns;
    long blackKnights;
    long blackBishops;
    long blackRooks;
    long blackQueens;
    long blackKing; 
} Pieces;

// Standard chess rules
typedef struct ruleTypes
{
    bool whiteTurn;
    int castlingRights;
    int enPassantSquare;
    bool capture;
    bool promotion;
    bool pawnDoublePush;
} Rules;

// move structure
typedef struct
{
    int from;
    int to;
    int piece;
    int captured;
    int promotion;
} Move;

typedef struct
{
    Pieces pieces;
    Rules rules;
} Position;

// Square to Square representation
enum {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

// hardcode initial position pending iteration improvement.
// pending pseudo-legal and legal moves.
void init_position(Position *pos)
{
    pos->pieces.whitePawns     = 0x000000000000FF00ULL;
    pos->pieces.whiteRooks     = 0x0000000000000081ULL;
    pos->pieces.whiteKnights   = 0x0000000000000042ULL;
    pos->pieces.whiteBishops   = 0x0000000000000024ULL;
    pos->pieces.whiteQueens    = 0x0000000000000008ULL;
    pos->pieces.whiteKing      = 0x0000000000000010ULL;

    pos->pieces.blackPawns     = 0X00FF000000000000ULL;
    pos->pieces.blackRooks     = 0X8100000000000000ULL;
    pos->pieces.blackKnights   = 0X4200000000000000ULL;
    pos->pieces.blackBishops   = 0X2400000000000000ULL;
    pos->pieces.blackQueens    = 0X0800000000000000ULL;
    pos->pieces.blackKing      = 0X1000000000000000ULL;

    pos->rules.whiteTurn = true;
    pos->rules.castlingRights = 0b1111; // WK | WQ | BK | BQ
    pos->rules.enPassantSquare = -1;
    pos->rules.capture = false;
    pos->rules.promotion = false;
    pos->rules.pawnDoublePush = false;
}

// square occupation
unsigned long long white_occupancy(Pieces *p)
{
    return p->whitePawns | p->whiteKnights | p->whiteBishops | p->whiteRooks |
        p->whiteQueens | p->whiteKing;
}

unsigned long long black_occupancy(Pieces *p)
{
    return p->blackPawns | p->blackKnights | p->blackBishops | p->blackRooks |
        p->blackQueens | p->blackKing;
}

unsigned long long all_occupancy(Pieces *p)
{
    return white_occupancy(p) | black_occupancy(p);
}

// for debbuging purposes------------------------------
// print bitboards 0-63
void print_bitboard(unsigned long long bb)
{
    for (int rank = 7; rank >=0; rank--) {
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            printf("%d ", (bb >> sq) & 1);
        }
        printf("\n");
    }
    printf("\n");
}

// pawns moves and captures
unsigned long long white_pawn_single_push(Pieces *p){
    unsigned long long empty = ~all_occupancy(p);
    return (p->whitePawns << 8) & empty;
} 
unsigned long long white_pawn_attacks_left(Pieces *p) {
    return (p->whitePawns << 7) & black_occupancy(p);
}
unsigned long long white_pawn_attacks_right(Pieces *p) {
    return (p->whitePawns << 9) & black_occupancy(p);
}

// knights moves and captures/attacks------------------>
unsigned long long knightsAttacks[64];
void init_knight_attacks()
{
    for (int sq = 0; sq < 64; sq++) {
        unsigned long long bb = 1ULL << sq;
        unsigned long long attacks = 0ULL;

        attacks |= (bb << 17) & NOT_A_FILE;
        attacks |= (bb << 15) & NOT_H_FILE;
        attacks |= (bb << 10) & NOT_AB_FILE;
        attacks |= (bb << 6)  & NOT_GH_FILE;

        attacks |= (bb >> 17) & NOT_H_FILE;
        attacks |= (bb >> 15) & NOT_A_FILE;
        attacks |= (bb >> 10) & NOT_GH_FILE;
        attacks |= (bb >> 6)  & NOT_AB_FILE;

        knightsAttacks[sq] = attacks;
    }
}

// white knights
unsigned long long white_knight_moves(Pieces *p)
{
    unsigned long long moves = 0ULL;
    unsigned long long knights = p->whiteKnights;
    unsigned long long own = white_occupancy(p);

    while (knights) {
        int sq = __builtin_ctzll(knights);
        knights &- knights - 1;
        moves |= knightsAttacks[sq] & ~own;
    }
    return moves;
}

// black knights
unsigned long long black_knight_moves(Pieces *p)
{
    unsigned long long moves = 0ULL;
    unsigned long long knights = p->blackKnights;
    unsigned long long own = black_occupancy(p);

    while (knights) {
        int sq = __builtin_ctzll(knights);
        knights &= knights -1;
        moves |= knightsAttacks[sq] & ~own;
    }
    return moves;
}

// knight attack for one square
unsigned long long knight_attacks_from(int square) 
{
    return knightsAttacks[square];
}

// knight move debug purpose---------------->
void debug_knight_pos(Position *pos)
{
    printf("white knight moves: \n");
    print_bitboard(white_knight_moves(&pos->pieces));
}

int main() 
{
    Position pos;

    init_knight_attacks();
    init_postion(&pos);

    debug_knight_pos(&pos);

    return 0;
}