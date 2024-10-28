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

## Install Crow
```bash
cd ~/ProgramFiles
git clone git@github.com:CrowCpp/Crow.git
mkdir build
cd build
cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF
sudo make install
```

## Run
```bash
cmake -B build -S . && cmake --build build && ./build/ShowStream
```

## TODO
- ~~JSON configuration file (put users there)~~
- ~~Real sessions (use map to store sessions)~~
- ~~Rate limiting~~
- Set up scripts to generate and move stream files
- Serve css and html files from a map?
- Move crow headers into the project?
- Way to quickly set up show-stream http app on the remote server.
