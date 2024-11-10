# Home Security System
Record webcam footage with sound using ffmpeg.
Upload files to a remote server.
App shows stream of webcam footage from that remote server (with login page).

## Installation
```bash
sudo apt update
sudo apt install build-essential
sudo apt install pkg-config
# ffmpeg
sudo apt install ffmpeg
# asio
sudo apt install libasio-dev
# inotifywait command
sudo apt install inotify-tools
```

## Run
```bash
# build and run locally
cmake -B build -S . && cmake --build build && ./build/ShowStream
# build show-stream http app
cmake -B build -S . && cmake --build build
# run locally
./build/ShowStream

# remove ~/recordings dir on the server
./scripts/clean_up.sh server
# clean ./tmp/ dir of segment* and stream.m3u8 files
./scripts/clean_up.sh

# continuously upload .ts and .m3u8 files to the server
./scripts/upload_stream_files.sh

# stream
./scripts/record_camera_and_sound.sh
```

## Deploy
```bash
# first build
cmake -B build -S . && cmake --build build
# then deploy app on the remote server
./scripts/build_deploy.sh

# run following commands on the server
cd ~/.hss
./ShowStream
```

## TODO
- ~~JSON configuration file (put users there)~~
- ~~Real sessions (use map to store sessions)~~
- ~~Rate limiting~~
- ~~Move crow headers into the project~~
- ~~Set up scripts to generate and move stream files~~
- ~~Way to quickly set up show-stream http app on the remote server~~
- ~~Make sure that ffmpeg command works with any hardware~~
    - ~~Detect video devices~~
    - ~~Detect microphone~~
- ~~Control port with a config file~~
- ~~Control recordings dir with a config file~~
- ~~Investigate why Android's Chrome browser can't play HLS playlist~~
- Security
    - Login
        - ~~Generate session IDs on the server~~
        - ~~Rate limiter~~
        - ~~Add the Secure flag to cookies, ensuring they are only sent over HTTPS connections.~~
        - Use a cryptographically secure random number generator for session IDs
        - Store hashed and salted passwords using a strong hashing algorithm (e.g., bcrypt, Argon2) and compare hashed values during authentication
        - ~~Ensure that cookie parsing is robust and watch out for header injection~~
            - ~~Injection can't happen as we supply session id ourselves.~~
    - ~~File serving routes like / and /static/css~~
        - ~~Path traversal (no need Crow comes with path normalization out of the box it seems)~~
            - ~~Crow normalizes paths and confines access within the specified directory structure. This means that attempts to use path traversal (../) in URLs are typically resolved relative to the designated root directory, preventing access to files outside of it.~~

## ffmpeg
```bash
# ffmpeg
sudo apt-get install libavcodec-dev libavformat-dev libavfilter-dev libavdevice-dev libavutil-dev libswscale-dev libswresample-dev

# alsa sound
sudo apt-get update
sudo apt-get install libasound2-dev
```
