// stickbank.h
#pragma once

#include "includes.h"
#include "multichannel.h"

using Eigen::Matrix;
using Eigen::Map;
using Eigen::Array;

namespace soundmath
{
	// bank of stickers -- low-pass filters
	// N ins, N outs
	template <typename T, int N> class Stickbank : public Multichannel<T, N>
	{
	private:
		using typename Multichannel<T, N>::MatrixCT;
		using typename Multichannel<T, N>::VectorCT;
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
		~Stickbank()
		{
			// delete forward;
			delete back;
			delete inputs;
			delete outputs;
			delete out;
		}

		// initialize N DC-pass filters of a given order
		// then filter using response. the idea of response is to 
		// provide different radii for positive and negative changes
		Stickbank(int order, T rad) : rad(rad)
		{
			this->order = order = std::max(1, order);
			const std::vector<double> zeros(order, rad);
			std::vector<T> coeffs = coefficients(zeros);

			// forward = new VectorT;
			back = new VectorCT;
			inputs = new MatrixCT;
			outputs = new MatrixCT;
			out = new ArrayCT;

			back->resize(order);
			for (int i = 0; i < order; i++)
			{
				(*back)(i) = coeffs[i];
			}
			inputs->resize(N, 2 * (order + 1)); // allows for circular buffering without modulo
			outputs->resize(N, 2 * (order + 1)); // N circular buffers of output
			out->resize(N);
			
			// forward->setZero();
			inputs->setZero();
			outputs->setZero();
			out->setZero();
		}

		void rmod(T rad)
		{
			this->rad = rad;
			this->order = order = std::max(1, order);
			const std::vector<double> zeros(order, rad);
			std::vector<T> coeffs = coefficients(zeros);
			for (int i = 0; i < order; i++)
			{
				(*back)(i) = coeffs[i];
			}
		}

		// get the result of filters applied to a sample
		const ArrayCT* operator()(const ArrayCT* input)
		{
			if (!computed)
				compute(input);

			*out = outputs->col(origin).array();
			return out;
		}

		const ArrayCT* operator()(const ArrayT* input)
		{
			if (!computed)
				compute(input);

			*out = outputs->col(origin).array();
			return out;
		}

		const ArrayCT* operator()()
		{
			if (computed)
				return out;

			return NULL;
		}

		// timestep
		void tick()
		{
			origin--;
			if (origin < 0)
				origin += order + 1;
			computed = false;
		}

		// writes to a boolean array passed by reference
		void poll(bool* voices, T thresh, T proportion = 0.8)
		{
			T total = 0;
			for (int i = 0 ; i < N; i++)
				total += std::norm((*out)(i));

			T square = thresh * thresh;
			if (total < square)
			{
				for (int i = 0; i < N; i++)
					voices[i] = false;
			}
			else
			{
				for (int i = 0; i < N; i++)
				{
					if (std::norm((*out)(i)) > total * proportion)
					{
						voices[i] = true;
					}
					else
					{
						voices[i] = false;
					}
				}
			}
		}

	private:
		// VectorT* forward;
		VectorCT* back;
		MatrixCT* inputs;
		MatrixCT* outputs;

		ArrayCT* out;

		T rad;

		int order;
		int origin = 0;
		bool computed = false;

		T positive(T in)
		{
			return (in > 0 ? in : 0);
		}

		T negative(T in)
		{
			return (in < 0 ? in : 0);
		}

		void compute(const ArrayT* input)
		{
			ArrayCT casted = input->template cast<std::complex<T>>();
			compute(&casted);
		}

		void compute(const ArrayCT* input)
		{
			inputs->col(origin) = *input;
			inputs->col(origin + (order + 1)) = *input;

			// std::cout << "back: " << back->rows() << " x " << back->cols() << std::endl;
			// std::cout << "outputs->block(0, origin + 1, N, order): " << outputs->block(0, origin + 1, N, order).rows() << " x " << outputs->block(0, origin + 1, N, order).cols() << std::endl;

			MatrixCT block = outputs->block(0, origin + 1, N, order);

			VectorCT temp = pow((1 + rad), order) * input->matrix() - block * (*back);
			
			outputs->col(origin) = temp;
			outputs->col(origin + (order + 1)) = temp;

			computed = true;
		}

		// get coefficients of a monic polynomial given its roots
		static std::vector<T> coefficients(const std::vector<T>& zeros, int start = 0)
		{
			int degree = zeros.size() - start;
			if (degree == 0)
				return {0};
			if (degree == 1)
				return {zeros[start], 1};

			std::vector<T> total(degree + 1, 0);
			T first = zeros[start];

			std::vector<T> shifted(coefficients(zeros, start + 1));

			for (int i = 0; i < degree; i++)
			{
				total[i] += first * shifted[i];
				total[i + 1] += shifted[i];
			}

			return total;
		}

	};
}
