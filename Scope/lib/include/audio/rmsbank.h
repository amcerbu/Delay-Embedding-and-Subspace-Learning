// rmsbank.h
#pragma once

#include "includes.h"
#include "multichannel.h"

using Eigen::Matrix;
using Eigen::Map;
using Eigen::Array;

namespace soundmath
{
	// bank of stickers -- nonlinear low-pass filters
	// N ins, N outs
	template <typename T, int N> class RMSbank : public Multichannel<T, N>
	{
	private:
		using typename Multichannel<T, N>::MatrixT;
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
		RMSbank(uint width = SR / 20) : width(width), origin(0)
		{
			inputs = new MatrixT;
			inputs->resize(N, 2 * (width + 1)); // allows for circular buffering without modulo
			inputs->setZero();

			out = new ArrayT;
			out->resize(N);
			out->setZero();

			lastout = new ArrayT;
			lastout->resize(N);
			lastout->setZero();
		}

		~RMSbank()
		{
			delete inputs;
			delete out;
			delete lastout;
		}

		const ArrayT* operator()(const ArrayCT* input)
		{
			if (!computed)
			{
				inputs->col(origin) = input->abs2();
				inputs->col(origin + (width + 1)) = input->abs2();
				*out = (inputs->col(origin) - inputs->col(origin + width) + lastout->matrix()).array();
				*lastout = *out;
				computed = true;
			}
			*out = ((*out) / width).sqrt();
			return out;
		}

		void tick()
		{
			origin--;
			if (origin < 0)
				origin += width + 1;
			computed = false;
		}

	private:
		int width;
		int origin;
		bool computed = false;

		MatrixT* inputs;
		ArrayT* out;
		ArrayT* lastout;
	};
}
