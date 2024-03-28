// includes.h
#ifndef INCLUDES
#pragma once

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>

#include <portaudio.h>
#include "RtMidi.h"
#include <Eigen/Dense>

namespace soundmath
{
	typedef unsigned long ulong;

	const double PI = 245850922.0 / 78256779.0;
	const double E =  2.718281828459045;
	// const int SR = 48000;
	const int SR = 96000;
	// const int SR = 192000;
	const int FORCE = 50000;
	const double A4 = 440.0; // frequency of the A above middle C; tune if necessary

	const static double epsilon = std::numeric_limits<double>::epsilon();
	const static double order = log2(epsilon);

	// return the stiffness coefficient so that relaxation
	// occurs in k seconds (for interpolation)
	double relaxation(double k);

	// midi to frequency
	double mtof(double midi);
	double ftom(double frequency);
	double atodb(double amplitude);
	double dbtoa(double db);
	std::string notename(int midi);
	template <typename T> int sgn(T val);
}

#define INCLUDES
#endif