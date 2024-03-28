#include "includes.h"

namespace soundmath
{
	// return the stiffness coefficient so that relaxation
	// occurs in k seconds (for interpolation)
	double relaxation(double k)
	{
		if (k == 0)
			return 0; 
		return pow(2.0, order / (fmax(0, k) * SR));
	}

	// midi to frequency
	double mtof(double midi)
	{
		return A4 * pow(2, (midi - 69) / 12);
	}

	double ftom(double frequency)
	{
		return 69 + log2(frequency / A4) * 12;
	}

	double atodb(double amplitude)
	{
		return 20 * log(amplitude);
	}

	double dbtoa(double db)
	{
		return pow(10, db / 20);
	}

	std::string notename(int midi)
	{
		static std::string notes = "C C#D D#E F F#G G#A A#B ";
		return notes.substr(2 * (midi % 12), 2) + std::to_string(midi / 12 - 1);
	}

	template <typename T> int sgn(T val)
	{
	    return (T(0) < val) - (val < T(0));
	}
}