#ifndef _Diaballik_Enums_h_
#define _Diaballik_Enums_h_

	//All enums and structs used in program

	//Enum of state of every gameboard field
	enum GameBoardFieldType
	{
    	empty = 0,
    	pawn1 = 1,
    	ball1 = 2,
    	pawn2 = 3,
    	ball2 = 4,
   	};
   	
   	//Struct contains position of field on gameboard
   	struct GameBoardPos
	{    	
      	char x;
    	char y;
    	GameBoardPos()
    	{
    		x = 0;
    		y = 0;	
    	}
	   
		GameBoardPos(char x, char y)
	    {	
	    	this->x = x;
	    	this->y = y;	
	    }
   	};
   	
   	//Enum of game modes
   	enum Mode
   	{
   		none = 0,
   		game = 1,
   		editor = 2,
   	};
   	
   	 //Enum of player types
   	enum PlayerType
   	{
   		human = 0,
   		cpu = 1,
   	};
   	
   	enum PlayerColor
   	{
   		blue = 0,
   		red = 1,	
   	};
   	
   	//Enum of move types
   	enum MoveType
   	{
   		nonemove = 0,
   		pawn = 1,
   		ball = 2,
   		end = 3,
   		edit = 4,
   	};
   	
   	//Enum of messenges returned by game judge
   	enum JudgeMsg
   	{
   		nonemsg = 0,
		changeplayer = 1,
		bluewin = 2,
		redwin = 3,
   		draw = 4,
   	};
   	
   	//Enum of moves returned by AI
   	struct AIMove
   	{
   		GameBoardPos b;
   		GameBoardPos e;
   		MoveType t;
   	};
		
#endif
