#include "Diaballik.h"

//Drawing full gamefield
void GameBoardLay::DrawLay(GameBoardField GameBoardArray[][BOARDSIZE])
{
	this->Background(BGBROWN);
	PaintingPainter p(this->GetSize());

	DrawMargin(p);
	
	for(int i = 0; i < BOARDSIZE; i++)
	{
		for(int j = 0; j < BOARDSIZE; j++)
		{
			GameBoardArray[i][j].DrawField(this->GetSize(), p);
		}
	}
			
	this->operator=(p);		
}

//Drawing margin letter and numbers
void GameBoardLay::DrawMargin(Painter &p)
{
	for(int i = 0; i < BOARDSIZE; i++)
	{
		p.DrawText
		(
			this->GetSize().cx*BOARDMARGIN + 
			(this->GetSize().cx - 2 * this->GetSize().cx*BOARDMARGIN)/BOARDSIZE/3 + 
			(this->GetSize().cx - 2 * this->GetSize().cx*BOARDMARGIN)*i/BOARDSIZE,
			BOARDLETTERMARGIN,
			this->IntToBigLetter(i),
			StdFont(),
			White()	
		);	
		
		p.DrawText
		(
			BOARDNUMBERMARGIN,
			this->GetSize().cy*BOARDMARGIN + 
			(this->GetSize().cy - 2 * this->GetSize().cy*BOARDMARGIN)/BOARDSIZE/3 + 
			(this->GetSize().cy - 2 * this->GetSize().cy*BOARDMARGIN)*i/BOARDSIZE,
			this->IntToNumber(i),
			StdFont(),
			White()	
		);	
	}
}

void GameBoardLay::operator=(const Painting& p)
{ 
	Picture::operator=(p); 
}

String GameBoardLay::IntToBigLetter(int x)
{
	return Format("%c", x+'A');
}

String GameBoardLay::IntToNumber(int x)
{
	return Format("%d", x);
}

//Function select selected pawn
void GameBoardLay::Select(GameBoardPos s)
{
	Unselect();
	
	maingame->gamehistory.vhistory[maingame->currentmove]
	.GameBoardArray[s.x][s.y].clicked = true;
	
	for(int i = 0; i < BOARDSIZE; i++)
	{
		for(int j = 0; j < BOARDSIZE; j++)
		{
			if(PossibleThrow(s, GameBoardPos(i, j)))
			{
				maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[i][j].possible = true;	
			}
		    else if(PossibleMove(s, GameBoardPos(i, j)))
			{
				if(FieldFromArray(i, j).type == empty)
				{
					maingame->gamehistory.vhistory[maingame->currentmove]
					.GameBoardArray[i][j].possible = true;	
				}
			}
		}
	}
}


//Function unselect all pawns
void GameBoardLay::Unselect()
{
	for(int i = 0; i < BOARDSIZE; i++)
	{
		for(int j = 0; j < BOARDSIZE; j++)
		{
			maingame->gamehistory.vhistory[maingame->currentmove]
			.GameBoardArray[i][j].clicked = false;
			maingame->gamehistory.vhistory[maingame->currentmove]
			.GameBoardArray[i][j].possible = false;
		}		
	}
}

//Funcion checks if any pawn is selected
GameBoardPos GameBoardLay::Selected()
{
	for(int i = 0; i < BOARDSIZE; i++)
	{
		for(int j = 0; j < BOARDSIZE; j++)
		{
			if(FieldFromArray(i, j).clicked)
			{
				return GameBoardPos(i, j);	
			}
		}		
	}
	return GameBoardPos(BOARDSIZE, BOARDSIZE);
}

GameBoardField GameBoardLay::FieldFromArray(int i, int j)
{
	return maingame->gamehistory.vhistory[maingame->currentmove].GameBoardArray[i][j];
}

//Function checks if on p field is any pawn
bool GameBoardLay::AnyPawn(GameBoardPos p)
{
	return  (FieldFromArray(p.x, p.y).type == pawn1) ||
			(FieldFromArray(p.x, p.y).type == pawn2);	
}

//Function checks if on p field is any ball
bool GameBoardLay::AnyBall(GameBoardPos p)
{
	return  (FieldFromArray(p.x, p.y).type == ball1) ||
			(FieldFromArray(p.x, p.y).type == ball2);		
}

////Function checks if on p field is any pawn or ball of player c
bool GameBoardLay::PlayerPawn(GameBoardPos p, PlayerColor c)
{
	if(c == blue)
	{
		return (FieldFromArray(p.x, p.y).type == pawn1) ||
			   (FieldFromArray(p.x, p.y).type == ball1);
	}
	return  (FieldFromArray(p.x, p.y).type == pawn2) ||
			(FieldFromArray(p.x, p.y).type == ball2);
}

//Function set iterator for NoEnemyInLine funcion
int GameBoardLay::SetIterator(int x1, int x2)
{
	int result = x2 - x1;
	if(result > 0)
	{
		return 1;	
	}
	if(result < 0)
	{
		return -1;	
	}
	return 0;
}

//Funcion checks if there is enemy between two pawns
bool GameBoardLay::NoEnemyInLine(GameBoardPos b, GameBoardPos e)
{
	int iteratorx = SetIterator(b.x, e.x);
	int iteratory = SetIterator(b.y, e.y);
	int sx = b.x;
	int sy = b.y;
	while(!(sx == e.x && sy == e.y))
	{
		if
		(
			(
				(maingame->gamehistory.vhistory[maingame->currentmove].GetColor() == blue) && 
				PlayerPawn(GameBoardPos(sx, sy), red)
			) ||
			(
				(maingame->gamehistory.vhistory[maingame->currentmove].GetColor() == red) && 
				PlayerPawn(GameBoardPos(sx, sy), blue)
			)
		)
		{
			return false;
		}
		sx = sx + iteratorx;
		sy = sy + iteratory;	
	}
	return true;
}

//Funcion checks if pawn from b can throw ball to e
bool GameBoardLay::PossibleThrow(GameBoardPos b, GameBoardPos e)
{
	if(maingame->mode == editor)
	{
			return  (
						((FieldFromArray(b.x, b.y).type == ball1) && 
						(FieldFromArray(e.x, e.y).type == pawn1)) ||
						((FieldFromArray(b.x, b.y).type == ball2) && 
						(FieldFromArray(e.x, e.y).type == pawn2))
					); 
	}
	if (!(b.x == e.x && b.y == e.y))
	{
		if((b.x == e.x) || (b.y == e.y) || (b.x+b.y) == (e.x+e.y) || (b.x-b.y) == (e.x-e.y))
		{
			
			if (NoEnemyInLine(b, e))		
			{
				return  (((FieldFromArray(b.x, b.y).type == ball1) && 
						(FieldFromArray(e.x, e.y).type == pawn1)) ||
						((FieldFromArray(b.x, b.y).type == ball2) &&
						(FieldFromArray(e.x, e.y).type == pawn2))) &&
						(maingame->gamehistory.vhistory[maingame->currentmove].throws > 0);
			}
		}
	}
	return false;
}

//Funcion checks if player can make two moves in one
bool GameBoardLay::CanDoubleMove(GameBoardPos b, GameBoardPos e)
{
	PlayerColor enemy = blue;
	if(maingame->CurrentPlayer().GetColor() == blue)
	{
		enemy = red;
	}
	if(BoardDistance(b, e) == 2)
	{
		if(b.x == e.x)
		{
			return  maingame->gamehistory.vhistory[maingame->currentmove]
					.GameBoardArray[b.x][(b.y+e.y)/2].type == empty;
		}
		if(b.y == e.y)
		{
			return  maingame->gamehistory.vhistory[maingame->currentmove]
					.GameBoardArray[(b.x+e.x)/2][b.y].type == empty;
		}
		return  maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[e.x][b.y].type == empty || 
				maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[b.x][e.y].type == empty;
	}
	return true;
}

//Funcion checks if pawn from b can move to e
bool GameBoardLay::PossibleMove(GameBoardPos b, GameBoardPos e)
{
	if(e.x< 0 || e.y < 0 || e.x >= BOARDSIZE || e.y >= BOARDSIZE)
	{
		return false;	
	}
	if(maingame->mode == editor)
	{
		return AnyPawn(b) && !AnyBall(e) && !AnyPawn(e);
	}
	if(CanDoubleMove(b, e))
	{
		return  AnyPawn(b) && !AnyBall(e) && !AnyPawn(e) && 
				(
					maingame->gamehistory.vhistory[maingame->currentmove].moves >= 
					BoardDistance(b, e)
				);
	}
	return false;
}

//Funcion checks if player can select pawn
bool GameBoardLay::PossibleSelect(GameBoardPos b)
{
	if(maingame->mode == editor)
	{
		return true;
	}
	return PlayerPawn(b, maingame->gamehistory.vhistory[maingame->currentmove].GetColor()) && 
	maingame->CurrentPlayer().type == human;
}

//Funcion count distance between b and e
int GameBoardLay::BoardDistance(GameBoardPos b, GameBoardPos e)
{
	return abs(e.x-b.x) + abs(e.y-b.y);
}

//Funcion supports clicking on gameboard
void GameBoardLay::LeftUp(Point p, dword keyflags)
{
	if(!maingame->aithinking)
	{
		if((Selected().x == ReturnSelectedPos(p).x) && (Selected().y == ReturnSelectedPos(p).y))
		{
			Unselect();	
		}
		
		else if((PossibleThrow(Selected(), ReturnSelectedPos(p))))
		{
			maingame->MakeMove(Selected(),ReturnSelectedPos(p), ball);
			Unselect();
		}
		
		else if(PossibleMove(Selected(), ReturnSelectedPos(p)))
		{
			maingame->MakeMove(Selected(), ReturnSelectedPos(p), pawn);
			Unselect();
		}
	
		else if(PossibleSelect(ReturnSelectedPos(p)))
		{
			Select(ReturnSelectedPos(p));
		}	
		diaballik->DrawInterface();
	}
}

//Function returns GameBoardPos of selected point on board
GameBoardPos GameBoardLay::ReturnSelectedPos(Point p)
{
	int x = p.x;
	int y = p.y;
	x = x - this->GetSize().cx*BOARDMARGIN;
	y = y - this->GetSize().cy*BOARDMARGIN;
	x = x/GameBoardField::FieldWidth(this->GetSize());
	y = y/GameBoardField::FieldHeight(this->GetSize());
	if(x >= 0 && x < BOARDSIZE && y >= 0 && y < BOARDSIZE)
	{
		return GameBoardPos(x, y);	
	}
	return GameBoardPos(BOARDSIZE, BOARDSIZE);
}
