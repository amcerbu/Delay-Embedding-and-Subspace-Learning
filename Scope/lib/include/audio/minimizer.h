// minimizer.h
#pragma once

#include "includes.h"
#include "particle.h"

namespace soundmath
{
	// receives requests for particle systems; runs physics on them
	template <typename T> class Minimizer
	{
	public:
		Minimizer() { }
		~Minimizer()
		{
			delete [] springs;
			delete [] gravities;
			delete [] guides;
			delete [] particles;		
			delete [] active;
			delete [] pitches;
		}

		Minimizer(uint voices, uint overtones, T decay, T harmonicity) : 
			voices(voices), overtones(overtones), decay(decay), harmonicity(harmonicity)
		{
			particles = new Particle[voices * overtones];
			guides = new Particle[voices];
			springs = new Spring[voices * overtones];
			gravities = new Gravity[voices * (voices - 1) * overtones * overtones / 2];

			active = new T[voices];
			pitches = new T[voices];

			memset(active, 0, voices * sizeof(T));

			for (int i = 0; i < voices; i++)
			{
				springs[i * overtones].bind(&guides[i], &particles[i * overtones]);
				springs[i * overtones].strength(0.1 * FORCE);
				// springs[i * overtones].strength(0 * FORCE);
				for (int j = 1; j < overtones; j++)
				{
					springs[i * overtones + j].bind(&particles[i * overtones + j - 1], &particles[i * overtones + j]);
					springs[i * overtones + j].strength(1 * FORCE);
				}
			}

			int count = 0;
			for (int i = 0; i < voices; i++)
				for (int j = i + 1; j < voices; j++)
					for (int k = 0; k < overtones; k++)
						for (int m = 0; m < overtones; m++)
						{
							gravities[count].bind(&particles[i * overtones + k], &particles[j * overtones + m]);
							gravities[count].strength(1 * FORCE);
							count++;
						}
		}

		void physics()
		{
			// zero the forces on particles
			for (int i = 0; i < voices; i++)
				if (active[i])
				{
					guides[i].prepare();
					guides[i].mass = active[i];
					for (int j = 0; j < overtones; j++)
					{
						particles[i * overtones + j].prepare();
						particles[i * overtones + j].mass = active[i];
					}
				}

			// apply spring forces
			for (int i = 0; i < voices; i++)
				if (active[i])
					for (int j = 0; j < overtones; j++)
						springs[i * overtones + j].tick();

			// apply forces from gravity
			int count = 0;
			for (int i = 0; i < voices; i++)
			{
				if (active[i]) // if one voice is active
					for (int j = i + 1; j < voices; j++)
					{
						if (active[j]) // as is another
							for (int k = 0; k < overtones; k++)
								for (int m = 0; m < overtones; m++)
								{
									gravities[count].tick(); // allow their overtones to interact
									count++;
								}
						else
							count += overtones * overtones;
					}
				else
					count += overtones * overtones * (voices - i - 1);
			}

			// tick the particles
			for (int i = 0; i < voices; i++)
				if (active[i])
					for (int j = 0; j < overtones; j++)
						particles[i * overtones + j].tick();
		}

		// request a new note, return allocated voice number
		int request(T fundamental, T amplitude = 0)
		{
			int voice = -1;
			bool stolen = false;
			for (int i = 0; i < voices; i++)
			{
				if (!active[i])
				{
					voice = i;
					break;
				}
			}

			if (voice < 0)
			{
				stolen = true;
				T pitch = ftom(fundamental);
				T distance = 0;
				int nearest = -1;

				for (int i = 0; i < voices; i++)
				{
					T offset = pitch - guides[i].position;
					offset *= offset;
					if (nearest < 0 || offset < distance)
					{
						nearest = i;
						distance = offset;
					}
				}

				voice = nearest;
			}

			T frequency = fundamental;
			T previous;
			active[voice] = amplitude;
			guides[voice].initialize(1, ftom(fundamental)); // mass depends on decay / amplitude?
			for (int j = 0; j < overtones; j++)
			{
				previous = frequency;
				frequency = fundamental * (1 + pow((double)j / (overtones - 1), harmonicity) * (overtones - 1));

				// if (!stolen)
					particles[voice * overtones + j].initialize(1, ftom(frequency));
				springs[voice * overtones + j].target(ftom(frequency) - ftom(previous));
			}

			return voice;
		}

		// release a given voice, or all voices
		void release(int voice)
		{
			if (voice >= 0)
			{
				active[voice] = 0;
				return;
			}

			for (int i = 0; i < voices; i++)
			{
				active[i] = 0;
			}
		}

		void makenote(T pitch, T amplitude)
		{
			int voice = request(mtof(pitch), amplitude);
			if (voice >= 0)
				pitches[voice] = pitch;
		}

		void endnote(T pitch)
		{
			for (int j = 0; j < voices; j++)
				if (pitches[j] == pitch)
					release(j);
		}

	protected:
		uint voices;
		uint overtones;
		T decay;
		T harmonicity;

		Particle* particles; // subclasses can access particle positions
		T* active; // and turn voices on / off
		T* pitches;

	private:
		Particle* guides;
		Spring* springs;
		Gravity* gravities;
	};
}
