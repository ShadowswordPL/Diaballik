#include "Diaballik.h"
AIPlayer::AIPlayer()
{
}

AIPlayer::AIPlayer( PlayerColor color, MainGame* maingame, 
					GameBoardLay* gameboardlay, Diaballik* diaballik)
{
	this->color = color;
	this->maingame = maingame;
	this->gameboardlay = gameboardlay;
	this->diaballik = diaballik;
	isthinking = false;
}

//Return string with cords of b and e position
String AIPlayer::Cords(GameBoardPos b, GameBoardPos e)
{
	return	GameBoardLay::IntToBigLetter(b.x) +
			GameBoardLay::IntToNumber(b.y) +
			TO+
			GameBoardLay::IntToBigLetter(e.x) +
			GameBoardLay::IntToNumber(e.y);
}

//Return true if AI player make any action
bool AIPlayer::Play(bool hint)
{
	if(!maingame->aithinking && !isthinking)
	{
		if(maingame->mode == game && (maingame->currentmaxmove == maingame->currentmove || hint))
		{
			if
			(
				(
					maingame->CurrentPlayer().type == cpu && 
					maingame->gamehistory.vhistory[maingame->currentmove].GetColor() == color
				) ||
				hint
			)
			{
				isthinking = true;
				maingame->aithinking = true;
				diaballik->DrawInterface();
				AIMove thismove = AIThink();
				if(hint)
				{
					if(thismove.t == pawn)
					{
						PromptOK(HINTPAWN + Cords(thismove.b, thismove.e));	
					}
					
					if(thismove.t == ball)
					{
						PromptOK(HINTBALL + Cords(thismove.b, thismove.e));	
					}
					
					if(thismove.t == end)
					{
						PromptOK(HINTEND);	
					}
					maingame->gamehistory.vhistory
					.Remove(currentmove+1, maingame->currentmaxmove-currentmove);
					
					maingame->currentmaxmove = currentmove;
				}
				else
				{
					maingame->MakeMove(thismove.b, thismove.e, thismove.t);
				}
				isthinking = false;
				maingame->aithinking = false;
				return true;
			}
		}
	}
	return false;
}

//Function count value of pawn move
int AIPlayer::ValueMove(GameBoardFieldType t, int i, int j)
{
	int one = 1;
	int	result = 0;
	int y = BOARDSIZE-1;
	if (t == pawn2)
	{
		one = -1;	
		y = 0;
		j = BOARDSIZE - j - 1;
	}
	if(((color == blue) && (t == pawn1)) || ((color == red) && (t == pawn2)))
	{	
		result = result + j*MOVEBONUS*one;
		for(int x = 0; x < BOARDSIZE; x++)
		{
			if
			(
				maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[x][y].type == empty
			)
			{
				result = result + (BOARDSIZE-abs(x-i))*one;
			}
			if
			(
				maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[x][y].type == t
			)
			{
				result = result + (BOARDSIZE-abs(x-i))*MOVEBONUS2*one;
			}
		}
	}
	return result;
}

//Function count value of ball move
int AIPlayer::ValueBall(GameBoardFieldType t, int i, int j)
{
	int one = 1;
	int y = BOARDSIZE-1;
	if (t == ball2)
	{
		one = -1;	
		y = 0;
		j = BOARDSIZE - j - 1;
	}
	int  result = 0;
	if(((color == blue) && (t == ball1)) || ((color == red) && (t == ball2)))
	{
		result = BALLBONUS*j*one;
	}
	
	for(int x = 0; x < BOARDSIZE; x++)
	{
		if(((color == blue) && (t == ball1)) || ((color == red) && (t == ball2)))
		{
			if
			(
				maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[x][y].type == empty ||
				(
					(color == blue) && 
					(maingame->gamehistory.vhistory[maingame->currentmove]
					.GameBoardArray[x][y].type == pawn1)
				) || 
				(
					(color == red) && 
					(maingame->gamehistory.vhistory[maingame->currentmove]
					.GameBoardArray[x][y].type == pawn2)
				)
			)
			{
					result = result + (BOARDSIZE-abs(x-i))*one*BALLBONUS2;
			}
		}
	}
	return result;
}

//Function return value of gamestate
int AIPlayer::GameValue()
{	
	if(maingame->GameResult() == redwin)
	{
		return MINALPHA;	
	}
	if(maingame->GameResult() == bluewin)
	{
		return MAXBETA;	
	}
	
	int result = 0;
	for(int i = 0; i < BOARDSIZE; i++)
	{
		for(int j = 0; j < BOARDSIZE; j++)
		{
			if
			(
				maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[i][j].type == pawn1
			)
			{
				result = result + ValueMove(pawn1, i, j);
			}
			
			if
			(
				maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[i][j].type == pawn2
			)
			{
				result = result + ValueMove(pawn2, i, j);
			}
			
			if
			(
				maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[i][j].type == ball1
			)
			{
				result = result + ValueBall(ball1, i, j);
			}
			
			if
			(
				maingame->gamehistory.vhistory[maingame->currentmove]
				.GameBoardArray[i][j].type == ball2
			)
			{
				result = result + ValueBall(ball2, i, j);
			}
			
			result = result + Random(RANDOM) - RANDOM/2;
		}
	}
	
	return result;
}

//Return maximum if max == true or minimum otherwise
int AIPlayer::MinMax(int a, int b, bool max)
{
	if(max)
	{
		if(a > b)
		{
			return a;
		}
		return b;
	}
	if(a > b)
	{
		return b;
	}
	return a;
}

//Function trying to move from pos (x1,y1) to (x2, y2) and return move value
int AIPlayer::MoveTo(int x1, int y1, int x2, int y2, int depth, int alpha, int beta, bool max)
{
	if(gameboardlay->PossibleMove(GameBoardPos(x1, y1), GameBoardPos(x2, y2)))
	{
		maingame->MakeMove(GameBoardPos(x1, y1), GameBoardPos(x2, y2), pawn);
		return AlphaBeta(depth, alpha, beta);			
	}
	if(max)
	{
		return MINALPHA;
	}
	return MAXBETA;
}

//Function trying to throw from pos (x1,y1) to (x2, y2) and return move value
int AIPlayer::ThrowTo(int x1, int y1, int x2, int y2, int depth, int alpha, int beta, bool max)
{
	if(gameboardlay->PossibleThrow(GameBoardPos(x1, y1), GameBoardPos(x2, y2)))
	{
		maingame->MakeMove(GameBoardPos(x1, y1), GameBoardPos(x2, y2), ball);
		return AlphaBeta(depth, alpha, beta);			
	}
	if(max)
	{
		return MINALPHA;
	}
	return MAXBETA;
}

//Function remove moves made by AI to depth 
void AIPlayer::RemoveMoves(int depth)
{
	maingame->currentmove = currentmove+AIDEPTH-depth-1;	
}

//Function check if new move have better value then old one
void AIPlayer::CheckValue(  int value, int &oldvalue, GameBoardPos b, GameBoardPos e, 
							MoveType type, AIMove &mv, bool max)

{
	if((oldvalue < value && max) || (oldvalue > value && !max) )
	{
			mv.b = b;
			mv.e = e;
			mv.t = type;
	}
	oldvalue = value;
}

//Function ckecks all possible moves from current state
int AIPlayer::AllPossible(int depth, int alpha, int beta, bool max, AIMove &mv)
{
	int value = MinMax(alpha, beta, !max);
	int oldvalue = value;
	for(int i = 0; i < BOARDSIZE; i++)
	{
		for(int j = 0; j < BOARDSIZE; j++)
		{
			if(gameboardlay->PossibleSelect(GameBoardPos(i, j)))
			{
				if(maingame->gamehistory.vhistory[maingame->currentmove].moves > 0)
				{
					value = MinMax(value, MoveTo(i, j, i+1, j, depth, alpha, beta, max), max);
					CheckValue( value, oldvalue, GameBoardPos(i, j),
								GameBoardPos(i+1, j), pawn, mv, max); 
					RemoveMoves(depth);
					
					value = MinMax(value, MoveTo(i, j, i-1, j, depth, alpha, beta, max), max);
					CheckValue( value, oldvalue, GameBoardPos(i, j), 
								GameBoardPos(i-1, j), pawn, mv, max); 
					RemoveMoves(depth);
					
					value = MinMax(value, MoveTo(i, j, i, j+1, depth, alpha, beta, max), max);
					CheckValue( value, oldvalue, GameBoardPos(i, j), 
								GameBoardPos(i, j+1), pawn, mv, max); 
					RemoveMoves(depth);
					
					value = MinMax(value, MoveTo(i, j, i, j-1, depth, alpha, beta, max), max);
					CheckValue( value, oldvalue, GameBoardPos(i, j), 
								GameBoardPos(i, j-1), pawn, mv, max); 
					RemoveMoves(depth);
				}
				else
				{
					for(int i1 = 0; i1 < BOARDSIZE; i1++)
					{
						for(int j1 = 0; j1 < BOARDSIZE; j1++)
						{
							value = MinMax( value, ThrowTo(i, j, i1, j1, 
											depth, alpha, beta, max), max);
							CheckValue( value, oldvalue, GameBoardPos(i, j), 
										GameBoardPos(i1, j1), ball, mv, max); 
							RemoveMoves(depth);
						}
					}
				}
			}
		}
	}
	
	if(mv.b.x == mv.e.x && mv.b.y == mv.e.y)
	{
		mv.t = end;	
	}
	
	return value;
}

//Function run alpha beta algorithm for current state
int AIPlayer::AlphaBeta(int depth, int alpha, int beta)
{
	diaballik->Sync();
	diaballik->ProcessEvents();

	AIMove dummy = AIMove();

	if ((depth == 0))
	{
		return GameValue();	
	}
	if(maingame->gamehistory.vhistory[maingame->currentmove].GetColor() == red)
	{
		beta = AllPossible(depth-1, alpha, beta, false, dummy);
		if(alpha > beta)
		{
			return MAXBETA;	
		}
		return beta;
	}
	else
	{
		alpha = AllPossible(depth-1, alpha, beta, true, dummy);
		if(alpha > beta)
		{
			return MINALPHA;	
		}
		return alpha;
	}
}

//Function launch Ai making of moves
AIMove AIPlayer::MakeMove()
{
	AIMove result;
	bool isplayerone = color == blue;	
	int x = AllPossible(AIDEPTH-1, MINALPHA, MAXBETA, isplayerone, result);
	return result;
}

//Function launch AI thinking
AIMove AIPlayer::AIThink()
{
	PlayerType p1 = maingame->player1.type;
	PlayerType p2 = maingame->player2.type;
	maingame->player1.type = human;
	maingame->player2.type = human;
	currentmove = maingame->currentmove;
	AIMove result = MakeMove();	
		
	maingame->player1.type = p1;
	maingame->player2.type = p2;
	return result;
}
