// oscstream.h
#pragma once

#include "includes.h"
#include "tinyosc.h"

// circular FIFO buffer
template <typename T> class Stream
{
public:
	Stream(uint capacity = 2048) : 
		capacity(capacity), size(0), readhead(0), writehead(0)
	{
		buffer = new tosc_message[capacity];
	}

	~Stream()
	{
		delete [] buffer;
	}

	void push(T osc)
	{
		buffer[writehead] = osc;
		writehead++;
		writehead %= capacity;
		size++;

		if (writehead == readhead)
			std::cout << "error: Stream is full" << std::endl;
	}

	T pop()
	{
		assert(nonempty());
		T out = buffer[readhead];
		size--;
		readhead++;
		readhead %= capacity;
		return out;
	}

	bool nonempty()
	{
		return size > 0;
	}

private:
	T* buffer;
	int capacity;
	int size;
	int readhead;
	int writehead;
};
