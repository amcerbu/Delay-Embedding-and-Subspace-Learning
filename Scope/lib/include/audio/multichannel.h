// multichannel.h
#pragma once

#include "includes.h"

using Eigen::Matrix;
using Eigen::Dynamic;
using Eigen::Map;
using Eigen::Array;

namespace soundmath
{
	// abstract base class
	// all multichannel objects have a protocol for activating and deactivating
	// channels as a way of optimizing update methods
	template <typename T, int N> class Multichannel
	{
	public:
		Multichannel()
		{
			where = new std::vector<int>();;
			active = new bool[N];
			memset(active, false, N * sizeof(bool));
		}

		~Multichannel()
		{
			delete where;
			delete [] active;
		}

	protected:
		using MatrixCT = Matrix<std::complex<T>, Dynamic, Dynamic>;
		using VectorCT = Matrix<std::complex<T>, Dynamic, 1>;
		using ArrayCT = Array<std::complex<T>, Dynamic, 1>;
		
		using MatrixT = Matrix<T, Dynamic, Dynamic>;
		using VectorT = Matrix<T, Dynamic, 1>;
		using ArrayT = Array<T, Dynamic, 1>;

		using ArrayB = Array<bool, Dynamic, 1>;

		bool* active; // which channels are active?
		std::vector<int>* where; // what are their indices?

		const bool* activity()
		{
			return active;
		}

		bool popout(int index)
		{
			if (where->empty())
			{
				return false;
			}
			if (index == where->back())
			{
				std::cout << "popped out the back.\n";
				where->pop_back();
				active[index] = false;
				return true;
			}

			return popout_search(index, 0, where->size() - 1);
		}

		bool insert(int index)
		{
			if (where->empty() || index < where->front())
			{
				where->insert(where->begin(), index);
				active[index] = true;
				return true;
			}
			else if (index > where->back())
			{
				where->push_back(index);
				active[index] = true;
				return true;
			}

			return insert_search(index, 0, where->size() - 1);
		}

		// activate oscillators indexed by indices
		void activate(const std::vector<int>& indices)
		{
			for (int i : indices)
				if (0 <= i && i < N && !active[i])
					insert(i);
		}

		// deactivate oscillators indexed by indices
		void deactivate(const std::vector<int>& indices)
		{
			for (int i : indices)
				if (0 <= i && i < N && active[i])
					popout(i);
		}

		// activate all oscillators
		void open()
		{
			std::vector<int> indices(N);
			for (int i = 0; i < N; i++)
				indices[i] = i;
			activate(indices);
		}

		// deactivate all oscillators
		void close()
		{
			std::vector<int> indices(N);
			for (int i = 0; i < N; i++)
				indices[i] = i;
			deactivate(indices);	
		}

	private:
		bool popout_search(int index, int left, int right)
		{
			bool located = (*where)[left] <= index && index < (*where)[right];
			if (!located)
				return false;

			if (right - left == 1)
			{
				if ((*where)[left] == index)
				{
					where->erase(where->begin() + left);
					active[index] = false;
					return true;
				}
				return false;
			}

			int pivot = (left + right) / 2;
			return popout_search(index, left, pivot) || popout_search(index, pivot, right);
		}

		bool insert_search(int index, int left, int right)
		{
			bool located = (*where)[left] < index && index < (*where)[right];
			if (!located)
				return false;

			if (right - left == 1)
			{	
				where->insert(where->begin() + right, index);
				active[index] = true;
				return true;
			}

			int pivot = (left + right) / 2;
			return insert_search(index, left, pivot) || insert_search(index, pivot, right);
		}

	};
}
