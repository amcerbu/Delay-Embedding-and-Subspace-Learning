// distbank.h
#pragma once

#include "includes.h"

namespace soundmath
{
	// modulates a pair of multichannel signals
	template <typename T, int N> class Distbank : public Multichannel<T, N>
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
		Distbank(std::complex<T> (*distortion)(std::complex<T> in))
		{
			this->distortion = distortion;
			out = new ArrayCT;
			out->resize(N);
			out->setZero();
		}

		~Distbank()
		{
			delete out;
		}

		// distort input
		const ArrayCT* operator()(const ArrayCT* input)
		{
			*out = input->unaryExpr(distortion);
			return out;
		}

	private:
		std::complex<T> (*distortion)(std::complex<T>);
		ArrayCT* out;
	};
}
