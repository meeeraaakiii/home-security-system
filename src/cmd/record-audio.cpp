#include <alsa/asoundlib.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

const int SAMPLE_RATE = 44100;
const int CHANNELS = 1;
const int BITS_PER_SAMPLE = 16;
const int BUFFER_SIZE = SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8); // 1 second buffer

// Function to set terminal to non-blocking mode
void set_nonblocking_input() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON); // Disable canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // Set stdin to non-blocking
}

// Function to restore terminal settings
void restore_terminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON; // Re-enable canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int main() {
    int rc;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames = SAMPLE_RATE * CHANNELS;  // Set a smaller, fixed frame size for testing
    char *buffer = new char[BUFFER_SIZE];

    // Open PCM device for recording (capture).
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);  // Use USB audio device
    if (rc < 0) {
        std::cerr << "Unable to open PCM device: " << snd_strerror(rc) << std::endl;
        return 1;
    }

    // Allocate a hardware parameters object.
    snd_pcm_hw_params_malloc(&params);

    // Fill it in with default values.
    snd_pcm_hw_params_any(handle, params);

    // Set the desired hardware parameters.
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED); // Set access type
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, CHANNELS);
    val = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir); // Set period size

    // Write the parameters to the driver
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        std::cerr << "Unable to set hardware parameters: " << snd_strerror(rc) << std::endl;
        return 1;
    }

    std::cout << "Frames to capture per read: " << frames << std::endl;
    std::cout << "Sample Rate: " << SAMPLE_RATE << ", Channels: " << CHANNELS << std::endl;

    // Set non-blocking input mode
    set_nonblocking_input();
    std::cout << "Recording... Press Enter to stop." << std::endl;

    // Start recording loop
    while (true) {
        rc = snd_pcm_readi(handle, buffer, frames);
        if (rc == -EPIPE) {
            std::cerr << "Overrun occurred" << std::endl;
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            std::cerr << "Error reading from PCM device: " << snd_strerror(rc) << std::endl;
        } else if (rc != (int)frames) {
            std::cerr << "Short read, read " << rc << " frames" << std::endl;
        } else {
            std::cout << "Captured audio frame!" << std::endl;
        }

        // Check if Enter was pressed
        if (std::cin.get() == '\n') {
            break;
        }

        // Add a small sleep to avoid excessive CPU usage
        usleep(1000);
    }

    // Clean up
    restore_terminal(); // Restore terminal settings
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    snd_pcm_hw_params_free(params);
    delete[] buffer;

    std::cout << "Recording stopped." << std::endl;
    return 0;
}
