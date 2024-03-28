// midi.h
#pragma once
#include "includes.h"

namespace soundmath
{
	enum Status
	{
		note_on = 144,
		note_off = 128,
		aftertouch = 160,
		pitchwheel = 224
	};

	class MidiIn
	{
	public:
		std::vector<unsigned char> message;
		int bytes;
		double stamp;

		int status;

	private:
		RtMidiIn *midi = 0;
		std::vector<std::string> names;
		int port = -1;

	public:
		MidiIn()
		{

		}

		void startup()
		{
			try { midi = new RtMidiIn(); }
			catch (RtMidiError &error)
			{
				error.printMessage();
				exit(EXIT_FAILURE);
			}
		}

		void shutdown()
		{
			delete midi;
		}

		void getports(bool verbose = false)
		{
			names.clear();
			int n = midi->getPortCount();
			std::string name;
			
			for (int i = 0; i < n; i++)
			{
				try { name = midi->getPortName(i); }
				catch (RtMidiError &error)
				{
					error.printMessage();
					continue;
				}
				if (verbose)
					std::cout << "  input port #" << i+1 << ": " << name << '\n';
				names.push_back(name);
			}
		}

		int open(int number)
		{
			try
			{
				midi->openPort(number);
			}
			catch (RtMidiError &error)
			{
				error.printMessage();
				return 1;
			}
			port = number;
			return 0;
		}


		int open(std::string name)
		{
			for (int i = 0; i < names.size(); i++)
				if (names[i] == name)
					return open(i);
			return 1;
		}

		void ignore(bool sysex = true, bool time = true, bool sense = true)
		{
			midi->ignoreTypes(sysex, time, sense);
		}

		double get(std::vector<unsigned char>* message)
		{
			return midi->getMessage(message);
		}

		void get()
		{
			stamp = get(&message);
			bytes = message.size();
			if (bytes)
				status = (int)message[0] >> 4;
		}

	};


	class MidiOut
	{
	private:
		RtMidiOut *midi = 0;
		std::vector<std::string> names;
		int port = -1;

	public:
		MidiOut()
		{

		}

		void startup()
		{
			try { midi = new RtMidiOut(); }
			catch (RtMidiError &error)
			{
				error.printMessage();
				exit(EXIT_FAILURE);
			}
		}

		void shutdown()
		{
			delete midi;
		}

		void getports(bool verbose = false)
		{
			names.clear();
			int n = midi->getPortCount();
			std::string name;
			
			for (int i = 0; i < n; i++)
			{
				try { name = midi->getPortName(i); }
				catch (RtMidiError &error)
				{
					error.printMessage();
					continue;
				}
				if (verbose)
					std::cout << "  output port #" << i+1 << ": " << name << '\n';
				names.push_back(name);
			}
		}

		int open(int number)
		{
			try
			{
				midi->openPort(number);
			}
			catch (RtMidiError &error)
			{
				error.printMessage();
				return 1;
			}
			port = number;
			return 0;
		}


		int open(std::string name)
		{
			for (int i = 0; i < names.size(); i++)
				if (names[i] == name)
					return open(i);
			return 1;
		}

		void send(std::vector<unsigned char>* message)
		{
			midi->sendMessage(message);
		}
	};
}
