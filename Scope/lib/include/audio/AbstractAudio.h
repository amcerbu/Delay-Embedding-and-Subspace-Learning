// AbstractAudio.h
#pragma once

#include "includes.h"
using namespace soundmath;

// abstract base class
class AbstractAudio
{
public:

	AbstractAudio(int bsize = def_bsize, int SR = def_SR);
	~AbstractAudio();
	
	static void initialize(bool report = false, int* def_in = NULL, int* def_out = NULL);
	virtual void args(int argc, char *argv[]);
	virtual void startup(bool report = true);
	virtual void startup(int in_chans, int out_chans, bool report = true, int in_device_id = -1, int out_device_id = -1);

	virtual int process(const float* in, float* out, unsigned long frames);
	
	void shutdown();

	static PaError clean(PaError err);

private:
	static const int def_bsize = 16;
	static const int def_SR = 48000;

protected:
	int bsize;
	int SR;

	bool command_line = false;
	int in_device;
	int in_chans;
	int in_channel;
	int out_device;
	int out_chans;

	PaStream* stream;
	PaStreamParameters inParams, outParams;
	bool running = false;
};