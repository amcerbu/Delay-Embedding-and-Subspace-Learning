// signal.h
#pragma once
#ifndef SIGNAL_H

#include "includes.h"
#include "multichannel.h"

using Eigen::Matrix;
using Eigen::Map;
using Eigen::Array;

namespace soundmath
{
	// bank of stickers -- nonlinear low-pass filters
	// N ins, N outs
	template <typename T, int N> class Signal : public Multichannel<T, N>
	{
	private:
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
		~Signal()
		{
			delete out;
		}

		// initialize N DC-pass filters of a given order
		// then filter using response. the idea of response is to 
		// provide different radii for positive and negative changes
		Signal()
		{
			out = new ArrayT;
			out->resize(N);
			out->setZero();
		}

		// get the result of filters applied to a sample
		const ArrayT* operator()(const std::vector<T>& input)
		{
			for (int i = 0; i < N; i++)
			{
				(*out)(i) = input[i];
			}

			return out;
		}

	private:
		ArrayT* out;
	};
}

#define SIGNAL_H
#endif