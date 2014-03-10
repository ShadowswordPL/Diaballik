#include "Diaballik.h"

//Launching maingame class
MainGame::MainGame()
{
	Clear();
}

//Funcion checks if player definitly can move
bool MainGame::WillMove()
{
	if(aithinking)
	{
		return true;	
	}
	if(currentmove != currentmaxmove)
	{
		return PromptOKCancel(L_DELETEMOVE);
	}
	return true;
}

//Funcion clear gamestate
void MainGame::Clear()
{
	arbiter = false;
	aithinking = false;
	mode = none;
	player1 = Player(human, blue);
	player2 = Player(human, red);
	unsaved = false;
	currentmove = 0;
	currentmaxmove = 0;
	gamehistory.vhistory.Clear();	
}

//Function change current game mode
void MainGame::ChangeMode(Mode m)
{
	if(m == none)
	{
		Clear();
		StartNone();	
	}
	if(m == game)
	{
		Clear();
		StartNewGame();
	}
	if(m == editor)
	{
		StartEditor();
	}
	mode = m;	
}

//Function change current player
void MainGame::ChangePlayer(bool end)
{
	if(mode != editor)
	{
		gamehistory.vhistory[currentmove].end = end;
		if(end)
		{
			gamehistory.vhistory[currentmove].moves = DEFMOVES;
			gamehistory.vhistory[currentmove].throws = DEFTHROWS;
		}
	}
}

//Function checks if current player can move
bool MainGame::NoMoves()
{
	return  (gamehistory.vhistory[currentmove].moves  == 0) && 
			(gamehistory.vhistory[currentmove].throws == 0);
}

//Function checks if pawn on position p belong to player c
bool MainGame::PlayerPawn(GameBoardPos p, PlayerColor c)
{
	if(c == blue)
	{
		return (gamehistory.vhistory[currentmove].GameBoardArray[p.x][p.y].type == pawn1) ||
			   (gamehistory.vhistory[currentmove].GameBoardArray[p.x][p.y].type == ball1);
	}
	return  (gamehistory.vhistory[currentmove].GameBoardArray[p.x][p.y].type == pawn2) ||
			(gamehistory.vhistory[currentmove].GameBoardArray[p.x][p.y].type == ball2);
}

//Function checks if player c have unpassable line and opposit player touch it in set points
bool MainGame::HaveTouchedLine(PlayerColor c)
{
	PlayerColor current = blue;
	PlayerColor enemy = red;
	int touchingcurrent = 0;
	int touchingenemy = 0;
	if(c == red)
	{
		current = red;
		enemy = blue;	
	}
	for(int i = 0; i < BOARDSIZE; i++)
	{	
		for(int j = 0; j < BOARDSIZE; j++)
		{
			if(PlayerPawn(GameBoardPos(i, j), current))
			{
				if
				(
				 	((i+1 < BOARDSIZE && j - 1 >= 0) && 
				 	PlayerPawn(GameBoardPos(i+1, j-1), current)) ||
					((i+1 < BOARDSIZE) && PlayerPawn(GameBoardPos(i+1, j), current)) ||
					((i+1 < BOARDSIZE && j + 1 < BOARDSIZE) && 
					PlayerPawn(GameBoardPos(i+1, j+1), current))
				)
				{
					touchingcurrent++;
				}
				if
				(
					((j - 1 >= 0) && PlayerPawn(GameBoardPos(i, j-1), enemy)) ||
					((j + 1 < BOARDSIZE) && PlayerPawn(GameBoardPos(i, j+1), enemy))
				)
				{
					touchingenemy++;
				}
			}
		}
	}
	return touchingcurrent == BOARDSIZE-1 && touchingenemy >= MAXTOUCH;
}

//Function checks if current gamestate repeats
bool MainGame::SameState()
{
	for(int x = 0; x < currentmove-NOSAMESTATE; x++)
	{
		int same = 0;
		for(int i = 0; i < BOARDSIZE; i++)
		{
			for(int j = 0; j < BOARDSIZE; j++)
			{
				if
				(
					gamehistory.vhistory[x].GameBoardArray[i][j].type == 
					gamehistory.vhistory[currentmove].GameBoardArray[i][j].type
				)
				{
					same++;
				}
			}
		}
		if(same == BOARDSIZE*BOARDSIZE)
		{
			return true;	
		}
	}
	return false;
}

//Function return result of current gamestate
JudgeMsg MainGame::GameResult()
{
	if(SameState())
	{
		return draw;	
	}
	if(HaveTouchedLine(blue) && HaveTouchedLine(red))
	{
		if(CurrentPlayer().GetColor() == blue)
		{
			return redwin;	
		}
		return bluewin;
	}
	if(HaveTouchedLine(blue))
	{
		return redwin;	
	}
	if(HaveTouchedLine(red))
	{
		return bluewin;	
	}
	for(int i = 0; i < BOARDSIZE; i++)
	{
		if(gamehistory.vhistory[currentmove].GameBoardArray[i][0].type == ball2)
		{
			return redwin;	
		}
		if(gamehistory.vhistory[currentmove].GameBoardArray[i][BOARDSIZE-1].type == ball1)
		{
			return bluewin;	
		}
	}
	return nonemsg;
}

GameBoardFieldType MainGame::FieldType(GameBoardPos b)
{
	return gamehistory.vhistory[currentmove].GameBoardArray[b.x][b.y].type;
}

//Function make move type m from b position to e
void MainGame::MakeMove(GameBoardPos b, GameBoardPos e, MoveType m)
{
	if(WillMove())
	{
		int mvs = gamehistory.vhistory[currentmove].moves;
		int thrws = gamehistory.vhistory[currentmove].throws;
		if(currentmove != currentmaxmove)
		{
			gamehistory.vhistory.Remove(currentmove+1, currentmaxmove-currentmove);
			currentmaxmove = currentmove;
		}
		GameBoardField newboard[BOARDSIZE][BOARDSIZE];
		for(int i = 0; i < BOARDSIZE; i++)
		{
			for(int j = 0; j < BOARDSIZE; j++)
			{
				newboard[i][j] = gamehistory.vhistory[currentmove].GameBoardArray[i][j];
			}
		}
		newboard[b.x][b.y].type = FieldType(e);
		newboard[e.x][e.y].type = FieldType(b);
		if(m == pawn)
		{
			mvs -= GameBoardLay::BoardDistance(b, e);	
		}
		if(m == ball)
		{
			thrws--;	
		}
		if(m == end)
		{
			mvs = 0;
			thrws = 0;
		}
		gamehistory.vhistory.Add(Move(newboard, m, b, e, gamehistory.vhistory[currentmove].GetColor(), thrws, mvs));
		if(mode == editor)
		{
			gamehistory.vhistory[currentmaxmove+1].movetype = edit;
			gamehistory.vhistory.Remove(gamehistory.vhistory.GetCount()-2, 1);	
		}
		else
		{
			currentmove++;
			currentmaxmove++;	
		}
		if(thrws == 0 && mvs == 0)
		{
			if(mode != editor)
			{
				gamehistory.vhistory[currentmove].end = true;
				gamehistory.vhistory[currentmove].moves = DEFMOVES;
				gamehistory.vhistory[currentmove].throws = DEFTHROWS;
			}
		}
		unsaved = true;
	}
}

//Function set players selected in pop-up
void MainGame::SetPlayers()
{
	SelectPlayerLayout s;
	s.Execute();
	
	if(s.p1switch == human)
	{
		player1 = Player(human, blue);	
	}
	else
	{
		player1 = Player(cpu, blue);	
	}
	
	if(s.p2switch == human)
	{
		player2 = Player(human, red);	
	}
	else
	{
		player2 = Player(cpu, red);	
	}
}

//Function set board to the new game state
void MainGame::SetStartBoard()
{
	GameBoardField newboard[BOARDSIZE][BOARDSIZE];
	for (int i = 0; i < BOARDSIZE; i++)
	{
		for (int j = 0; j < BOARDSIZE; j++)
		{
			newboard[i][j] = GameBoardField(GameBoardPos(i, j));	
		}
	}
	for (int i = 0; i < BOARDSIZE; i++)
	{
		if(i == BOARDSIZE/2)
		{
			newboard[i][0] = GameBoardField(ball1, GameBoardPos(i, 0));
			newboard[i][BOARDSIZE-1] = GameBoardField(ball2, GameBoardPos(i, BOARDSIZE-1));
		}
		else
		{
			newboard[i][0] = GameBoardField(pawn1, GameBoardPos(i, 0));
			newboard[i][BOARDSIZE-1] = GameBoardField(pawn2, GameBoardPos(i, BOARDSIZE-1));
		}
	}
	
	gamehistory.AddMove(newboard, nonemove, GameBoardPos(0,0), 
						GameBoardPos(0,0), DEFTHROWS, DEFMOVES);
}

//Function start mode "none"
void MainGame::StartNone()
{
	GameBoardField newboard[BOARDSIZE][BOARDSIZE];
	for (int i = 0; i < BOARDSIZE; i++)
	{
		for (int j = 0; j < BOARDSIZE; j++)
		{
			newboard[i][j] = GameBoardField(GameBoardPos(i, j));	
		}
	}
	gamehistory.AddMove(newboard, nonemove, GameBoardPos(0,0), GameBoardPos(0,0), 0, 0);
}

//Function start mode "new game"
void MainGame::StartNewGame()
{
	SetPlayers();
	SetStartBoard();
	currentmove = 0;
	currentmaxmove = 0;
	arbiter = true;
}

//Function start mode "editor"
void MainGame::StartEditor()
{
	
}

//Function parse move i from history to QTF
String MainGame::QTFMove(int i)
{
	String pcolor = QTFRED;
	String bgcolor = "";
	String size = QTFSIZE;
	String txt = "";
	if 
	(
		(gamehistory.vhistory[i].GetColor() == blue && !gamehistory.vhistory[i].end) ||
		(gamehistory.vhistory[i].GetColor() == red && gamehistory.vhistory[i].end)
	)
	{
		pcolor = QTFBLUE;	
	}
	if(i == currentmove)
	{
		bgcolor = QTFBG;	
	}
	if(gamehistory.vhistory[i].movetype == edit)
	{
		txt = QTFEDITOR;	
	}
	else if (i == 0)
	{
		txt = QTFNEWGAME;
	}
	else if(gamehistory.vhistory[i].movetype == end)
	{
		txt = QTFENDMOVE;	
	}
	else
	{
		txt = Format("%d", i) +": " + gamehistory.vhistory[i].moveString();
	}
	String result = ("[+" + size + pcolor + bgcolor + " [^" + Format("%d", i) + "^ " + txt +  "]]");
	return result;  
}

//Function return full game history parsed to QTF
String MainGame::QTFHistory()
{
	String qtf = "";
	for(int i = 0; i <= currentmaxmove; i++)
	{
		qtf = QTFMove(i) +"&"+ qtf;
	}
	return qtf;
}

//Function return current player
Player MainGame::CurrentPlayer()
{
	if(gamehistory.vhistory[currentmove].GetColor() == blue)
	{
		return player1;	
	}
	return player2;
}

//Function judge the game
JudgeMsg MainGame::Judge()
{
	if(arbiter)
	{
		if(gamehistory.vhistory[currentmove].end)
		{
			if(GameResult() != nonemsg)
			{
				return GameResult();	
			}
		}
	}
	return nonemsg;
}

