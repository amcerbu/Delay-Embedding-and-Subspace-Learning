// envelope.h
#pragma once

#include "includes.h"
#include "wave.h"

namespace soundmath
{
	// an n-shot Synth<T>
	template <typename T> class Envelope
	{
	public:
		Envelope() { }
		~Envelope()
		{
			for (int i = 0; i < stages; i++)
				delete shapes[i];
			delete [] shapes;
		}

		Envelope(const vector<function<T(double)>>& functions, Interp interp = Interp::cubic)
		{
			stages = functions.size();
			shapes = new Wave<T>*[stages];
			for (int i = 0; i < stages; i++)
				shapes[i] = new Wave<T>(functions[i], interp, 0, 1, false);

			stage = -1;
			rate = 0;
			active = false;
		}

		void trigger(T time, int stage = -1)
		{
			active = true;
			if (stage >= 0)
				this->stage = stage;
			else
				this->stage++;

			this->stage %= stages;
			phase = 0;
			this->rate = 1 / (time * SR);
		}

		void tick()
		{
			if (active)
			{
				phase += rate;
				if (phase > 1)
				{
					phase = 1;
					active = false;
					rate = 0;
				}
			}
		}

		T operator()()
		{
			return (*shapes[(stage >= 0) ? stage : 0])(phase);
		}

	private:
		Wave<T>** shapes;
		uint stages;
		int stage;
		T rate;
		T phase;

		bool active;
	};

	// Envelope<double> hann({[] (double phase) -> double { return 0.5 * (1 - cos(2 * PI * phase)); }});
}
