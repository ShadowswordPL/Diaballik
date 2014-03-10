#include "Diaballik.h"

Player::Player(){}

Player::Player(PlayerType t, PlayerColor c)
{
	type = t;
}

PlayerColor Player::GetColor()
{
	return color;
}


