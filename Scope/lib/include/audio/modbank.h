// modbank.h
#pragma once

#include "includes.h"

namespace soundmath
{
	// modulates a pair of multichannel signals
	template <typename T, int N> class Modbank : public Multichannel<T, N>
	{
	private:
		using typename Multichannel<T, N>::MatrixCT;
		using typename Multichannel<T, N>::MatrixT;
		using typename Multichannel<T, N>::VectorT;
		using typename Multichannel<T, N>::ArrayCT;
		using typename Multichannel<T, N>::ArrayT;
		using Multichannel<T, N>::active;
		using Multichannel<T, N>::where;

	public:
		using Multichannel<T, N>::activate;
		using Multichannel<T, N>::deactivate;
		using Multichannel<T, N>::open;
		using Multichannel<T, N>::close;
		using Multichannel<T, N>::activity;

	public:
		Modbank()
		{
			out = new ArrayCT;
			out->resize(N);
			out->setZero();
		}

		~Modbank()
		{
			delete out;
		}

		// modulate signal by carrier
		const ArrayCT* operator()(const ArrayCT* signal, const ArrayCT* carrier)
		{
			*out = (*signal) * (*carrier);
			return out;
		}

		const ArrayCT* operator()(std::complex<T> signal, const ArrayCT* carrier)
		{
			*out = signal * (*carrier);
			return out;
		}

		const ArrayCT* operator()(T signal, const ArrayCT* carrier)
		{
			*out = signal * (*carrier);
			return out;
		}

	private:
		ArrayCT* out;
	};
}
