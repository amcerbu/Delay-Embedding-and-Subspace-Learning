// sinusoids.h
#pragma once

#include "includes.h"
#include "synth.h"

namespace soundmath
{
	// a bank of synthesizers, with optional harmonicity.
	template <typename T> class Sinusoids
	{
	public:
		Sinusoids() { }
		~Sinusoids() { }

		Sinusoids(Wave<T>* form, T fundamental, uint overtones, T decay, T harmonicity = 1, T k = 2.0 / SR)
		{
			this->fundamental = this->target_fundamental = fundamental;
			this->harmonicity = this->target_harmonicity = harmonicity;
			this->overtones = overtones;
			this->decay = this->target_decay = decay;

			for (int i = 0; i < overtones; i++)
			{
				Synth<T> synth(form, fundamental * pow(i + 1, harmonicity));
				synths.push_back(synth);
			}

			normalization = decay != 1 ? (1 - pow(decay, overtones)) / (1 - decay) : overtones;

			stiffness = relaxation(k);
		}

		T operator()()
		{
			T sample = 0;
			for (int i = 0; i < overtones; i++)
			{
				sample += pow(decay, i) * synths[i]() / normalization;
			}
			return (T)sample;
		}

		void tick()
		{
			fundamental = target_fundamental * (1 - stiffness) + fundamental * stiffness;
			decay = target_decay * (1 - stiffness) + decay * stiffness;
			harmonicity = target_harmonicity * (1 - stiffness) + harmonicity * stiffness;

			for (int i = 0; i < overtones; i++)
			{
				synths[i].freqmod(fundamental * pow(i + 1, harmonicity));
				synths[i].tick();
			}

			normalization = decay != 1 ? (1 - pow(decay, overtones)) / (1 - decay) : overtones;
		}

		void decaymod(T target)
		{ target_decay = target; }

		void harmmod(T target)
		{ target_harmonicity = target; }

		void fundmod(T target)
		{ target_fundamental = target; }

	private:
		uint overtones;
		T fundamental;
		T target_fundamental;
		T decay;
		T target_decay;
		T normalization;
		T harmonicity;
		T target_harmonicity;
		T stiffness;
		vector<Synth<T>> synths;
	};
}
