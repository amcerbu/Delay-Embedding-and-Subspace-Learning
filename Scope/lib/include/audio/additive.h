// additive.h
#pragma once

#include "includes.h"
#include "minimizer.h"
#include "wave.h"
#include "oscillator.h"

namespace soundmath
{
	template <typename T> class Additive : public Minimizer<T>
	{
	public:
		using Minimizer<T>::voices, Minimizer<T>::overtones, Minimizer<T>::decay, Minimizer<T>::harmonicity, 
			  Minimizer<T>::particles, Minimizer<T>::active, Minimizer<T>::pitches;
		
		Additive() { }
		~Additive()
		{
			delete [] oscillators;
			delete [] amplitudes;
		}

		Additive(Wave<T>* waveform, uint voices, uint overtones, T decay, T harmonicity = 1.0, T k = 0.1) : 
			Minimizer<T>(voices, overtones, decay, harmonicity), waveform(waveform), attack(relaxation(k))
		{
			normalization = decay != 1 ? (1 - pow(decay, overtones)) / (1 - decay) : overtones;
			
			oscillators = new Oscillator<T>[voices * overtones];
			amplitudes = new T[voices];

			memset(amplitudes, 0, voices * sizeof(T));

			for (int i = 0; i < voices * overtones; i++)
				oscillators[i] = Oscillator<T>();
		}

		void tick()
		{
			for (int i = 0; i < voices; i++)
				amplitudes[i] = (1 - attack) * active[i] + attack * amplitudes[i];

			// update oscillator frequencies; tick oscillators
			for (int i = 0; i < voices; i++)
				if (active[i] || amplitudes[i])
					for (int j = 0; j < overtones; j++)
					{
						oscillators[i * overtones + j].freqmod(mtof(particles[i * overtones + j]()));
						oscillators[i * overtones + j].tick();
					}
		}

		T operator()()
		{
			T sample = 0;
			for (int i = 0; i < voices; i++)
				if (amplitudes[i])
					for (int j = 0; j < overtones; j++)
						sample += amplitudes[i] * pow(decay, j) * (*waveform)(oscillators[i * overtones + j]()) / (voices * normalization);

			return sample;		
		}

	private:
		Wave<T>* waveform;
		Oscillator<T>* oscillators;
		T* amplitudes;
		T attack;
		T normalization;
	};
}