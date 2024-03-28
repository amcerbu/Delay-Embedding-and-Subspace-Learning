// subtractive.h
#pragma once

#include "includes.h"
#include "minimizer.h"
#include "filterbank.h"
#include "filter.h"
#include "filterbank.h"

namespace soundmath
{
	template <typename T> class Subtractive : public Minimizer<T>
	{
	public:
		using Minimizer<T>::voices, Minimizer<T>::overtones, Minimizer<T>::decay, Minimizer<T>::harmonicity,
			  Minimizer<T>::particles, Minimizer<T>::active, Minimizer<T>::pitches;

		Subtractive() { }
		~Subtractive()
		{
			delete [] amplitudes;

			for (int i = 0; i < voices * overtones; i++)
				delete filters[i];
			delete [] filters;
		}

		Subtractive(uint voices, uint overtones, T decay, T resonance = 0.99999, T harmonicity = 1.0, T k = 0.1) : 
			Minimizer<T>(voices, overtones, decay, harmonicity), attack(relaxation(k)), resonance(resonance)
		{
			normalization = decay != 1 ? (1 - pow(decay, overtones)) / (1 - decay) : overtones;
			amplitudes = new T[voices];
			memset(amplitudes, 0, voices * sizeof(T));

			filters = new Filter<T>*[voices * overtones];
			for (int i = 0; i < voices * overtones; i++)
				filters[i] = new Filter<T>({1,0,0},{0});
		}

		void tick()
		{
			for (int i = 0; i < voices; i++)
				amplitudes[i] = (1 - attack) * active[i] + attack * amplitudes[i];

			// update and tick filters
			for (int i = 0; i < voices; i++)
				if (amplitudes[i])
					for (int j = 0; j < overtones; j++)
					{
						T frequency = mtof(particles[i * overtones + j]());
						while (frequency > SR / 2)
							frequency /= 2;

						filters[i * overtones + j]->resonant(frequency, resonance);
						filters[i * overtones + j]->tick();
					}
		}

		T operator()(T sample)
		{
			T out = 0;
			for (int i = 0; i < voices; i++)
				if (amplitudes[i])
					for (int j = 0; j < overtones; j++)
						// out += amplitudes[i] * softclip(pow(decay, j) * (*filters[i * overtones + j])(sample)) / (voices * normalization);
						out += amplitudes[i] * pow(decay, j) * ((*filters[i * overtones + j])(sample)) / (voices * normalization);

			return out;
		}

	private:
		Filter<T>** filters;
		T* amplitudes;
		T normalization;
		T attack;

		T resonance;

		// clips (-infty, infty) to (-1, 1); linear in (-width, width)
		T softclip(T sample, T width = 0.99)
		{
			if (abs(sample) < width)
				return sample;

			int sign = sgn(sample);
			T gap = sample - sign * width;
			return sign * width + (1 - width) * 2.0 / PI * atan(PI * gap / (2 * (1 - width)));
		}

	};
}
