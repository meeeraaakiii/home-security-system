#include <alsa/asoundlib.h>
#include <iostream>
#include <vector>

const int SAMPLE_RATE = 44100;
const int CHANNELS = 1;
const int BITS_PER_SAMPLE = 16;
const int BUFFER_SIZE = SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8); // 1 second buffer

int main() {
    int rc;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;
    char *buffer = new char[BUFFER_SIZE];

    // Open PCM device for recording (capture).
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        std::cerr << "Unable to open PCM device: " << snd_strerror(rc) << std::endl;
        return 1;
    }

    // Allocate a hardware parameters object.
    snd_pcm_hw_params_malloc(&params);

    // Fill it in with default values.
    snd_pcm_hw_params_any(handle, params);

    // Set the desired hardware parameters.
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, CHANNELS);
    val = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

    // Write the parameters to the driver
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        std::cerr << "Unable to set hardware parameters: " << snd_strerror(rc) << std::endl;
        return 1;
    }

    frames = BUFFER_SIZE / (BITS_PER_SAMPLE / 8);

    // Start recording
    std::cout << "Recording... Press Enter to stop." << std::endl;
    while (std::cin.get() != '\n') {
        rc = snd_pcm_readi(handle, buffer, frames);
        if (rc == -EPIPE) {
            std::cerr << "Overrun occurred" << std::endl;
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            std::cerr << "Error reading from PCM device: " << snd_strerror(rc) << std::endl;
        } else if (rc != (int)frames) {
            std::cerr << "Short read, read " << rc << " frames" << std::endl;
        }

        // Process `buffer` here, such as writing it to a file or further processing
        // for now not implemented
        std::cout << "Captured audio frame!" << std::endl;
    }

    // Clean up
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    snd_pcm_hw_params_free(params);
    delete[] buffer;

    std::cout << "Recording stopped." << std::endl;
    return 0;
}
