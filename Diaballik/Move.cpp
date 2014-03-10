#include "Diaballik.h"

//Move constructor
Move::Move( GameBoardField gba[][BOARDSIZE], MoveType m, GameBoardPos b, 
			GameBoardPos e, PlayerColor c, int t, int mint)
{
	for(int i = 0; i < BOARDSIZE; i++)
	{
		for(int j = 0; j < BOARDSIZE; j++)
		{
			GameBoardArray[i][j] = gba[i][j];
		}
	}
	movetype = m;
	bcords = b;
	ecords = e;
	pcolor = c;
	throws = t;
	moves = mint;
	end = false;
}

//Return toString of move
String Move::moveString()
{
	String stype = "";
	if (movetype == ball)
	{
		stype = BALL; 
	}
	else
	{
		stype = PAWN;
	}
	return  stype + " " + GameBoardLay::IntToBigLetter(bcords.x) + 
			GameBoardLay::IntToNumber(bcords.y) + " -> " + 
			GameBoardLay::IntToBigLetter(ecords.x)+GameBoardLay::IntToNumber(ecords.y);
}

GameBoardPos Move::GetBcords()
{
	return bcords;
}

GameBoardPos Move::GetEcords()
{
	return ecords;
}

PlayerColor Move::GetColor()
{
	if(end)
	{
		if(pcolor == blue)
		{
			return red;	
		}
		return blue;
	}
	return pcolor;
}

void Move::SetColor(PlayerColor pc)
{
	if(end)
	{
		if(pc == blue)
		{
			pcolor = red;	
		}
		else
		{
			pcolor = blue;	
		}
	}
	else
	{
		pcolor = pc;
	}
}

