// fourier.h
#pragma once

#include "includes.h"
#include "wave.h"
#include "delay.h"
#include <fftw3.h>

namespace soundmath
{
	class Fourier
	{
	public:
		int (*processor)(const std::complex<double>* in, std::complex<double>* out);

		Fourier(int (*processor)(const std::complex<double>*, std::complex<double>*), int N, int laps) : processor(processor), N(N), laps(laps), stride(N / laps)
		{
			in = (std::complex<double>*) fftw_malloc(sizeof(std::complex<double>) * N * laps * 2);
			middle = (std::complex<double>*) fftw_malloc(sizeof(std::complex<double>) * N * laps * 2);
			out = (std::complex<double>*) fftw_malloc(sizeof(std::complex<double>) * N * laps * 2); // lookup with out[j + N * i]

			analysis = new fftw_plan[laps * 2];
			synthesis = new fftw_plan[laps * 2];

			for (int i = 0; i < laps * 2; i++)
			{
				analysis[i] = fftw_plan_dft_1d(N, reinterpret_cast<fftw_complex*>(in + i * N), reinterpret_cast<fftw_complex*>(middle + i * N), FFTW_FORWARD, FFTW_MEASURE);
				synthesis[i] = fftw_plan_dft_1d(N, reinterpret_cast<fftw_complex*>(out + i * N), reinterpret_cast<fftw_complex*>(in + i * N), FFTW_BACKWARD, FFTW_MEASURE);
			}

			writepoints = new int[laps * 2];
			readpoints = new int[laps * 2];

			memset(writepoints, 0, sizeof(int) * laps * 2);
			memset(readpoints, 0, sizeof(int) * laps * 2);

			for (int i = 0; i < 2 * laps; i++) // initialize half of writepoints
				writepoints[i] = -stride * i;

			reading = new bool[laps * 2];
			writing = new bool[laps * 2];

			memset(reading, false, sizeof(bool) * laps * 2);
			memset(writing, true, sizeof(bool) * laps * 2);
		}

		~Fourier()
		{
			for (int i = 0; i < laps * 2; i++)
			{
				fftw_destroy_plan(analysis[i]);
				fftw_destroy_plan(synthesis[i]);
			}
			fftw_free(in); fftw_free(middle); fftw_free(out);

			delete [] writepoints;
			delete [] readpoints;
			delete [] reading;
			delete [] writing;
		}

		// writes a single sample (with windowing) into the in array
		void write(double real, double imag = 0)
		{
			for (int i = 0; i < laps * 2; i++)
			{
				if (writing[i])
				{
					if (writepoints[i] >= 0)
					{
						double window = halfhann(writepoints[i] / (double)N);
						in[writepoints[i] + N * i].real(window * real);
						in[writepoints[i] + N * i].imag(window * imag);
					}
					writepoints[i]++;

					if (writepoints[i] == N)
					{
						writing[i] = false;
						reading[i] = true;
						readpoints[i] = 0;

						forward(i); // FTs ith in to ith middle buffer
						process(i); // user-defined; ought to move info from ith middle to out buffer 
						backward(i); // IFTs ith out to ith in buffer
					}
				}
			}
		}

		inline void forward(const int i)
		{
			fftw_execute(analysis[i]);
		}

		inline void backward(const int i)
		{
			fftw_execute(synthesis[i]);
		}

		// executes user-defined callback
		inline void process(const int i)
		{
			processor((middle + i * N), (out + i * N));
		}

		// read a single reconstructed sample (real and imaginary parts)
		void read(double* real, double* imag)
		{
			long double realaccum = 0;
			long double imagaccum = 0;

			for (int i = 0; i < laps * 2; i++)
			{
				if (reading[i])
				{
					double window = halfhann(readpoints[i] / (double)N);
					std::complex<double> sample = in[readpoints[i] + N * i];
					realaccum += window * sample.real();
					imagaccum += window * sample.imag();

					readpoints[i]++;

					if (readpoints[i] == N)
					{
						writing[i] = true;
						reading[i] = false;
						writepoints[i] = 0;
					}
				}
			}

			realaccum /= N * laps / 2;
			imagaccum /= N * laps / 2;

			*real = realaccum;
			*imag = imagaccum;
		}



	private:
		std::complex<double> *in, *middle, *out;

		int N;
		int laps;
		int stride;

		fftw_plan* analysis;
		fftw_plan* synthesis;
		int* writepoints;
		int* readpoints;
		bool* reading;
		bool* writing;
	};


	class Cosine
	{
	public:
		// initialize a DCT of size n
		Cosine(int N, double** user_in, double** user_out)
		{
			in = (double*) fftw_malloc(sizeof(double) * N);
			out = (double*) fftw_malloc(sizeof(double) * N);

			*user_in = in;
			*user_out = out;

			p = fftw_plan_r2r_1d(N, in, out, FFTW_REDFT10, FFTW_MEASURE);
			q = fftw_plan_r2r_1d(N, out, in, FFTW_REDFT01, FFTW_MEASURE);
		}

		~Cosine()
		{
			fftw_destroy_plan(p);
			fftw_destroy_plan(q);
			fftw_free(in); fftw_free(out);
		}

		void forward()
		{
			fftw_execute(p);
		}

		void backward()
		{
			fftw_execute(q);
		}

	private:
		double *in, *out;
		fftw_plan p, q;

	};
}
