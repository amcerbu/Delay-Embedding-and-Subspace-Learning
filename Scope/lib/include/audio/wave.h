// wave.h
#pragma once

#include "includes.h"

#define FUNCTIONAL

namespace soundmath
{
	const int TABSIZE = 65536;

	enum Interp
	{
		none = 0, linear = 1, quadratic = 2, cubic = 3
	};

	template <typename T> class Wave
	{
	public:
		Wave() { }
		~Wave() { }

		Wave(std::function<T(double)> shape, Interp interp = Interp::cubic, T left = 0, T right = 1, bool periodic = true)
		{
			this->shape = shape;
			this->interp = interp;
			this->left = left;
			this->right = right;
			this->periodic = periodic;

			for (int i = 0; i < TABSIZE; i++)
			{
				T phase = (T) i / TABSIZE;
				table[i] = shape((1 - phase) * left + phase * right);
			}

			this->endpoint = shape(right);
		}

		T none(int center)
		{
			return table[center];
		}

		T linear(int center, int after, T disp)
		{
			return table[center] * (1 - disp) + table[after] * disp;
		}

		T quadratic(int before, int center, int after, T disp)
		{
			return table[before] * ((disp - 0) * (disp - 1)) / ((-1 - 0) * (-1 - 1)) + 
				   table[center] * ((disp + 1) * (disp - 1)) / (( 0 + 1) * ( 0 - 1)) + 
				   table[after]  * ((disp + 1) * (disp + 0)) / (( 1 + 1) * ( 1 - 0));
		}

		T cubic(int before, int center, int after, int later, T disp)
		{
			return table[before] * ((disp - 0) * (disp - 1) * (disp - 2)) / ((-1 - 0) * (-1 - 1) * (-1 - 2)) + 
				   table[center] * ((disp + 1) * (disp - 1) * (disp - 2)) / (( 0 + 1) * ( 0 - 1) * ( 0 - 2)) + 
				   table[after]  * ((disp + 1) * (disp + 0) * (disp - 2)) / (( 1 + 1) * ( 1 - 0) * ( 1 - 2)) + 
				   table[later]  * ((disp + 1) * (disp + 0) * (disp - 1)) / (( 2 + 1) * ( 2 - 0) * ( 2 - 1));
		}

		#ifdef FUNCTIONAL 

		T lookup(T input)
		{
			return shape(input);
		}

		#else 

		T lookup(T input)
		{
			T phase = (input - left) / (right - left);
			
			// get value at endpoint if input is out of bounds
			if (!periodic && (phase < 0 || phase >= 1))
			{
				if (phase < 0)
					return none(0);
				else
					return endpoint;
			}
			else
			{
				phase += 1;
				phase -= int(phase);

				int center = (int)(phase * TABSIZE) % TABSIZE;
				int before = (center - 1 + TABSIZE) % TABSIZE;
				int after = (center + 1) % TABSIZE;
				int later = (center + 2) % TABSIZE;

				T disp = (phase * TABSIZE - center);
				disp -= int(disp);

				// interpolation
				switch (interp)
				{
					case Interp::none : return none(center);
					case Interp::linear : return linear(center, after, disp);
					case Interp::quadratic : return quadratic(before, center, after, disp);
					case Interp::cubic : return cubic(before, center, after, later, disp);
				}
			}
		}

		#endif

		T operator()(T phase)
		{
			return lookup(phase);
		}

		Wave<T> operator+(const Wave<T>& other)
		{
			Wave<T> sum;
			for (int i = 0; i < TABSIZE; i++)
			{
				sum.table[i] = this->table[i] + other.table[i];
			}
			return sum;
		}

	protected:
		T table[TABSIZE];

	private:
		Interp interp;
		T left; // input phases are interpreted as lying in [left, right)
		T right;
		bool periodic;

		T endpoint; // if (this->periodic == false), provides a value for (*this)(right)

		std::function<T(double)> shape;

	};

	Wave<double> saw([] (double phase) -> double { return 2 * phase - 1; }, Interp::linear);
	Wave<double> triangle([] (double phase) -> double { return abs(fmod(4 * phase + 3, 4.0) - 2) - 1; }, Interp::linear);
	Wave<double> square([] (double phase) -> double { return phase > 0.5 ? 1 : (phase < 0.5 ? -1 : 0); }, Interp::none);
	Wave<double> phasor([] (double phase) -> double { return phase; }, Interp::linear);
	// Wave<double> noise([] (double phase) -> double { return  2 * ((double)rand() / RAND_MAX) - 1; }, Interp::linear);
	Wave<double> cycle([] (double phase) -> double { return sin(2 * PI * phase); });
	Wave<double> hann([] (double phase) -> double { return 0.5 * (1 - cos(2 * PI * phase)); });
	Wave<double> halfhann([] (double phase) -> double { return sqrt(0.5 * (1 - cos(2 * PI * phase))); });
	Wave<double> limiter([] (double phase) -> double { return 2.0 / PI * atan(phase); }, Interp::linear, -100, 100, false);
}
