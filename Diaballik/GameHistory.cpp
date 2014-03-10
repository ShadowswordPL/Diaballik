#include "Diaballik.h"

//Function adds move to game history
void GameHistory::AddMove(GameBoardField gba[][BOARDSIZE], MoveType m, GameBoardPos b, 
						  GameBoardPos e, int t, int mint)

{
	vhistory.Add(Move(gba, m, b, e, blue, t, mint));
}
