// AbstractAudio.cpp
#include "AbstractAudio.h"
#include "argparse.h"

using namespace soundmath;
inline int callback(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*);

AbstractAudio::AbstractAudio(int bsize, int SR) : bsize(bsize), SR(SR)
{
}

AbstractAudio::~AbstractAudio()
{
	shutdown();
}

void AbstractAudio::initialize(bool report, int* def_in, int* def_out)
{
	clean(Pa_Initialize());

	int numDevices = Pa_GetDeviceCount();
	const PaDeviceInfo *deviceInfo;

		if (report)
			std::cout << std::endl;

	for (int i = 0; i < numDevices; i++)
	{
		if (i == Pa_GetDefaultInputDevice() && i == Pa_GetDefaultOutputDevice())
		{
			if (def_in != NULL)
				*def_in = i;
			if (def_out != NULL)
				*def_out = i;
			if (report)
				std::cout << " [def i/o] ";
		}
		else if (i == Pa_GetDefaultInputDevice())
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

void AbstractAudio::args(int argc, char *argv[])
{
	argparse::ArgumentParser program("");

	int def_in = 0;
	int def_out = 0;
	initialize(false, &def_in, &def_out);

	program.add_argument("-i", "--input")
		.default_value<int>((int)def_in)
		.required()
		.scan<'i', int>()
		.help("device id for audio in");

	program.add_argument("-if", "--in_framesize")
		.default_value<int>(2)
		.required()
		.scan<'i', int>()
		.help("channels per frame of input");

	program.add_argument("-c", "--channel")
		.default_value<int>(0)
		.required()
		.scan<'i', int>()
		.help("input channel selector (in [0, if - 1])");

	program.add_argument("-o", "--output")
		.default_value<int>((int)def_out)
		.required()
		.scan<'i', int>()
		.help("device id for audio out");

	program.add_argument("-of", "--out_framesize")
		.default_value<int>(2)
		.required()
		.scan<'i', int>()
		.help("channels per frame of output");

	program.add_argument("-d", "--devices")
		.help("list audio device names and exits")
		.default_value(false)
		.implicit_value(true);

	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err)
	{
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(1);
	}

	in_device = program.get<int>("-i");
	in_chans = program.get<int>("-if");
	in_channel = program.get<int>("-c");
	out_device = program.get<int>("-o");
	out_chans = program.get<int>("-of");

	initialize(program.is_used("-d"));

	if (program.is_used("-d"))
	{
		std::exit(1);
	}
}

void AbstractAudio::startup(bool report)
{
	startup(in_chans, out_chans, report, in_device, out_device);
}

void AbstractAudio::startup(int in_chans, int out_chans, bool report, int in_device_id, int out_device_id)
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
	inParams.channelCount = in_chans;
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
	outParams.channelCount = out_chans;
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
	running = true;
}

int AbstractAudio::process(const float* in, float* out, unsigned long frames)
{
	return 0;
}

void AbstractAudio::shutdown()
{
	if (running)
	{
		clean(Pa_StopStream(stream));
		clean(Pa_CloseStream(stream));
		clean(Pa_Terminate());
		running = false;
	}
}

PaError AbstractAudio::clean(PaError err)
{
	if (err != paNoError)
	{
		Pa_Terminate();
		std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
		// std::exit(1);
	}
	return err;
}

int callback(const void* inputBuffer, void* outputBuffer, 
			  unsigned long framesPerBuffer, 
			  const PaStreamCallbackTimeInfo* timeInfo, 
			  PaStreamCallbackFlags statusFlags,
			  void* userData)
{
	AbstractAudio* A = (AbstractAudio*)userData;
	A->process((const float*) inputBuffer, (float*) outputBuffer, framesPerBuffer);
	return 0;
}