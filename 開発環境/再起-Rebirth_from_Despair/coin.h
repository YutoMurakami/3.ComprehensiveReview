#pragma once
#include "image.h"

class Coin : public Image
{
public:
	Coin();
	void Init(HDC hdc);
	void Draw(int screenX, int y);

private:
	static const int SIZE = 32;
};
