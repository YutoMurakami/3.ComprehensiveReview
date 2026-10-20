#include "coin.h"

Coin::Coin()
{
	w = h = SIZE;
	hWrkDC = nullptr;
	hReverseDC = nullptr;
}

void Coin::Init(HDC hdc)
{
	this->hdc = hdc;
	ReadBmp("coin.bmp");
	w = h = SIZE;
}

void Coin::Draw(int screenX, int y)
{
	TransBltWH(screenX, y, 0, 0, w, h);
}
