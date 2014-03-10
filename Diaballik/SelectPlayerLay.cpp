#include "Diaballik.h"

//Function return value when player clicks "Ok"
bool SelectPlayerLayout::Accept()
{
	return true;
}

//Funcion set defoult values of player switch
SelectPlayerLayout::SelectPlayerLayout()
{
	p1switch = 1;
	p2switch = 0;
	CtrlLayoutOK(*this, SELECTPLAYER);
}

