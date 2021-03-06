#include "Mind.h"
#include "stuff.h"
#include "ChessBoard.h"
#include "Tools.h"
#include "MoveTree.h"
#include "Xboard.h"
#include <stdio.h>
#include <ctime>
#include <cstdlib>  //pentru rand()

extern Game joc;
extern MoveTree tree;
extern FILE* debug_file;
extern Xboard xboard;


Mind::Mind() {
}


PIECE pieseImplementate[]={KNIGHT,PAWN,BISHOP,QUEEN,ROOK};
int nrPieseImplementate=5; // fara rege
PIECE Mind::getDesperateMove(int* movePositions){
    int i;
    int j;
    int t;
    int pozPiesa,finalPozPiesa;


     int pozKing=Tools::bitScan(joc.currentBoard.board[joc.colorEngine][KING]);
    PIECE piesa;
    BITBOARD tablaPiese,aux,aux2,mutari;
    fprintf(debug_file,"E apelata functia DESPERATE !!!!!!!!\n\n\n\n\n");
    // si cu regele, dar el este ultimul;
    for(i=0;i<5;i++)
    {   // se iau toate piesele
        piesa=pieseImplementate[i];
        tablaPiese=joc.currentBoard.board[joc.colorEngine][piesa];


        for(j=0;j<64;j++)
        {// pentru fiecare piese de un anumit tip se iau toate mutarile 
            aux=((U64)1<<j) & tablaPiese;
            if(aux)
            {
        pozPiesa=Tools::bitScan(aux);
        if(joc.currentBoard.canDoMove(pozPiesa,pozKing,joc.colorEngine)==1)
        {
        mutari=joc.currentBoard.getPieceMoves(pozPiesa,piesa,joc.colorEngine);
        for(t=0;t<64;t++){
            // se ia fiecare mutare
            aux2=((U64)1<<t) & mutari;
        Tools::printBitBoard(aux2);
                if(aux2)
                {

                finalPozPiesa=Tools::bitScan(aux2);
                movePositions[0]=pozPiesa;
                movePositions[1]=finalPozPiesa;
                    return piesa;
            
                }

            }
            }
            }
        }
    }
    
    piesa=KING;
    tablaPiese=joc.currentBoard.board[joc.colorEngine][KING];

    pozPiesa=Tools::bitScan(tablaPiese);
   joc.currentBoard.board[joc.colorEngine][KING]=(~((U64)1<<pozPiesa))&joc.currentBoard.board[joc.colorEngine][KING];

            
    mutari=joc.currentBoard.getPieceMoves(pozPiesa,piesa,joc.colorEngine);

    fprintf(debug_file,"Incercam sa miscam regele ca ultima solutie\n");

    for(i=0;i<64;i++)
    { aux=((U64)1<<i) & mutari;
    if(aux)
        {

        finalPozPiesa=Tools::bitScan(aux);
        if(joc.currentBoard.isCheck(joc.colorEngine,finalPozPiesa)==0)
        {       movePositions[0]=pozPiesa;
                movePositions[1]=finalPozPiesa;
                  joc.currentBoard.board[joc.colorEngine][KING]=tablaPiese;
                return piesa;

        }

        }

    }
  joc.currentBoard.board[joc.colorEngine][KING]=tablaPiese;
    
    return NO_PIECE;
}

char * Mind::getSanNegaMax()
{

	char *san=NULL;
	MoveTree tree;

	//Calculam timpul alocat pentru mutare
	time_t timePerMove;
	timePerMove=joc.time;
	if(joc.lateGame==false)
		timePerMove/=(40-joc.movesDone);
	else
		timePerMove/=(70-joc.movesDone);
	
	timePerMove*=10;
	fprintf(debug_file,"Time left %d. Time per move %d.\n",joc.time,(int)timePerMove);
	fprintf(debug_file,"Enemy Time left %d. Time per move %d.\n",joc.otime,(int)timePerMove);

	//Calculam pozitia optima

	fprintf(debug_file,"Returning San\n");

	san=tree.IterativeDeepening(timePerMove-1000);
		fprintf(debug_file,"Returner San\n");

	//Dispose the root
	tree.Dispose(tree.getTree(),0);

		fprintf(debug_file,"Tree Disposed\n");

	return san;
    
}

PIECE Mind::getMove(int *movePositions){
    PIECE piesa;
    int ok=0,da=0;
  
    fprintf(debug_file,"se incerca generare arbore\n");
  


Tools::printBitBoard(joc.currentBoard.getAllBoard());
    
  do{
      piesa=getPiece();

      if(ok>200 &&da==0)
      {piesa=getDesperateMove(movePositions);
      da=1;
      return piesa;
      }
      else
	  if (ok>205)
	      return NO_PIECE;

      ok++;
      


      if(piesa!=NO_PIECE)
          piesa=getPieceMove(piesa,movePositions);


  }while(piesa==NO_PIECE);

    
    //salvam pozitiile obtinute
   

    return piesa;
}

PIECE Mind::getPiece(){
     int ok=0;
     PIECE piesa;

    do{
	piesa=pieseImplementate[rand()%nrPieseImplementate];
	ok++;
	//daca dupa 30 de incercari nu s-a gasit nimic valid, ne retragem
	if(ok==30)
	    return NO_PIECE;

    }while(joc.currentBoard.board[joc.colorEngine][piesa]==0);
     return piesa;
    
}
PIECE Mind::getPieceMove(PIECE piesa,int *movePositions){
    int i,ok;
    int pozitiiPiesa[8];
    int nrPiesa=0;
    int indexPiesa;
    int mutariPiesa[50];
    int nrMutari=0;

    int pozKing=Tools::bitScan(joc.currentBoard.board[joc.colorEngine][KING]);
    //obtinem in pozitiiPiesa pozitiile pieselor de tipul ales
    for(i=0;i<64;i++)
	if((((U64)1<<i) & joc.currentBoard.board[joc.colorEngine][piesa])&&(joc.currentBoard.canDoMove(i,pozKing,joc.colorEngine)==1))
	    pozitiiPiesa[nrPiesa++]=i;
     ok=0;
    do{
	//alegem indexul unei piese de tipul ales
	indexPiesa=rand()%nrPiesa;

	//daca dupa 30 de incercari nu s-a gasit nimic valid, ne retragem
	ok++;
	if(ok==30)
	    return NO_PIECE;
    }while((joc.currentBoard.getPieceMoves(pozitiiPiesa[indexPiesa],piesa,joc.colorEngine))==0&&(joc.currentBoard.canDoMove(i,pozKing,joc.colorEngine)==1));
      //Alegem o anumita mutare
    BITBOARD mutari=joc.currentBoard.getPieceMoves(pozitiiPiesa[indexPiesa],piesa,joc.colorEngine);
    fprintf(debug_file,"Mutari posibile pentru piesa %d:\n",piesa);
    Tools::printBitBoard(mutari);

    for(i=0;i<64;i++)
	if(((U64)1<<i) & mutari)
	    mutariPiesa[nrMutari++]=i;
    //obtinem pozitia finala

    int indexMutare=rand()%nrMutari;

    //salvam pozitiile obtinute
    movePositions[0]=pozitiiPiesa[indexPiesa];
    movePositions[1]=mutariPiesa[indexMutare];

    return piesa;

}

PIECE Mind::getKingsEscape(int *movePositions){

    // se localizeaza regele enginu-lui


    PIECE piesa,piesa_de_mutat,piese[]={PAWN,ROOK,BISHOP,QUEEN,KNIGHT};
    BITBOARD pieceBoard,kingMoves,pieceMove=0,auxiliar;

    int nr;
    int pozitiiPiesa[30];
    int i;
    int move;
    int nr_piese=5;
    int linie,coloana,pozitie;
    COLOR culoare=joc.colorEngine;

    int pozKing=Tools::bitScan(joc.currentBoard.board[culoare][KING]);

    piesa=whatPieceAttacksKing(culoare,pozKing,&pieceBoard);

    if(piesa==NO_PIECE)
        fprintf(debug_file,"Eroare, nu s-a gasit piesa care il atacape rege\n");
    else
        fprintf(debug_file,"S-a gasit tipul de piesa %d \n",piesa);
    Tools::printBitBoard(pieceBoard);
    pozitie=Tools::bitScan(pieceBoard);
    for(i=0;i<nr_piese;i++)
    {

        if(piese[i]!=PAWN)
        pieceMove=joc.currentBoard.getPseudoMoves(Tools::bitScan(pieceBoard),piese[i]) & joc.currentBoard.board[culoare][piese[i]];
        else
        {
            linie=pozitie/8;
            coloana=pozitie%8;
            fprintf(debug_file,"Se face tab pe pioni lin:%d col:%d poz:%d \n",linie,coloana,pozitie);

            if(1-culoare==WHITE)  //pionul se misca in sus
        {
                fprintf(debug_file,"Atac pioni WHITE\n");
            if(coloana<7) //poata ataca in dreapta

                    pieceMove=pieceMove | ((U64)1<<(pozitie+9));
            if(coloana>0) //poata ataca in stanga

                   pieceMove|=((U64)1<<(pozitie+7));

        }
        else                //pionul se misca in jos
        {

            if(coloana<7) //poata ataca in dreapta (stanga lui)

                    pieceMove|=((U64)1<<(pozitie-7));
            if(coloana>0) //poata ataca in stanga

                    pieceMove|=((U64)1<<(pozitie-9));


        }

            pieceMove=pieceMove & joc.currentBoard.board[piese[i]][joc.colorEngine];
        }

        fprintf(debug_file,"Pozitia piesei care trebuie luata %d\n",Tools::bitScan(pieceBoard));
        Tools::printBitBoard(pieceMove);
        Tools::printBitBoard(joc.currentBoard.board[culoare][piese[i]]);
        move=Tools::bitScan(pieceMove);
        fprintf(debug_file,"Poate sa ia piesa ce ataca regele fara sa expuna regele %d\n\n",joc.currentBoard.canDoMove(move,pozKing,joc.colorEngine));

        if(pieceMove!=0 )
        {


         fprintf(debug_file,"Se ia piesa ce da sah cu alta piesa\n");
         movePositions[0]=Tools::bitScan(pieceMove);
         movePositions[1]=Tools::bitScan(pieceBoard);
            return piese[i];

         }


     }


    auxiliar=joc.currentBoard.board[joc.colorEngine][KING];
    joc.currentBoard.board[joc.colorEngine][KING]=(~((U64)1<<pozKing)) & joc.currentBoard.board[joc.colorEngine][KING];;

    fprintf(debug_file,"Incercam sa scapam regele\n");
    kingMoves=joc.currentBoard.getPseudoMoves(pozKing,KING);
    nr=0;
    //incerca sa ia piese de pe tabla

    for(i=0;i<64;i++)
	if((((U64)1<<i) & joc.currentBoard.getAllBoardOf(1-culoare) & kingMoves)&&(!joc.currentBoard.isCheck(culoare,i)))
	    pozitiiPiesa[nr++]=i;
    //incerca sa fuga chiar si luand piese
    for(i=0;i<64;i++)
	if((((U64)1<<i) & kingMoves & ~joc.currentBoard.getAllBoardOf(culoare))&&(!joc.currentBoard.isCheck(culoare,i)))
	    pozitiiPiesa[nr++]=i;


            if(nr>0)
            {
    movePositions[0]=pozKing;
    movePositions[1]=pozitiiPiesa[0];

    piesa_de_mutat=KING;
            }
            else{
                joc.currentBoard.board[joc.colorEngine][KING]= auxiliar;
                return NO_PIECE;
            }
     joc.currentBoard.board[joc.colorEngine][KING]= auxiliar;
    return piesa_de_mutat;


}
PIECE Mind::whatPieceAttacksKing(int culoare,int pozKing, BITBOARD *pieceBoard){

    // s-a copiat functia isCheck

    BITBOARD bitPoz;

    unsigned int poz;
    /*la cal nu e nevoie sa se verifice daca e aparat de o piesa pentru ca niciodata nu va fi
     * langa rege astfel in cat sa se puna problema ca acesta sa il captureze sau nu*/

//KNIGHT
    //se verifica daca sahul e dat de cal
    *pieceBoard=joc.currentBoard.board[1-culoare][KNIGHT];    //tabla cu caii de culoare opusa

    //se afla tabla cu pozitiile din care ar putea un cal sa atace regele aflat la pozKing
    bitPoz=joc.currentBoard.getPseudoMoves(pozKing,KNIGHT);

    *pieceBoard=*pieceBoard & bitPoz;
    if(*pieceBoard!=0)    //daca exista cel putin o piesa opusa pe una din pozitiile respective =>sah
        return KNIGHT;

//BISHOP
    //se verifica daca sahul e dat de nebun
    *pieceBoard=joc.currentBoard.board[1-culoare][BISHOP];    //tabla cu nebuni de culoare opusa

    //se afla tabla cu pozitiile din care ar putea un nebun sa atace regele aflat la pozKing
    bitPoz=joc.currentBoard.getPseudoMoves(pozKing,BISHOP);
    *pieceBoard=*pieceBoard & bitPoz;
    if(*pieceBoard!=0)
        return BISHOP;


//ROOK
    //se verifica daca sahul e dat de tura
    *pieceBoard=joc.currentBoard.board[1-culoare][ROOK];    //tabla cu ture de culoare opusa

    //se afla tabla cu pozitiile din care ar putea o tura sa atace regele aflat la pozKing
    bitPoz=joc.currentBoard.getPseudoMoves(pozKing,ROOK);
    *pieceBoard=*pieceBoard & bitPoz;

    if(*pieceBoard!=0)
        return ROOK;

//QUEEN
    //se verifica daca sahul e dat de regina
    *pieceBoard=joc.currentBoard.board[1-culoare][QUEEN];    //tabla cu retine de culoare opusa

    //se afla tabla cu pozitiile din care ar putea o regina sa atace regele aflat la pozKing
    bitPoz=joc.currentBoard.getPseudoMoves(pozKing,QUEEN);
    *pieceBoard=*pieceBoard & bitPoz;
    if(*pieceBoard!=0)
        return QUEEN;


//PAWN
    //se verifica daca sahul e dat de pion


    //se afla tabla cu pozitiile din care un pion ar putea sa atace regele aflat la pozKing
    *pieceBoard=joc.currentBoard.board[1-culoare][PAWN];
    int poz1,poz2;
    poz=pozKing;

    if(culoare==WHITE)  //pionul care ataca e negru deci merge in jos pe tabla => trebuie sa fie pe linia de desupra piesei
    {
        poz1=poz+7;
        poz2=poz+9;

        if(poz1<=63)
            if((*pieceBoard & ((U64) 1<<poz1))!=0)   //daca e pion pe poz1 => piesa e atacata
            {

                *pieceBoard=* pieceBoard & ((U64) 1<<poz1);
                return PAWN;
            }

        if(poz2<=63)
            if((*pieceBoard & ((U64) 1<<poz2))!=0)   //daca e pion pe poz2 => piesa e aparata
            {
                *pieceBoard=* pieceBoard & ((U64) 1<<poz2);
                return PAWN;

            }
    }
    else    //pionul care ataca e alb deci merge in sus pe tabla => trebuie sa fie pe linia de sub piesei
    {
        poz1=poz-7;
        poz2=poz-9;
        fprintf(debug_file,"Se verifica atacul in pozitiile %d %d \n",poz1,poz2);
        Tools::printBitBoard(*pieceBoard);
        if(poz1>=0)
            if((*pieceBoard & ((U64) 1<<poz1))!=0)   //daca e pion pe poz1 => piesa e atacata
            {fprintf(debug_file,"e atacat de PION\n");
               *pieceBoard=*pieceBoard & ((U64) 1<<poz1);
                return PAWN;
            }

        if(poz2>=0)
            if((*pieceBoard & ((U64) 1<<poz2))!=0)   //daca e pion pe poz2 => piesa e aparata
            {
               *pieceBoard=*pieceBoard & ((U64) 1<<poz2);
                return PAWN;
            }
    }



    return NO_PIECE;


}

Mind::~Mind() {
}
