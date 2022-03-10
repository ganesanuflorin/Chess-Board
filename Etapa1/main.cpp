#include <bits/stdc++.h>
#include "utils.h"

// XBoard Chess Game - First part
// Members:
//  - Iordache Matei Cezar - 325CA - matei.iordache
//  - Siminica Andrei Razvan - 321CA - andrei.siminica
//  - Petrescu Robert-Mihai - 323CA - robert.petrescu
//  - Ganesanu Gigi-Florin - 324CA - gigi.ganesanu


// converts the character to
// its corresponding ASCII value

int ctoi(const char c) {
	return (int)c - (int)'0';
}

// adding all the pieces other than the pawns

void add_rest_pieces(unsigned char board[9][9]) {
    for(int i = 1; i < 5; i++) {
        if(i == 1) {
            board[1][i] =  WHITE_ROCK;
            board[1][8 - i + 1] = WHITE_ROCK;
            board[8][i] = BLACK_ROCK;
            board[8][8 - i + 1] =BLACK_ROCK;
        }
        if(i == 2) {
            board[1][i] = WHITE_HORSE;
            board[1][8 - i + 1] = WHITE_HORSE;
            board[8][i] = BLACK_HORSE;
            board[8][8 - i + 1] = BLACK_HORSE;
        }
        if(i == 3) {
            board[1][i] = WHITE_BISHOP;
            board[1][8 - i + 1] = WHITE_BISHOP;
            board[8][i] = BLACK_HORSE;
            board[8][8 - i + 1] = BLACK_HORSE;
        }
        if(i == 4) {
            board[1][i] = WHITE_QUEEN;
            board[1][8 - i + 1] = WHITE_KING;
            board[8][i] = BLACK_QUEEN;
            board[8][8 - i + 1] = BLACK_KING;
        }
    }
}

// initializing the board
// adding all the pieces

void initialize_board() {
    memset(board, (char) NULL, sizeof(board));
    int contor = 0;
    while(contor < 8) {
        board[2][contor + 1] = WHITE_PAWN;
        board[7][contor + 1] = BLACK_PAWN;
        contor++;
    }
    add_rest_pieces(board);
    
}

Piece *bp = new Piece(7, 1, BLACK_PAWN);
Piece *wp = new Piece(2, 6, WHITE_PAWN);
Piece *p;

// Moving the pawn
// As the black pawn, it will start with
// the one positioned on board[7][1] (a7).
// It will move forward. If the square
// in front of it is blocked, the script will
// check if the pawn can attack. It will check
// first if it can attack by going right, then
// by going left.
// If both of them are available, the pawn will
// to the right.
// As the white pawn, it will start with
// the one positioned on board[2][6] (f2).
// The movement will be the same as to the
// black one, except the fact that in case it
// is needed for the pawn to attack, it will
// start with the left side.
// In both cases, the bot will resign in the
// following cases:
//      - the pawn has reached the other side
//      of the table
//      - the pawn has been eliminated from
//      the table
//      - the pawn has nowhere to move
//      (it is blocked by another piece
//      and is unable to attack)

void pawn_move(int color) {
    int axis;
    char PAWN;
    char pawn_current_column;
    if(color == BLACK) {
        p = bp;
        if(picked_black_pawn == false) {
            picked_black_pawn = true;
        }
        pawn_current_column = black_pawn_current_column;
        axis = -1;
        PAWN = BLACK_PAWN;
    } else if(color == WHITE) {
        p = wp;
        if(picked_white_pawn == false) {
            picked_white_pawn = true;
        }
        pawn_current_column = white_pawn_current_column;
        axis = 1;
        PAWN = WHITE_PAWN;
    }
    if (board[p->row + axis][p->column] == (char) NULL) {
            board[p->row][p->column] = (char) NULL;
            std::string temp_move = "move ";
            temp_move.push_back(pawn_current_column);
            temp_move.append(std::to_string(p->row));
            p->row += axis;
            temp_move.push_back(pawn_current_column);
            temp_move.append(std::to_string(p->row));
            temp_move.append("\n");
            board[p->row][p->column] = PAWN;
            std::cout << temp_move;
            if (p->row == 1 || p->row == 8) {
                std::cout << "resign\n";
            }
        } else if (board[p->row + axis][p->column] != (char) NULL) {
            if (board[p->row + axis][p->column + 1] != (char) NULL) {
                board[p->row][p->column] = (char) NULL;
                std::string temp_move = "move ";
                temp_move.push_back(pawn_current_column);
                pawn_current_column ++;
                temp_move.append(std::to_string(p->row));
                p->row += axis;
                temp_move.push_back(pawn_current_column);
                temp_move.append(std::to_string(p->row));
                temp_move.append("\n");
                p->column ++;
                board[p->row][p->column] = PAWN;
                std::cout << temp_move;
                if (p->row == 1 || p->row == 8) {
                    std::cout << "resign\n";
                }
            } else if (board[p->row + axis ][p->column - 1] != (char) NULL) {
                board[p->row][p->column] = (char) NULL;
                std::string temp_move = "move ";
                temp_move.push_back(pawn_current_column);
                pawn_current_column--;
                temp_move.append(std::to_string(p->row));
                p->row += axis;
                temp_move.push_back(pawn_current_column);
                temp_move.append(std::to_string(p->row));
                temp_move.append("\n");
                p->column--;
                board[p->row][p->column] = PAWN;
                std::cout << temp_move;
                
                if (p->row == 1 || p->row == 8){
                    std::cout << "resign\n";
                }
            } else {
                std::cout << "resign\n";
            }
        } else if (board[p->row + axis ][p->column + axis] != (char) NULL) {
            board[p->row][p->column] = (char) NULL;
            std::string temp_move = "move ";
            temp_move.push_back(pawn_current_column);
            pawn_current_column += axis; // ------ AA
            temp_move.append(std::to_string(p->row));
            p->row += axis;
            temp_move.push_back(pawn_current_column);
            temp_move.append(std::to_string(p->row));
            temp_move.append("\n");
            p->column += axis;      // -------AA
            board[p->row][p->column] = PAWN;
            std::cout << temp_move;
            
            if (p->row == 1 || p->row == 8){
                std::cout << "resign\n";
            }
        } else if (board[p->row +axis][p->column - axis] != (char) NULL) {
            board[p->row][p->column] = (char) NULL;
            std::string temp_move = "move ";
            temp_move.push_back(pawn_current_column);
            pawn_current_column -= axis;
            temp_move.append(std::to_string(p->row));
            p->row += axis;
            temp_move.push_back(pawn_current_column);
            temp_move.append(std::to_string(p->row));
            temp_move.append("\n");
            p->column -= axis;
            board[p->row][p->column] = PAWN;
            std::cout << temp_move;
            if (p->row == 1 || p->row == 8) {
                std::cout << "resign\n";
            }
        }
        if(color == BLACK) {
            black_pawn_current_column = pawn_current_column;
        } else {
            white_pawn_current_column = pawn_current_column;
        }
}

// This function is being called at the beginning of the game

void new_game() {
    engine_side = BLACK;
    play_side = WHITE;
    bp = new Piece(7, 1, BLACK_PAWN);
    wp = new Piece(2, 6, WHITE_PAWN);
    picked_black_pawn = false;
    picked_white_pawn = false;
    black_pawn_current_column = 'a';
    white_pawn_current_column = 'f';
    initialize_board();
}

// Stops the bot from playing.
// Makes a PvP game.

void force(char command[30], int current_side) {
    if (is_valid_command(command)) {

        if (is_legal_move(command)) {
            if (ctoi(board[ctoi(command[1])][ctoi(command[0]) - 96]) - 90 < 0) {
                current_side = WHITE;
            } else {
                current_side = BLACK;
            }

            if (current_side != play_side) {
                board[ctoi(command[3])][ctoi(command[2]) - 96] = board[ctoi(command[1])][ctoi(command[0]) - 96];
                board[ctoi(command[1])][ctoi(command[0]) - 96] = (char) NULL;
                play_side = !play_side;
            } else {
                std::cout << "Not your turn\n";
            }
        } else {
            std::cout << "Invalid move\n";
        }
            
        std::cin >> command;
        force(command, current_side);
    }
}

// verify if the pawn can be moved

void move_legal(char command[30]) {
    if (board[ctoi(command[3])][command[2] - 96] == (char) NULL) {
        board[ctoi(command[3])][command[2] - 96] = board[ctoi(command[1])][command[0] - 96];
        board[ctoi(command[1])][command[0] - 96] = (char) NULL;
        } 
            
        if (engine_side == BLACK) {
            if (board[ctoi(command[3])][command[2] - 96] == board[bp->row][bp->column]) {
                board[ctoi(command[3])][command[2] - 96] = board[ctoi(command[1])][command[0] - 96];
                board[bp->row][bp->column] = (char) NULL;
                std::cout << "resign\n";
            }
        }

        if (engine_side == WHITE) {
            if (board[ctoi(command[3])][command[2] - 96] == board[wp->row][wp->column]) {
                board[ctoi(command[3])][command[2] - 96] = board[ctoi(command[1])][command[0] - 96];
                board[wp->row][wp->column] = (char) NULL;
                std::cout << "resign\n";
            }
        }

        pawn_move(BLACK);
        pawn_move(WHITE);
}

int main() {
    std::signal(SIGINT, SIG_IGN);
    std::setbuf(stdin, NULL);
    std::setbuf(stdout, NULL);
    std::cout.setf(std::ios::unitbuf);
    std::cin.setf(std::ios::unitbuf);
    char command[30];
    initialize_board();

    std::cout << "feature myname=\"CiucasLa2Jumate\" san=0 usermove=0\n";

    while (1) {
        std::cin >> command;
        if (strcmp(command, "xboard") == 0) {
            continue;
        }

        if (strcmp(command, "new") == 0) {
            new_game();
        }

        if (strcmp(command, "quit") == 0) {
            delete wp;
            delete bp;
            break;
        }

        if (strcmp(command, "resign") == 0) {
            std::cout << "resign\n";
        }
        
        if (strcmp(command, "white") == 0) {
            engine_side = WHITE;
            play_side = BLACK;
            continue;
        }

        if (strcmp(command, "black") == 0) {
           engine_side = BLACK;
           play_side = WHITE;
           continue;
        }

        if (strcmp(command, "force") == 0) {
            std::cin >> command;
            int current_side;
            play_side = engine_side;
            force(command, current_side);

            if (strcmp(command, "new") == 0) {
                new_game();
            }

            if (strcmp(command, "white") == 0) {
                engine_side = WHITE;
                play_side = BLACK;
                continue;
            }

            if (strcmp(command, "black") == 0) {
                engine_side = BLACK;
                play_side = WHITE;
                continue;
            }
        }

      
        if (strcmp(command, "go") == 0) {
            if (engine_side == WHITE)
                pawn_move(WHITE);
            if (engine_side == BLACK){
                pawn_move(BLACK);
            }
        }

        if (is_legal_move(command)) {
            move_legal(command);
        }
    }
    
    return 0;
}