#pragma once
#include <SDL2/SDL.h>

class Color
{
public:
	Color();
	Color(const Color& other);

	void hsva(double hue, double sat, double val, double alph = 1);
	void rgba(double red, double blu, double gre, double alph = 1);

	SDL_Color raw();

private:
	void compute();
	void cubeclip(double hue);

	double red, blu, gre, alph;
	SDL_Color internals;
};