// latchbank.h
#pragma once

#include "includes.h"
#include "multichannel.h"
#include "rmsbank.h"

using Eigen::Matrix;
using Eigen::Map;
using Eigen::Array;

namespace soundmath
{
	// bank of stickers -- nonlinear low-pass filters
	// N ins, N outs
	template <typename T, int N> class Latchbank : public Multichannel<T, N>
	{
	private:
		using typename Multichannel<T, N>::ArrayT;
		using typename Multichannel<T, N>::ArrayCT;
		using typename Multichannel<T, N>::ArrayB;

		using Multichannel<T, N>::active;
		using Multichannel<T, N>::where;

	public:
		using Multichannel<T, N>::activate;
		using Multichannel<T, N>::deactivate;
		using Multichannel<T, N>::open;
		using Multichannel<T, N>::close;
		using Multichannel<T, N>::activity;

	public:
		Latchbank(T thresh, T ratio = 0.2) : thresh(thresh), ratio(ratio)
		{
			armed = new ArrayB;
			engaged = new ArrayB;
			out = new ArrayT;
			outC = new ArrayCT;

			armed->resize(N);
			engaged->resize(N);
			out->resize(N);
			outC->resize(N);

			armed->setZero();
			engaged->setZero();
			out->setZero();
			outC->setZero();
		}

		~Latchbank()
		{
			delete armed;
			delete engaged;
			delete out;
			delete outC;
		}

		const ArrayT* operator()(const ArrayT* input)
		{
			*armed = (*armed) || (*input) < (thresh * ratio);

			ArrayB temp1 = (*engaged) && (*input < thresh * ratio);
			ArrayB temp2 = !(*engaged) && (*input > thresh * (1 - ratio)) && (*armed);

			// if temp1, set engaged and armed to false
			*engaged = (*engaged) && !temp1;
			*armed = (*armed) && !temp1;

			// if temp2, set engaged to true
			*engaged = (*engaged) || temp2; 

			*out = (*input) * (engaged->template cast<T>());

			return out;
		}

		const ArrayCT* operator()(RMSbank<T, N>* rms, const ArrayCT* signal)
		{
			ArrayT input = *(*rms)(signal);

			*armed = (*armed) || (input) < (thresh * ratio);

			ArrayB temp1 = (*engaged) && (input < thresh * ratio);
			ArrayB temp2 = !(*engaged) && (input > thresh * (1 - ratio)) && (*armed);

			// if temp1, set engaged and armed to false
			*engaged = (*engaged) && !temp1;
			*armed = (*armed) && !temp1;

			// if temp2, set engaged to true
			*engaged = (*engaged) || temp2; 

			*outC = (*signal) * (engaged->template cast<T>());

			return outC;
		}

	private:
		ArrayB* armed;
		ArrayB* engaged;
		ArrayT* out;
		ArrayCT* outC;

		T thresh;
		T ratio;
	};
}

/*
latch logic

input
thresh
ratio
active
armed


if input < thresh * ratio
	armed = true

if active
	if input < thresh * (1 - ratio)
		active = false
		armed = false
else
	if input > thresh * (1 - ratio) and armed
		active = true

if A set B to false. if not A, do nothing

B = !(B or A)

B = (B and !A) or (false and A) -- if A, then B = false, if not A, then B = B.

*/