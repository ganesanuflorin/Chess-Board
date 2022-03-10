#ifndef __UTILS__H__
#define __UTILS__H__

#define WHITE_PAWN 'P'
#define BLACK_PAWN 'p'
#define WHITE_HORSE 'H'
#define BLACK_HORSE 'h'
#define WHITE_BISHOP 'B'
#define BLACK_BISHOP 'b'
#define WHITE_QUEEN 'Q'
#define BLACK_QUEEN 'q'
#define WHITE_KING 'K'
#define BLACK_KING 'k'
#define WHITE_ROCK 'R'
#define BLACK_ROCK 'r'

enum SIDE {WHITE = 1, BLACK = 0};

unsigned char board[9][9];
int engine_side = BLACK;
int play_side = WHITE;
char black_pawn_current_column = 'g';
char white_pawn_current_column = 'd';
bool picked_black_pawn = false;
bool picked_white_pawn = false;

class Piece {
public:
    int row;
    int column;
    char type;

    Piece() {}
    virtual ~Piece() {}

    explicit Piece(int row, int column, char type) {
        this->row = row;
        this->column = column;
        this->type = type;
    }
};

// verify if the command given is
// the one needed for the 'force' function

int is_valid_command(char command[30]) {
    if (strcmp(command, "xboard") && strcmp(command, "new") && strcmp(command, "quit") 
            && strcmp(command, "resign") && strcmp(command, "white") && strcmp(command, "black") && 
            strcmp(command, "go")) {
        return 1;
    }
    return 0;
}

bool is_legal_move(char *move) {
    if( move[0] >= 'a' && move[0] <= 'h' && move[2] >= 'a' && move[2] <= 'h' &&
        move[1] - '0' >= 1 && move[1] - '0' <= 8 && move[3] - '0' >= 1 && move[3] - '0' <= 8) {
        return 1;
    }
    return 0;
}

#endif // __UTILS_H__