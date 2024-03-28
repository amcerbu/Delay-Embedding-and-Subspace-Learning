#include "RenderWindow.h"
#include "Color.h"
#include <iostream>
// #include "Vector.h"
// #include "Common.h"

#include <cmath>

Color::Color() : red(0), gre(0), blu(0), alph(0)
{
	compute();
}

Color::Color(const Color& other)
{
	red = other.red;
	blu = other.blu;
	gre = other.gre;
	alph = other.alph;
}

// hue: angle; saturation: radius; value: distance along diagonal
void Color::hsva(double hue, double sat, double val, double alph)
{
	// starting point:
	// (1, 1, 1) * val + 
	// 2 * val * (1 - val) * sat * (rfactor * (1, -0.5, -0.5) + gfactor * (-0.5, 1, -0.5) + bfactor * (-0.5, -0.5, 1))
	// maybe jfactor = cos(2 * pi * j / 3) with j = [0,1,2] (r, g, b).

	double rfactor = cos(2 * M_PI * (hue - 0.0 / 3));
	double gfactor = cos(2 * M_PI * (hue - 1.0 / 3));
	double bfactor = cos(2 * M_PI * (hue - 2.0 / 3));

	val = (val < 0 ? 0 : (val > 1 ? 1 : val));

	// std::cout << rfactor << std::endl;

	red = val + 2 * val * (1 - val) * sat * ( 1.0 * rfactor - 0.0 * gfactor - 0.0 * bfactor);
	gre = val + 2 * val * (1 - val) * sat * (-0.0 * rfactor + 1.0 * gfactor - 0.0 * bfactor);
	blu = val + 2 * val * (1 - val) * sat * (-0.0 * rfactor - 0.0 * gfactor + 1.0 * bfactor);
	this->alph = alph;

	cubeclip(val);
	compute();
}

void Color::rgba(double red, double blu, double gre, double alph)
{
	this->red = red;
	this->blu = blu;
	this->gre = gre;
	this->alph = alph;

	compute();
}

SDL_Color Color::raw()
{
	return internals;
}

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

// move along line segment to val * (1,1,1)
void Color::cubeclip(double val)
{
	// at least one of these is nonzero
	int rori, bori, gori;
	rori = (red < 0 ? -1 : (red > 1 ? 1 : 0));
	gori = (gre < 0 ? -1 : (gre > 1 ? 1 : 0));
	bori = (blu < 0 ? -1 : (blu > 1 ? 1 : 0));

	bool outside = (rori != 0 || bori != 0 || gori != 0);
	if (outside)
	{
		double rint = 0;
		double gint = 0;
		double bint = 0;

		// find distance to nearest plane
		if (rori != 0)
			rint = fmin((1.0 - red) / (val - red), (-red) / (val - red));
		
		if (gori != 0)
			gint = fmin((1.0 - gre) / (val - gre), (-gre) / (val - gre));
		
		if (bori != 0)
			bint = fmin((1.0 - blu) / (val - blu), (-blu) / (val - blu));

		// take furthest point among these intersections
		double t = fmax(rint, fmax(gint, bint));

		red += t * (val - red);
		gre += t * (val - gre);
		blu += t * (val - blu);
	}

	// don't do anything if inside
	return;
}

void Color::compute()
{
	unsigned char r, g, b, a;
	const static double before = std::nextafter(1.0, 0.0);

	r = (int)(256 * std::fmax(0, std::fmin(red, before)));
	g = (int)(256 * std::fmax(0, std::fmin(gre, before)));
	b = (int)(256 * std::fmax(0, std::fmin(blu, before)));
	a = (int)(256 * std::fmax(0, std::fmin(alph, before)));

	internals = { r, g, b, a };
}