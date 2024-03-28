// slidebank.h
#pragma once

#include "includes.h"
#include "multichannel.h"
#include <Eigen/SparseCore>
#include <iomanip>

using Eigen::Matrix;
using Eigen::Map;
using Eigen::Array;
using Eigen::SparseMatrix;
using Eigen::seqN;
using Eigen::seq;

namespace soundmath
{
	// bank of (conceivably high-order) lowpass filters
	// with poles optimized for particular decay times
	// N ins, N outs
	template <typename T, int N> class Slidebank : public Multichannel<T, N>
	{
	private:
		using typename Multichannel<T, N>::MatrixCT;
		using typename Multichannel<T, N>::VectorCT;
		using typename Multichannel<T, N>::ArrayCT;
		using typename Multichannel<T, N>::ArrayT;
		typedef SparseMatrix<std::complex<T>> SpMatrixCT;
		typedef Eigen::Triplet<std::complex<T>> Triplet;
		using Multichannel<T, N>::active;
		using Multichannel<T, N>::where;

	public:
		using Multichannel<T, N>::activate;
		using Multichannel<T, N>::deactivate;
		using Multichannel<T, N>::open;
		using Multichannel<T, N>::close;
		using Multichannel<T, N>::activity;

	public:
		Slidebank() { }

		~Slidebank()
		{
			delete [] radii;
			delete back;
			delete inputs;
			delete out;
		}

		// initialize N DC-pass filters of a given order
		// then filter using response. the idea of response is to 
		// provide different radii for positive and negative changes
		Slidebank(int order, const std::vector<std::complex<T>>& radii)
		{
			setup(order, radii);
		}

		void setup(int order, const std::vector<std::complex<T>>& radii)
		{
			this->order = order = std::max(1, order);

			this->radii = new std::complex<T>[N]; // one radius per filter
			for (int i = 0; i < N; i++)
				this->radii[i] = radii[i];

			back = new SpMatrixCT(order * N, (order + 1) * N); // feedback is a sparse matrix
			inputs = new VectorCT((order + 1) * N); // inputs is unrolled vector; stores history
			out = new ArrayCT(N);

			std::vector<Triplet> coefficients;

			for (int j = 0; j < N; j++)
			{
				coefficients.push_back(Triplet(j * order, j, std::complex<T>(1,0) - radii[j]));
				coefficients.push_back(Triplet(j * order, N + j * order, radii[j]));
			}
			for (int i = 0; i < N; i++)
			{
				for (int j = 1; j < order; j++)
				{
					coefficients.push_back(Triplet(j + i * order, N + j - 1 + i * order, std::complex<T>(1,0) - radii[i]));
					coefficients.push_back(Triplet(j + i * order, N + j + i * order, radii[i]));
				}
			}
			back->setFromTriplets(coefficients.begin(), coefficients.end());

			// print();

			inputs->setZero();
			out->setZero();
		}

		// get the result of filters applied to a sample
		const ArrayCT* operator()(const ArrayCT* input)
		{
			if (!computed)
				compute(input);

			return out;
		}

		// get the result of filters applied to a sample
		const ArrayCT* operator()(const ArrayT* input)
		{
			if (!computed)
				compute(input);

			return out;
		}

		const ArrayCT* operator()()
		{
			if (computed)
				return out;

			return NULL;
		}

		void tick()
		{
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
						voices[i] = true;
					else
						voices[i] = false;
				}
			}
		}

		void print()
		{
			for (int i = 0; i < order * N; i++)
			{
				for (int j = 0; j < (order + 1) * N; j++)
				{
					std::cout << "\t" << std::real(back->coeffRef(i,j)) << " ";
				}
				std::cout << std::endl;
			}
		}

	private:
		int order;
		std::complex<T>* radii;

		SpMatrixCT* back;
		VectorCT* inputs;
		ArrayCT* out;

		bool computed = false;

		void compute(const ArrayT* input)
		{
			ArrayCT casted = input->template cast<std::complex<T>>();
			compute(&casted);
		}

		void compute(const ArrayCT* input)
		{
			(*inputs)(seqN(0, N)) = input->matrix();

			VectorCT temp = (*back) * (*inputs);
			(*inputs)(seq(N, Eigen::last)) = temp;

			*out = (*inputs)(seqN(N + order - 1, N, order));
			computed = true;
		}
	};
}
