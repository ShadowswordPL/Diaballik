#include "Diaballik.h"

#define IMAGECLASS MyImages
#define IMAGEFILE  <Diaballik/Pawn.iml>
#include <Draw/iml_source.h>

GameBoardField::GameBoardField(){}

GameBoardField::GameBoardField(GameBoardPos p)
{
	type = empty;
	position = p;
	clicked = false;
	possible = false;
}

GameBoardField::GameBoardField(GameBoardFieldType t, GameBoardPos p)
{
	type = t;
	position = p;
	clicked = false;
	possible = false;
}

//Function draw each game field
void GameBoardField::DrawField(Size s, PaintingPainter& p)
{
	Color c = LIGHTBROWN;
	if((position.x + position.y) % 2 == 1)
	{
		c = DARKBROWN;
	}

	p.DrawRect
	(
		s.cx * BOARDMARGIN + position.x * FieldWidth(s), 
		s.cy * BOARDMARGIN + position.y * FieldHeight(s),
		FieldWidth(s), 
		FieldHeight(s),  
		c
	);
	
	if(possible)
	{
		p.DrawRect
		(
			s.cx * BOARDMARGIN + position.x * FieldWidth(s) + s.cx * BOARDMARGIN/4, 
			s.cy * BOARDMARGIN + position.y * FieldHeight(s) + s.cy * BOARDMARGIN/4,
			FieldWidth(s) - s.cx * BOARDMARGIN/2, 
			FieldHeight(s) - s.cy * BOARDMARGIN/2,  
			POSSIBLE
		);	
	}
	
	if(type != empty)
	{
		p.DrawImage
		(
			s.cx * BOARDMARGIN + position.x * FieldWidth(s),
			s.cy * BOARDMARGIN + position.y * FieldHeight(s),
			FieldWidth(s),
			FieldHeight(s),  
			PlayerImage(type , clicked)
		);
	}
		
	if(type == ball1 || type == ball2)
	{
		p.DrawImage
		(
			s.cx * BOARDMARGIN + position.x * FieldWidth(s),
			s.cy * BOARDMARGIN + position.y * FieldHeight(s),
			FieldWidth(s),
			FieldHeight(s),  
			MyImages::Ball()
		);
	}
	
}

double GameBoardField::FieldWidth(Size s)
{
	return  (s.cx - 2 * s.cx * BOARDMARGIN) / BOARDSIZE;
}

double GameBoardField::FieldHeight(Size s)
{
	return (s.cy - 2 * s.cy * BOARDMARGIN) / BOARDSIZE;
}

//Function set correct image of pawn
Image GameBoardField::PlayerImage(GameBoardFieldType m, bool clicked)
{
	if(m == pawn1 || m == ball1)
	{
		if(clicked)
		{
			return MyImages::BluePawnClicked();
		}
		return MyImages::BluePawn();
	}
	if(clicked)
	{
		return MyImages::RedPawnClicked();
	}
	return MyImages::RedPawn();
}
