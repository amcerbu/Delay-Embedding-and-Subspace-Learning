// mixer.h
#pragma once

#include "includes.h"
#include "multichannel.h"

namespace soundmath
{
	template <typename T, int N> class Mixer : public Multichannel<T, N>
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
		Mixer() { }
		~Mixer() { }

		// mixdown the input
		const T operator()(const ArrayCT* input)
		{
			return (*input).sum().real();
		}

		const T operator()(const ArrayT* input)
		{
			return (*input).sum();
		}
	};
}
