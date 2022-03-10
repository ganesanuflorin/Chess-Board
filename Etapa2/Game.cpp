#include "Game.h"
#include <stdio.h>
#include "stuff.h"
#include<stdlib.h>
#include "Xboard.h"
#include <string.h>
#include "Tools.h"
#include "Mind.h"
#include "Book.h"


extern Xboard xboard;
extern Mind mind;

//DEBUG
FILE* debug_file;

Game::Game() {
    currentBoard.InitializePieces();
	movesDone=0;
	lateGame=false;
}


Game::~Game() {
}

void Game::initialize(){

}
//obtine tabla ChessBoard curenta;
ChessBoard Game::getCurrentBoard(){
    return currentBoard;
}

//seteaza tabla ChessBoard curenta;
void Game::setCurrentBoard(ChessBoard newCurrentBoard){
    currentBoard=newCurrentBoard;
}

//seteaza culoarea curenta a engine-ului
void Game::setGameColor(COLOR culoare){
    colorEngine=culoare;
}

//seteaza timerii curenti ai jocului
void Game::setGameTimers(int new_time,int new_otime){
    time=new_time;
    otime=new_otime;
}

//Functia de resetare a jocului. Se reinitializeaza tabla;
void Game::gameReset(){
    currentBoard.InitializePieces();
}

void Game::play(){

    char *buf=0;
    Book newBook;
    
    
    //Obtinem prima mutare, daca exista. Daca e randul meu la mutare, buf=null
    buf=xboard.getFirstMove();
  
    //daca a mutat oponentul, actualizez tabla
    if(buf!=NULL)
    {
	currentBoard.updateBoard(buf,1-colorEngine);
    }

    //DEBUG: Deschidem fisierele de debug, cate unul pentru fiecare player
    if(colorEngine==BLACK)
		debug_file=fopen("CiucasDebugFile_BLACK.txt","w");
    else
		debug_file=fopen("CiucasDebugFile_WHITE.txt","w");

    setbuf(debug_file,NULL);


    if(colorEngine==BLACK)
        fprintf(debug_file,"CiucasLa2jumate is playing BLACK\n");
    else
        fprintf(debug_file,"CiucasLa2jumate is playing WHITE\n");

    //DESCHIDERI
    while(1)
    {
	//Obtinem mutarea urmatoare
	buf=newBook.getNextMove(currentBoard,colorEngine);
	//Daca buf=0, inseamna ca nu mai avem deschideri si iesim;
	if(buf==0)
	    break;

	movesDone++;
	//actualizez tabla interna
	currentBoard.updateBoard(buf,colorEngine);
	
	//trimit mutarea la XBoard
	xboard.sendMoveToXboard(buf);
	free(buf);

	fprintf(debug_file,"Obtinem mutarea adversarului...\n");
	//Obtin mutarea adversarului
	buf=xboard.getMoveFromXboard();

	//Actualizez tabla curenta, dupa mutarea adversarului
	currentBoard.updateBoard(buf,1-colorEngine);
	fprintf(debug_file,"Mutare adversar:%s\n",buf);
	currentBoard.printBoard();

	free(buf);
    }

    fprintf(debug_file,"Exiting OPENINGS Area!\n\n");
    
    while(1)
    {
		if(colorEngine==WHITE)
			   fprintf(debug_file,"Engine WHITE:\n");
		else
			   fprintf(debug_file,"Engine BLACK:\n");

        //Obtinem urmatoarea mutare din functia mind.
        buf=mind.getSanNegaMax();

        //actualizez tabla interna
		currentBoard.updateBoard(buf,colorEngine);
		fprintf(debug_file,"Mutarea este %s \n",buf);

		//Numar de mutari
		movesDone++;
		if(movesDone==40)
		{
			lateGame=true;
			movesDone=0;
		}

		xboard.sendMoveToXboard(buf);
		free(buf);

		fprintf(debug_file,"Obtinem mutarea adversarului...\n");
		//Obtin mutarea adversarului
		buf=xboard.getMoveFromXboard();

		//Actualizez tabla curenta, dupa mutarea adversarului
		currentBoard.updateBoard(buf,1-colorEngine);
		fprintf(debug_file,"Tabla curenta dupa mutarea adversarului:%s\n",buf);
		currentBoard.printBoard();

		free(buf);
    }

}