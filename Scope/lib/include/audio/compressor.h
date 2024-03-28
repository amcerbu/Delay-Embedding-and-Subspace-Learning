// compressor.h
#pragma once

#include "includes.h"

namespace soundmath
{
	template <typename T> class Compressor
	{
	public:
		Compressor() { }
		~Compressor() { }

		Compressor(T threshold, T ratio, T attack, T release, T knee = 0, bool makeup = false)
		{
			this->threshold = threshold;
			this->correction = atodb(ratio);

			up_stiffness = relaxation(attack);
			down_stiffness = relaxation(release);
			amplitude = 0;
		}

		void tick()
		{
			if (amplitude < threshold)
				gain = 0;
			else if (amplitude > threshold)
				gain = correction;

			if (gain > target_gain)
				gain = target_gain * (1 - down_stiffness) + gain * down_stiffness;
			else if (gain < target_gain)
				gain = target_gain * (1 - up_stiffness) + gain * up_stiffness;
		}

		T operator()(T sample)
		{
			amplitude = abs(sample);
			return dbtoa(gain) * sample;
		}

	private:
		T gain;
		T target_gain;
		T up_stiffness;
		T down_stiffness;
		T threshold;
		T correction;
		T amplitude;
	};
}
