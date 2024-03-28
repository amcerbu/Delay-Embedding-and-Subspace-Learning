#pragma once

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>

namespace soundmath
{
    class WavFile
    {
    public:
        WavFile(std::string filename, int sample_rate, int bitrate, int channels) : 
            filename(filename), sample_rate(sample_rate), bitrate(bitrate), channels(channels),
            writing(false), opened(false), 
            max_amplitude(pow(2.0, bitrate - 1) - 1), f(filename, std::ios::binary)
        {
        }

        void open()
        {
            assert(!opened); //, "WavFile already open for writing.");

            // f = std::ofstream( filename, std::ios::binary );
            f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
            write_word( f,                                     16, 4 );  // no extension data
            write_word( f,                                      1, 2 );  // PCM - integer samples
            write_word( f,                               channels, 2 );  // two channels (stereo file)
            write_word( f,                            sample_rate, 4 );  // samples per second (Hz)
            write_word( f, (sample_rate * bitrate * channels) / 8, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
            write_word( f,               (bitrate * channels) / 8, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
            write_word( f,                                bitrate, 2 );  // number of bits per sample (use a multiple of 8)

            // Write the data chunk header
            data_chunk_pos = f.tellp();
            f << "data----";  // (chunk size to be filled in later)
        
            opened = true;
        }

        // expects numbers in [-1, 1]
        void write(double sample)
        {
            assert(opened); //, "WavFile must be opened before writing.");
            write_word( f, (int)(max_amplitude * sample), bitrate / 8 );
        }

        void close()
        {
            assert(opened); //, "WavFile must be opened before closing.");

            // (We'll need the final file size to fix the chunk sizes above)
            size_t file_length = f.tellp();

            // Fix the data chunk header to contain the data size
            f.seekp( data_chunk_pos + 4 );
            write_word( f, file_length - data_chunk_pos + 8 );

            // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
            f.seekp( 0 + 4 );
            write_word( f, file_length - 8, 4 ); 

            opened = false;
        }

    private:
        std::string filename;
        int sample_rate, bitrate, channels;
        bool writing;
        bool opened;
        double max_amplitude;

        std::ofstream f;
        size_t data_chunk_pos;

        template <typename Word> 
        std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
        {
            for (; size; --size, value >>= 8)
                outs.put( static_cast <char> (value & 0xFF) );
            return outs;
        }
    };
}