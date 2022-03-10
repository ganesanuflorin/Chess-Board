#include "ChessBoard.h"
#include <stdio.h>


#ifndef _FDSF_H
#define	_FDSF_H

//Diverse constante folosite pentru obtinerea mutarilor
static char BVELD[] = {  1,11,21,31,41,51,61,71,    // MVS format to
                 2,12,22,32,42,52,62,72,    // REBEL format
                         3,13,23,33,43,53,63,73,
                         4,14,24,34,44,54,64,74,
                         5,15,25,35,45,55,65,75,
                         6,16,26,36,46,56,66,76,
                         7,17,27,37,47,57,67,77,
                         8,18,28,38,48,58,68,78 };



 static char TA[] = {                                // ASCII output REBEL moves
                'X',
                'A','A','A','A','A','A','A','A','X','X',
                'B','B','B','B','B','B','B','B','X','X',
                'C','C','C','C','C','C','C','C','X','X',
                'D','D','D','D','D','D','D','D','X','X',
                'E','E','E','E','E','E','E','E','X','X',
                'F','F','F','F','F','F','F','F','X','X',
                'G','G','G','G','G','G','G','G','X','X',
                'H','H','H','H','H','H','H','H' };

static char TN[] = {                                // ASCII output REBEL moves
                'X',
                '1','2','3','4','5','6','7','8','X','X',
                '1','2','3','4','5','6','7','8','X','X',
                '1','2','3','4','5','6','7','8','X','X',
                '1','2','3','4','5','6','7','8','X','X',
                '1','2','3','4','5','6','7','8','X','X',
                '1','2','3','4','5','6','7','8','X','X',
                '1','2','3','4','5','6','7','8','X','X',
                '1','2','3','4','5','6','7','8' };

static char BOEKRND[] = { 8,4,2,1,1,1,1,1,1,1,1,1,1 };

static char BORDPOS[] =
                    {    8,18,28,38,48,58,68,78,
                         7,17,27,37,47,57,67,77,
                 6,16,26,36,46,56,66,76,
                         5,15,25,35,45,55,65,75,
                         4,14,24,34,44,54,64,74,
                         3,13,23,33,43,53,63,73,
                 2,12,22,32,42,52,62,72,
                         1,11,21,31,41,51,61,71,0 };

class Book {
public:
    //Obtine mutarea urmatoare, pornind de la tabla primita ca parametru;
    char* getNextMove (ChessBoard tabla, COLOR culoare);
    //Realizeaza initializarile necesare
    void INITIALIZE(void);
private:
    char BOOKFILE[300];
    char TOERFILE[300];
    char EPD[200];
    char FROM[10],TO[10];
    char ERROR;
    long RANDOM1[1120];
    long RANDOM2[1120];
    char _BORD[256];
    long *p_BORD;               // 32 bit redefinition BORD
    long HK1,HK2;
    int  XD,XE,AZ,RND;
    char FROM1[20],FROM2[20],TO1[20],TO2[20];
    //unsigned char _GB[100000];                   // TOERBOEK.BIN
    int  BOEKSTAT[20];

    FILE* FPB;


    void FIND_OPENING(void);
    void MAKE_HASHKEY(void);
    void BOEKIN(void);
    void BOEK_BEGIN(void);
};


#endif	/* _FDSF_H */