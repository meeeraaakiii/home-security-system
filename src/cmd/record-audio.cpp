#include <alsa/asoundlib.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <fstream>
#include <signal.h>

const int SAMPLE_RATE = 44100;
const int CHANNELS = 1;
const int BITS_PER_SAMPLE = 16;
const int BUFFER_SIZE = SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8); // 1 second buffer
bool stop_recording = false; // Flag to stop recording

// Helper function to write a WAV header to the file
void write_wav_header(std::ofstream &file, int sample_rate, int num_channels, int bits_per_sample, int data_size) {
    file.write("RIFF", 4);
    int32_t chunk_size = 36 + data_size;
    file.write(reinterpret_cast<const char*>(&chunk_size), 4);
    file.write("WAVE", 4);

    // Subchunk1 ("fmt " chunk)
    file.write("fmt ", 4);
    int32_t subchunk1_size = 16;
    file.write(reinterpret_cast<const char*>(&subchunk1_size), 4);
    int16_t audio_format = 1;
    file.write(reinterpret_cast<const char*>(&audio_format), 2);
    file.write(reinterpret_cast<const char*>(&num_channels), 2);
    file.write(reinterpret_cast<const char*>(&sample_rate), 4);
    int32_t byte_rate = sample_rate * num_channels * bits_per_sample / 8;
    file.write(reinterpret_cast<const char*>(&byte_rate), 4);
    int16_t block_align = num_channels * bits_per_sample / 8;
    file.write(reinterpret_cast<const char*>(&block_align), 2);
    file.write(reinterpret_cast<const char*>(&bits_per_sample), 2);

    // Subchunk2 ("data" chunk)
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&data_size), 4);
}

// Function to set terminal to non-blocking mode
void set_nonblocking_input() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

// Function to restore terminal settings
void restore_terminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Signal handler for Ctrl+C
void signal_handler(int signum) {
    stop_recording = true;
    std::cout << "\nStopping recording (Ctrl+C detected)..." << std::endl;
    // exit(0);
}

int main() {
    int rc;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames = SAMPLE_RATE * CHANNELS;
    char *buffer = new char[BUFFER_SIZE];

    // Set up the signal handler for Ctrl+C
    signal(SIGINT, signal_handler);

    // Open PCM device for recording (capture).
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        std::cerr << "Unable to open PCM device: " << snd_strerror(rc) << std::endl;
        return 1;
    }

    // Allocate a hardware parameters object.
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, CHANNELS);
    val = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        std::cerr << "Unable to set hardware parameters: " << snd_strerror(rc) << std::endl;
        return 1;
    }

    std::ofstream audio_file("./tmp/captured_audio.wav", std::ios::binary);
    if (!audio_file) {
        std::cerr << "Failed to open output file." << std::endl;
        return 1;
    }

    int header_size = 44;
    audio_file.seekp(header_size);
    int total_data_size = 0;

    set_nonblocking_input();
    std::cout << "Recording... Press Enter or Ctrl+C to stop." << std::endl;

    // Start recording loop
    while (!stop_recording) {
        rc = snd_pcm_readi(handle, buffer, frames);
        if (rc == -EPIPE) {
            std::cerr << "Overrun occurred" << std::endl;
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            std::cerr << "Error reading from PCM device: " << snd_strerror(rc) << std::endl;
        } else if (rc != (int)frames) {
            std::cerr << "Short read, read " << rc << " frames" << std::endl;
        } else {
            audio_file.write(buffer, rc * (BITS_PER_SAMPLE / 8));
            total_data_size += rc * (BITS_PER_SAMPLE / 8);
            std::cout << "Captured audio frame!" << std::endl;
        }

        usleep(1000);
    }

    // Finalize the WAV header with the correct data size
    audio_file.seekp(0);
    write_wav_header(audio_file, SAMPLE_RATE, CHANNELS, BITS_PER_SAMPLE, total_data_size);

    restore_terminal();
    audio_file.close();
    snd_pcm_drop(handle);
    snd_pcm_close(handle);
    snd_pcm_hw_params_free(params);
    delete[] buffer;

    std::cout << "Recording stopped and saved to ./tmp/captured_audio.wav" << std::endl;
    return 0;
}
