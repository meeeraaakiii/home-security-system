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
cmake -B build -S . && cmake --build build && ./build/ShowStream
```

## TODO
- ~~JSON configuration file (put users there)~~
- ~~Real sessions (use map to store sessions)~~
- ~~Rate limiting~~
- ~~Move crow headers into the project~~
- Set up scripts to generate and move stream files
- Way to quickly set up show-stream http app on the remote server.
