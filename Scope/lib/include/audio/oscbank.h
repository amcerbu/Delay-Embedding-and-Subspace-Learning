// oscbank.h
#pragma once

#include "includes.h"
#include "multichannel.h"

using Eigen::Matrix;
using Eigen::Map;
using Eigen::Array;

namespace soundmath
{
	// bank of oscillators; each has an associated frequency and phase
	// both are represented as unit-norm complex numbers
	template <typename T, int N> class Oscbank : public Multichannel<T, N>
	{
	private:
		using typename Multichannel<T, N>::ArrayCT;
		using Multichannel<T, N>::active;
		using Multichannel<T, N>::where;

	public:
		using Multichannel<T, N>::activate;
		using Multichannel<T, N>::deactivate;
		using Multichannel<T, N>::open;
		using Multichannel<T, N>::close;
		using Multichannel<T, N>::activity;

	public:
		~Oscbank()
		{
			delete phases;
			delete frequencies;
		}

		// initialize N DC-pass filters of a given order, reading and working with circular buffers
		Oscbank(double k = 2.0 / SR) : stiffness(relaxation(k))
		{
			phases = new ArrayCT;
			frequencies = new ArrayCT;

			phases->resize(N);
			frequencies->resize(N);

			phases->setOnes();
			frequencies->setOnes();
		}

		void freqmod(int index, T target)
		{
			if (0 <= index && index < N)
			{
				(*frequencies)(index) = std::complex<T>(cos(2 * PI * target / SR),
														sin(2 * PI * target / SR));
			}
		}

		void tick()
		{
			(*phases)(*where) *= (*frequencies)(*where);
			(*phases)(*where) /= (*phases)(*where).abs2();
		}

		const ArrayCT* operator()()
		{
			return phases;
		}

		std::complex<T> mixdown()
		{
			std::complex<T> value = 0;
			for (int i : *where)			
			{
				value += (*phases)(i);
			}

			return value;
		}

	private:
		ArrayCT* phases; // phases, as unit-norm complex numbers
		ArrayCT* frequencies; // frequencies, as unit-norm complex numbers
		
		T stiffness;
	};
}
