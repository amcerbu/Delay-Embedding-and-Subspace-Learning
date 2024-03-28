// audio.h
#pragma once

#include "includes.h"

namespace soundmath
{
	const int def_bsize = 16;
	inline int callback(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*);

	class Audio
	{
	public:
		int bsize;
		PaStream* stream;
		PaStreamParameters inParams, outParams;

		int (*process)(const float*, float*);

		Audio(int (*processor)(const float* in, float* out), int bsize = def_bsize) : bsize(bsize)
		{
			process = processor;
		}

		static PaError clean(PaError err)
		{
			if (err != paNoError)
			{
				Pa_Terminate();
				std::cout << "PortAudio error: " << Pa_GetErrorText(err);
			}
			return err;
		}

		static void initialize(bool report = false, int* def_in = NULL, int* def_out = NULL)
		{
			clean(Pa_Initialize());

			int numDevices = Pa_GetDeviceCount();
			const PaDeviceInfo *deviceInfo;
  
 			if (report)
 				std::cout << std::endl;

			for (int i = 0; i < numDevices; i++)
			{
				if (i == Pa_GetDefaultInputDevice())
				{
					if (def_in != NULL)
						*def_in = i;
					if (report)
						std::cout << " [def in]  ";
				}
				else if (i == Pa_GetDefaultOutputDevice())
				{
					if (def_out != NULL)
						*def_out = i;
					if (report)
					std::cout << " [def out] ";
				}
				else
					if (report)
						std::cout << "           ";

				deviceInfo = Pa_GetDeviceInfo(i);
				if (report)
				{
					std::cout << i << " \"" << deviceInfo->name << 
					   "\" (" << deviceInfo->maxInputChannels << " in, " << deviceInfo->maxOutputChannels << " out)" << std::endl;
				}
			}
			if (report)
				std::cout << std::endl;
		}

		void startup(int in = 1, int out = 2, bool report = true, int in_device_id = -1, int out_device_id = -1)
		{
			initialize();

			const PaDeviceInfo* deviceInfo;

			if (in_device_id < 0)
				inParams.device = Pa_GetDefaultInputDevice();
			else
				inParams.device = in_device_id;

			if (inParams.device == paNoDevice)
			{
				std::cout << "Error: No default input device.\n";
				shutdown();
			}

			deviceInfo = Pa_GetDeviceInfo(inParams.device);
			inParams.channelCount = in;
			inParams.sampleFormat = paFloat32;
			inParams.suggestedLatency = deviceInfo->defaultLowInputLatency;
			inParams.hostApiSpecificStreamInfo = NULL;

			if (report)
			{
				std::cout << "Input from " << deviceInfo->name << "\n";
				std::cout << "  " << deviceInfo->maxInputChannels << " channels available.\n";
			}

			if (out_device_id < 0)
				outParams.device = Pa_GetDefaultOutputDevice();
			else
				outParams.device = out_device_id;

			if (outParams.device == paNoDevice)
			{
				std::cout << "Error: No default output device.\n";
				shutdown();
			}
			deviceInfo = Pa_GetDeviceInfo(outParams.device);
			outParams.channelCount = out;
			outParams.sampleFormat = paFloat32;
			outParams.suggestedLatency = deviceInfo->defaultLowOutputLatency;
			outParams.hostApiSpecificStreamInfo = NULL;

			if (report)
			{
				std::cout << "Output to " << deviceInfo->name << "\n";
				std::cout << "  " << deviceInfo->maxOutputChannels << " channels available.\n";
			}

			clean(Pa_OpenStream(&stream, &inParams, &outParams, SR, bsize, 0, callback, this));
			clean(Pa_StartStream(stream));
		}

		void shutdown()
		{
			clean(Pa_StopStream(stream));
			clean(Pa_CloseStream(stream));
			clean(Pa_Terminate());
		}
	};

	int callback(const void* inputBuffer, void* outputBuffer, 
				  unsigned long framesPerBuffer, 
				  const PaStreamCallbackTimeInfo* timeInfo, 
				  PaStreamCallbackFlags statusFlags,
				  void* userData)
	{
		Audio* A = (Audio*)userData;
		A->process((const float*) inputBuffer, (float*) outputBuffer);
		return 0;
	}
}
