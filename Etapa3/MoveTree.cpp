#include "MoveTree.h"
#include <stdio.h>
#include "stuff.h"
#include "Tools.h"
#include <stdlib.h>
#include <list>
#include "time.h"
#include "Xboard.h"
extern FILE* debug_file;
extern Game joc;
extern Xboard xboard;
using namespace std;


MoveTree::MoveTree(){
    nrNoduri=0;

}

//aloca memorie pentru un nod si face setarile de rigoare
MNode* MoveTree::creareNod(){

    MNode *node;
	node=(MNode*)malloc(sizeof(MNode));

	node->parinte=NULL;
	node->copii=(MNode**)malloc(MAX_COPII*sizeof(MNode*));
	node->nrCopii=0;
	node->nrMaxAlocat=MAX_COPII;
	node->niv=0;

	node->boardScore=UNDEFINED;  // scorul intotdeauna pozitiv pe tabla

    return node;
}

//adauga copilul copil la parintele parinte
MNode* MoveTree::adaugaCopil(MNode* tata, MNode* copil,COLOR culoare)
{

	//trebuie realocat spatiu suplimentar pentru referinte catre copii
    if(tata->nrCopii==tata->nrMaxAlocat)
    {
        tata->copii=(MNode**)realloc(tata->copii,(tata->nrMaxAlocat+NR_REALLOC)*sizeof(MNode*));

        tata->nrMaxAlocat=tata->nrMaxAlocat+NR_REALLOC;
    }

	//Evaluam copilul
	copil->quickScore=copil->board.evaluateQuick(culoare);

	//Cautam si introducem respectivul copil in vectorul de copii, pe pozitia buna;
	short i;
	for(i=0;i<tata->nrCopii;i++)
		if(tata->copii[i]->quickScore > copil->quickScore)
			break;

	//Mutam elementele la dreapta si introducem copilul
	short poz=i;
	for(i=tata->nrCopii-1;i>=poz;i--)
		tata->copii[i+1]=tata->copii[i];

	//Punem elementul in pozitia finala
	tata->copii[poz]=copil;

	//Actualizam restul datelor
    tata->nrCopii=tata->nrCopii+1;

	copil->niv=tata->niv+1;
    copil->parinte=tata;

	//returneaza copilul
    return copil;
}

/* adauga/creeaza la arborele caracterizat de root un nou nivel, returneaza radacina;
 * nivelul contine doar mutarile de atac si va fi folosit in Quiesence Search.*/
MNode* MoveTree::creareNivelAtac(MNode* nodRoot,COLOR culoare)
{
    int i;
    int pozPiesa,finalPozPiesa;
    int movePositions[2];
    PIECE piesa;
    BITBOARD tablaPiese,mutari;
    ChessBoard backup;

    MNode *fiu_curent;

    int pozKing=Tools::bitScan(nodRoot->board.board[culoare][KING]);


    for(i=PAWN;i<=KING;i++)
    {
		// se iau toate piesele
        piesa=i;

        tablaPiese=nodRoot->board.board[culoare][piesa];

		//Cat timp tabla mai are piese, pentru fiecare piese de un anumit tip se iau toate mutarile
		while(tablaPiese)
        {
			//We find the position of the piece
            pozPiesa=Tools::bitScan(tablaPiese);
			//Stergem 1-ul de pe pozitia gasita
			tablaPiese=tablaPiese & (~((U64)1<<pozPiesa));
			{
				if(culoare==WHITE)
					mutari=nodRoot->board.getPieceMoves(pozPiesa,piesa,culoare) & nodRoot->board.getBlackBoard();
				else
					mutari=nodRoot->board.getPieceMoves(pozPiesa,piesa,culoare) & nodRoot->board.getWhiteBoard();

				// se ia fiecare mutare
				while(mutari)
				{
					//We find the final position of the piece
					finalPozPiesa=Tools::bitScan(mutari);
					//Stergem 1-ul de pe pozitia gasita
					mutari=mutari & (~((U64)1<<finalPozPiesa));

					movePositions[0]=pozPiesa;
					movePositions[1]=finalPozPiesa;

                    //salvam tabla curenta
                    backup=nodRoot->board;

                    //efectuam "virtual" mutarea pe tabla backup pentru a verifica sahul
                    backup.doMove(movePositions[0],movePositions[1],piesa,culoare);

                    if(piesa==KING)
                        pozKing=finalPozPiesa;


                    //Daca e sah, anulam mutarea si trecem mai departe
                    if(backup.isCheck(culoare,pozKing))
                    {
                            //fprintf(debug_file,"INCORECTA mutarea %d : %d -> %d!\n\n",piesa,movePositions[0],movePositions[1]);
                            continue;
                    }

                    //salvam fiul

                    //Cream un nou nod;
					nrNoduri++;

					fiu_curent=creareNod();
					fiu_curent->board=nodRoot->board;
					fiu_curent->niv=nodRoot->niv+1;
					adaugaCopil(nodRoot,fiu_curent,culoare);

					fiu_curent->movePositions[0]=movePositions[0];
					fiu_curent->movePositions[1]=movePositions[1];
					fiu_curent->movedPiece=i;

					fiu_curent->board.doMove(movePositions[0],movePositions[1],i,culoare);
				}
            }
		}
	}
	return  nodRoot;
}

// adauga/creeaza la arborele caracterizat de root un nou nivel, returneaza radacina;
MNode* MoveTree::creareNivelTree(MNode* nodRoot,COLOR culoare)
{
    int i;
    int pozPiesa,finalPozPiesa;
    int movePositions[2];
    PIECE piesa;
    BITBOARD tablaPiese,mutari;
    ChessBoard backup;

    MNode *fiu_curent;

    int pozKing=Tools::bitScan(nodRoot->board.board[culoare][KING]);


    for(i=PAWN;i<=KING;i++)
    {
		// se iau toate piesele
        piesa=i;

        tablaPiese=nodRoot->board.board[culoare][piesa];

		//Cat timp tabla mai are piese, pentru fiecare piese de un anumit tip se iau toate mutarile
		while(tablaPiese)
        {
			//We find the position of the piece
            pozPiesa=Tools::bitScan(tablaPiese);
			//Stergem 1-ul de pe pozitia gasita
			tablaPiese=tablaPiese & (~((U64)1<<pozPiesa));
			
				mutari=nodRoot->board.getPieceMoves(pozPiesa,piesa,culoare);

				// se ia fiecare mutare
				while(mutari)
				{
					//We find the final position of the piece
					finalPozPiesa=Tools::bitScan(mutari);
					//Stergem 1-ul de pe pozitia gasita
					mutari=mutari & (~((U64)1<<finalPozPiesa));

					movePositions[0]=pozPiesa;
					movePositions[1]=finalPozPiesa;

                    //salvam tabla curenta
                    backup=nodRoot->board;

                    //efectuam "virtual" mutarea pe tabla backup pentru a verifica sahul
                    backup.doMove(movePositions[0],movePositions[1],piesa,culoare);

                    if(piesa==KING)
                        pozKing=finalPozPiesa;


                    //Daca e sah, anulam mutarea si trecem mai departe
                    if(backup.isCheck(culoare,pozKing))
                    {
                            continue;
                    }

                    //salvam fiul

                    //Cream un nou nod;
					nrNoduri++;

					fiu_curent=creareNod();
					fiu_curent->board=nodRoot->board;
					fiu_curent->niv=nodRoot->niv+1;
					adaugaCopil(nodRoot,fiu_curent,culoare);

					fiu_curent->movePositions[0]=movePositions[0];
					fiu_curent->movePositions[1]=movePositions[1];
					fiu_curent->movedPiece=piesa;

					fiu_curent->board.doMove(movePositions[0],movePositions[1],piesa,culoare);
				}
            
		}
	}

	//Adaugam alte tipuri de mutari:
	/*ROCADA*/
	/*Daca movedPiece==KING si movePositions[1]=-1 => s-a facut rocada mica.
	*Daca movedPiece==KING si movePositions[1]=-2 => s-a facut rocada mare.*/
	ChessBoard nouaTabla=nodRoot->board;
	if(culoare==WHITE)
	{
		if((nodRoot->board.getFlags() & F_CASTLINGW)!=0 && (nodRoot->board.getFlags() & F_CATLDONEW)==0)	//daca nu a fost facuta si mai poate fi facuta
		{
			nouaTabla=nodRoot->board;
			//si daca nu sunt piese acolo si nu s-a mutat tura stanga
			if((nodRoot->board.getFlags() & F_CASTLR_L_W)==0 && (nodRoot->board.board[WHITE][ROOK] & ((U64)1)) && (nodRoot->board.getAllBoard() & ((U64)1<<1))==0 && (nodRoot->board.getAllBoard() & ((U64)1<<2))==0 && (nodRoot->board.getAllBoard() & ((U64)1<<3))==0)
				//Si nu e sah pe nici una dintre pozitii
				if(nodRoot->board.isCheck(WHITE,2)==0 && nodRoot->board.isCheck(WHITE,3)==0 && nodRoot->board.isCheck(WHITE,4)==0)
				{
					nouaTabla.board[WHITE][KING] =((U64)4);		//setam regele pe poz finala
					nouaTabla.board[WHITE][ROOK] |=((U64)8);	//setam tura pe pozitia finala
					nouaTabla.board[WHITE][ROOK] &=~((U64)1);	//stergem tura de pe pozitia initiala
					nouaTabla.setFlags(((nouaTabla.getFlags() | F_CATLDONEW) & ~F_CASTLINGW) | F_CASTLR_R_W);

					//Adaugare nod
					fiu_curent=creareNod();
					fiu_curent->board=nouaTabla;
					fiu_curent->niv=nodRoot->niv+1;
					adaugaCopil(nodRoot,fiu_curent,culoare);

					fiu_curent->movePositions[0]=ROCADA_MARE;
					fiu_curent->movedPiece=KING;
				}
			
			nouaTabla=nodRoot->board;
			//si daca nu sunt piese acolo si nu s-a mutat tura dreapta
			if((nodRoot->board.getFlags() & F_CASTLR_R_W)==0 && (nodRoot->board.board[WHITE][ROOK] & ((U64)1<<7)) && (nodRoot->board.getAllBoard() & ((U64)1<<5))==0 && (nodRoot->board.getAllBoard() & ((U64)1<<6))==0)
				//Si nu e sah pe nici una dintre pozitii
				if(nodRoot->board.isCheck(WHITE,4)==0 && nodRoot->board.isCheck(WHITE,5)==0 && nodRoot->board.isCheck(WHITE,6)==0)
				{
					nouaTabla.board[WHITE][KING] =((U64)1<<6);
					nouaTabla.board[WHITE][ROOK] |=((U64)1<<5);
					nouaTabla.board[WHITE][ROOK] &=~((U64)1<<7);
					nouaTabla.setFlags(((nouaTabla.getFlags() | F_CATLDONEW) & ~F_CASTLINGW) | F_CASTLR_R_W);
					

					//Adaugare nod
					fiu_curent=creareNod();
					fiu_curent->board=nouaTabla;
					fiu_curent->niv=nodRoot->niv+1;
					adaugaCopil(nodRoot,fiu_curent,culoare);

					fiu_curent->movePositions[0]=ROCADA_MICA;
					fiu_curent->movedPiece=KING;
				}
		}
	}
	else	//Culoare neagra
	{

		if((nodRoot->board.getFlags() & F_CASTLINGB)!=0 && (nodRoot->board.getFlags() & F_CATLDONEB)==0)	//daca nu a fost facuta si mai poate fi facuta
		{
			nouaTabla=nodRoot->board;
			//si daca nu sunt piese acolo si nu s-a mutat tura stanga
			if((nodRoot->board.getFlags() & F_CASTLR_L_B)==0 && (nodRoot->board.board[BLACK][ROOK] & ((U64)1<<56)) && (nodRoot->board.getAllBoard() & ((U64)1<<57))==0 && (nodRoot->board.getAllBoard() & ((U64)1<<58))==0 && (nodRoot->board.getAllBoard() & ((U64)1<<59))==0)
				//Si nu e sah pe nici una dintre pozitii
				if(nodRoot->board.isCheck(BLACK,58)==0 && nodRoot->board.isCheck(BLACK,59)==0 && nodRoot->board.isCheck(BLACK,60)==0)
				{
					nouaTabla.board[BLACK][KING] =((U64)1<<58);
					nouaTabla.board[BLACK][ROOK] |=((U64)1<<59);
					nouaTabla.board[BLACK][ROOK] &=~((U64)1<<56);
					nouaTabla.setFlags(((nouaTabla.getFlags() | F_CATLDONEB) & ~F_CASTLINGB) | F_CASTLR_L_B);

					//Adaugare nod
					fiu_curent=creareNod();
					fiu_curent->board=nouaTabla;
					fiu_curent->niv=nodRoot->niv+1;
					adaugaCopil(nodRoot,fiu_curent,culoare);

					fiu_curent->movePositions[0]=ROCADA_MARE;
					fiu_curent->movedPiece=KING;
				}
			nouaTabla=nodRoot->board;
			//si daca nu sunt piese acolo si nu s-a mutat tura dreapta
			if((nodRoot->board.getFlags() & F_CASTLR_R_B)==0 && (nodRoot->board.board[BLACK][ROOK] & ((U64)1<<63)) && (nodRoot->board.getAllBoard() & ((U64)1<<61))==0 && (nodRoot->board.getAllBoard() & ((U64)1<<62))==0)
				//Si nu e sah pe nici una dintre pozitii
				if(nodRoot->board.isCheck(BLACK,60)==0 && nodRoot->board.isCheck(BLACK,61)==0 && nodRoot->board.isCheck(BLACK,62)==0)
				{
					nouaTabla.board[BLACK][KING] =((U64)1<<62);
					nouaTabla.board[BLACK][ROOK] |=((U64)1<<61);
					nouaTabla.board[BLACK][ROOK] &=~((U64)1<<63);
					nouaTabla.setFlags(((nouaTabla.getFlags() | F_CATLDONEB) & ~F_CASTLINGB) | F_CASTLR_R_B);

					//Adaugare nod
					fiu_curent=creareNod();
					fiu_curent->board=nouaTabla;
					fiu_curent->niv=nodRoot->niv+1;
					adaugaCopil(nodRoot,fiu_curent,culoare);


					fiu_curent->movePositions[0]=ROCADA_MICA;
					fiu_curent->movedPiece=KING;
				}
		}
	}
	return  nodRoot;
}






/*Actualizeaza arborele in functie de tabla curenta 'currentBoard'
 *Se sterg ramurile inutile si se adauga noi nivele in arbore, pana se completeaza 'level' nivele;*/
void MoveTree::updateNewTree(ChessBoard currentBoard, int level)
{
}

/*Obtine radacina arborelui MoveTree, de tip MNode.*/
MNode* MoveTree::getTree()
{
    return root;
}

void MoveTree::eliberareNod(MNode* node)
{
	// se elibereaza nodul doar daca este frunza (nu cumva sa se distruga arborele)
    if(node->nrCopii==0)
    {
        node->board.~ChessBoard();
        free(node->copii);
        free(node);
    }
}

void MoveTree::parcurge(MNode* rad,int level){


    int i;

    nrCopii++;
    for(i=0;i<rad->nrCopii;i++)
    {

    parcurge(rad->copii[i],level+1);

    }




}


void MoveTree::Dispose(MNode *node,int level){



    int i=0;

// node trebuie sa fie parinte
    nrCopii++;

      if(node->nrCopii==0)
      {

			eliberareNod(node);
            return;
      }

      else
      {


      for(i=0;i<node->nrCopii;i++){

           Dispose(node->copii[i],level+1);

       }


      node->nrCopii=0;
        eliberareNod(node);


      }

}



int MoveTree::AlfaBeta(MNode* node, int alfa, int beta, COLOR color, int depth){
    int i;
    int score;
	int poz;

    int bestScore=MIN;


    if(depth==0)
        return node->board.evaluateBoard(color);

    if(node->nrCopii==0)
		creareNivelTree(node,1-color);


    for(i=0;i<node->nrCopii;i++)
    {
        score=-AlfaBeta(node->copii[i],-beta,-alfa,1-color,depth-1);


        if(score>=beta && node->parinte!=NULL )
            return score; // fail soft

        if(score>bestScore)
        {
            bestScore=score;
            if(score>alfa)
			{
				alfa=score;

			}
							poz=i;

        }
    }

	if(node->parinte==NULL)
		return poz;
    return bestScore;


}


int MoveTree::quesenceSearch(MNode* node,int alfa,int beta,COLOR color, int depth){

 int i;
    int score;

    int bestScore=MIN;


    // daca a ajuns la nivelul setat
    if(depth==0)
        return node->board.evaluateBoard(color);



    if(node->nrCopii==0)
        creareNivelAtac(node,color);



        //daca nu mai are copii
    if(node->nrCopii==0)
        return node->board.evaluateBoard(color);




    for(i=0;i<node->nrCopii;i++)
    {
        score=-quesenceSearch(node->copii[i],-beta,-alfa,1-color,depth-1);

        if(score>=beta)
            return score; // fail soft

        if(score>bestScore)
        {
            bestScore=score;
            if(score>alfa)
                alfa=score;

        }
    }
    return bestScore;





}


int MoveTree::NegaScout(MNode* node,int alfa,int beta,COLOR color,int depth, time_t maxDuration){
    int i;
    int poz=0;
    int cur;

	//facem verificarea de timp la nivelul final
    if(depth==0)
    {
		currentTime=clock();

		if(maxDuration-(clock()-startTime)/(CLOCKS_PER_SEC/1000)<=0)
		{

			this->levelIncomplete=true;
			onTime=0;


            return node->board.evaluateBoard(color);
       	}

    }

    if(depth==0)
    {
       
            return quesenceSearch(node,-beta,-alfa,color,QUESENCE_LEVEL);


    }
	// se creeaza copii pentru nodul curent, copii culoare opusa, alt nivel, celalat oponent
	// daca nodul nu are copii deja creati, nod terminal, atunci se creeaza;

    if(node->nrCopii==0)
		creareNivelTree(node,color);

    // daca suntem pe nivelul 0, punctul de plecare (tabla curenta) si nu se poate face nici o mutare
    if(node->parinte==NULL)
        if(node->nrCopii==0){

        int pozKing;
        // se obtine pozitia regelui a enginu-lui

        pozKing=Tools::bitScan(node->board.board[joc.colorEngine][KING]);



        if(node->board.isCheck(joc.colorEngine,pozKing)==1)
            return CHECKMATE;


            return DRAW;
       }

//daca nu se trece mai departe, ei sunt deja creati

    int b=beta;
    for(i=0;i<node->nrCopii;i++)
        {

	    //NegaScout


        cur=-NegaScout(node->copii[i],-b,-alfa,1-color,depth-1,maxDuration);


       if(cur>alfa)
       {


           alfa=cur;
           maxScore=cur;

           poz=i;

       }

		if(alfa>=beta && node->parinte!=NULL)
		{

		nrPrunninguri++;
                return alfa;             // beta cut-off
		}
                if(onTime==0)
                    return i;
    }
            if(node->parinte==NULL)
                    return poz;

	levelIncomplete=false;
    return alfa;
}

int MoveTree::negaMax2( int depth, MNode* rad,COLOR culoare )
{
        if ( depth == 0 )
			return rad->board.evaluateBoard(culoare);
        int max = 1000000;
 
		creareNivelTree(rad,1-culoare);
		rad->board.printBoard();
		int i=0,poz;
        for (i=0;i<rad->nrCopii;i++)
        {
                int score = -negaMax2( depth - 1, rad->copii[i],1-culoare );
                if( score < max )
				{
					max = score;
					poz=i;
				}
        }

		if(depth==3)
			copilToMove=poz;
        return max;
}



char* MoveTree::IterativeDeepening(time_t maxDuration)
{


    MNode* nodeRoot;
	COLOR culoare=joc.colorEngine;	//culoarea inversa pentru ca sa se inceapa cu generarea mutarilor noastre


// presepunem ca nivelele sunt generate complet


    int depthLimit;
    int max;
    int poz;



    // cand incepe procesul
    // cand incepe procesul
    startTime=clock();

    // se creeaza nodul radacina
    nodeRoot=creareNod();
    nodeRoot->board=joc.currentBoard;
    nodeRoot->parinte=NULL;

    root=nodeRoot;

    // depthLimitul initial
    depthLimit=ARB_DEPTH;
	


    levelIncomplete=0;
    onTime=1;


    int pozFinal=0;
    // iterative deepening

    while(1)
	{
		fprintf(debug_file,"Starting Iterative deepening with duration %d for level %d\n",(int)(maxDuration-(clock()-startTime)/(CLOCKS_PER_SEC/1000)),depthLimit);

		poz=0;

		nrPrunninguri=0;


        poz=NegaScout(nodeRoot,MIN,MAX,culoare,depthLimit-1,maxDuration);

		fprintf(debug_file,"S-a ales pozitia %d \n",poz);

		//Am primit sah mat
		if(poz==CHECKMATE)
		{
			xboard.resignEngineToXboard();
			fprintf(debug_file,"Am primit sah mat.\n");
			xboard.getMoveFromXboard();
			return NULL;
		}
		//Este pat (nu putem muta)
		if(poz==DRAW)
		{
			xboard.resignEngineToXboard();
			fprintf(debug_file,"Am primit draw. Nu putem muta.\n");
			xboard.getMoveFromXboard();
			return NULL;
		}

        fprintf(debug_file,"S-a ales pozitia %d \n",poz);

        if(levelIncomplete==0)
        {
            pozFinal=poz;

        }

		 if(levelIncomplete==false)
		 {

			 fprintf(debug_file,"-->S-a generat complet nivelul %d si mai sunt %d milisecunde.\n",depthLimit,(int)(maxDuration-(clock()-startTime)/(CLOCKS_PER_SEC/1000)));

		 }
		else
			 fprintf(debug_file,"Nu s-a generat complet nivelul %d si timpul \"ramas\" este %d milisecunde.\n",depthLimit,(int)(maxDuration-(clock()-startTime)/(CLOCKS_PER_SEC/1000)));



		//daca nu mai este timp, iesim;
		if(maxDuration-(clock()-startTime)/(CLOCKS_PER_SEC/1000)<=0)
		{
			fprintf(debug_file,"S-a depasit timpul alocat. Time left: %d\n",(int)(maxDuration-(clock()-startTime)/(CLOCKS_PER_SEC/1000)));
			break;
		}

		depthLimit++;

		if(depthLimit==MAX_DEPTH)
			break;

    }





    poz=pozFinal;
    max=maxScore;

MNode *chosenNode=nodeRoot->copii[poz];
fprintf(debug_file,"S-a ales poz %d cu aux %d din nr total copii root %d si valoarea %d\n",poz,max,nodeRoot->nrCopii,max);
	fprintf(debug_file,"S-a ales nodul %d varianta %d(%d -> %d) adica %s:\n",poz,chosenNode->parinte->movedPiece,chosenNode->movePositions[0],chosenNode->movePositions[1],chosenNode->board.getSANofMove(joc.colorEngine,chosenNode->movePositions[0],chosenNode->movePositions[1],chosenNode->movedPiece));
	chosenNode->board.printBoard();

      return chosenNode->parinte->board.getSANofMove(joc.colorEngine,chosenNode->movePositions[0],chosenNode->movePositions[1],chosenNode->movedPiece);



}

