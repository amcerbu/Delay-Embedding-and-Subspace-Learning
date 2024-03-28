// udpreceive.h
#pragma once

#include "includes.h"
#include "tinyosc.h"
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "stream.h"

namespace soundmath
{
	class UDPReceive
	{
	public:
		UDPReceive(int port = 9000) : port(port)
		{
			ready = false;
		}

		~UDPReceive()
		{
			shutdown();
		}

		void prepare(bool report = true)
		{
			fd = socket(AF_INET, SOCK_DGRAM, 0);
			fcntl(fd, F_SETFL, O_NONBLOCK); // set the socket to non-blocking
			sin.sin_family = AF_INET;
			sin.sin_port = htons(port);
			sin.sin_addr.s_addr = INADDR_ANY;
			::bind(fd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
			std::cout << std::endl << "Listening for osc messages on port " << port << std::endl;
			ready = true;
		}

		void read(Stream<tosc_message>* message_buff)
		{
			int len;
			tosc_message osc;
			FD_ZERO(&readSet);
			FD_SET(fd, &readSet);
			if (select(fd+1, &readSet, NULL, NULL, &timeout) > 0)
			{
				while ((len = (int) recvfrom(fd, buffer, sizeof(buffer), 0, &sa, &sa_len)) > 0)
				{
					if (tosc_isBundle(buffer))
					{
						tosc_bundle bundle;
						tosc_parseBundle(&bundle, buffer, len);
						tosc_getTimetag(&bundle);
						while (tosc_getNextMessage(&bundle, &osc))
							message_buff->push(osc);
					}
					else
					{
						tosc_parseMessage(&osc, buffer, len);
						message_buff->push(osc);
					}	
				}
			}
		}

		void shutdown()
		{
			if (ready)
			{
				ready = false;
				close(fd);
			}
		}

	private:
		int port;
		char buffer[2048];
		bool ready;
		int fd;
		struct sockaddr_in sin;
		fd_set readSet;
		struct timeval timeout = {0, 0}; // select times out after 0 seconds
		struct sockaddr sa; // can be safely cast to sockaddr_in
		socklen_t sa_len = sizeof(struct sockaddr_in);
	};
}
