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

## Useful commands
```bash
# List available PulseAudio devices:
pactl list sources short
```

## Test camera
```bash
ffplay /dev/video0
# into a file
ffmpeg -f v4l2 -framerate 30 -video_size 1280x720 -i /dev/video0 output.mp4
# ts file for streaming
ffmpeg -f v4l2 -framerate 30 -video_size 1280x720 -i /dev/video0 -c:v libx264 -f mpegts output.ts
# with timestamp
ffmpeg -f v4l2 -framerate 30 -video_size 1280x720 -i /dev/video0 -vf "drawtext=fontfile=/path/to/font.ttf:text='%{localtime\:%X}':x=10:y=10:fontsize=24:fontcolor=white:box=1:boxcolor=black@0.5" -c:v libx264 -f mpegts output.ts
# lower delay
ffmpeg -f v4l2 -framerate 30 -video_size 1280x720 -i /dev/video0 -vf "drawtext=fontfile=/path/to/font.ttf:text='%{localtime\:%X}':x=10:y=10:fontsize=24:fontcolor=white:box=1:boxcolor=black@0.5" -c:v libx264 -preset ultrafast -f mpegts output.ts

# with audio
ffmpeg \
-f v4l2 -framerate 30 -video_size 1280x720 -i /dev/video0 \
-f pulse -i alsa_input.pci-0000_00_1f.3.analog-stereo \
-vf "drawtext=fontfile=/path/to/font.ttf:text='%{localtime\:%X}':x=10:y=10:fontsize=24:fontcolor=white:box=1:boxcolor=black@0.5" \
-c:v libx264 -preset ultrafast -c:a aac -b:a 128k \
-f mpegts ./tmp/stream.ts

# over ssh
ffmpeg \
-f v4l2 -framerate 10 -video_size 1280x720 -i /dev/video0 \
-f pulse -i alsa_input.pci-0000_00_1f.3.analog-stereo \
-vf "drawtext=fontfile=/path/to/font.ttf:text='%{localtime\:%X}':x=10:y=10:fontsize=24:fontcolor=white:box=1:boxcolor=black@0.5" \
-c:v libx264 -preset ultrafast -c:a aac -b:a 128k \
-f mpegts - | ssh vagrant@127.0.0.1 -p 2222 'cat > /home/vagrant/recordings/stream.ts'

# hls
ffmpeg \
-f v4l2 \
-framerate 10 \
-video_size 1280x720 \
-i /dev/video0 \
-f pulse -i alsa_input.pci-0000_00_1f.3.analog-stereo \
-vf "drawtext=fontfile=/path/to/font.ttf:text='%{localtime\:%X}':x=10:y=10:fontsize=24:fontcolor=white:box=1:boxcolor=black@0.5" \
-c:v libx264 -preset ultrafast -c:a aac -b:a 128k \
-f hls \
-hls_list_size 9999 \
-hls_flags delete_segments+program_date_time \
-hls_allow_cache 0 \
-hls_segment_filename "./tmp/segment_%04d.ts" \
'./tmp/stream.m3u8'

# hls ssh, only stream.m3u8 will be on the server. Move segments using ./src/scripts/sent_ts_files.sh
ffmpeg \
-f v4l2 \
-framerate 10 \
-video_size 1280x720 \
-i /dev/video0 \
-f pulse -i alsa_input.pci-0000_00_1f.3.analog-stereo \
-vf "drawtext=fontfile=/path/to/font.ttf:text='%{localtime\:%X}':x=10:y=10:fontsize=24:fontcolor=white:box=1:boxcolor=black@0.5" \
-c:v libx264 -preset ultrafast -c:a aac -b:a 128k \
-f hls \
-hls_list_size 9999 \
-hls_flags delete_segments+program_date_time \
-hls_allow_cache 0 \
-hls_segment_filename "./tmp/segment_%04d.ts" \
- | ssh vagrant@127.0.0.1 -p 2222 'cat > /home/vagrant/recordings/stream.m3u8'
```

## Run
```bash
cmake -B build -S . && cmake --build build && ./build/ShowStream
```


## TODO
- JSON configuration file (put users there)
- Real sessions (use map to store sessions)
- Rate limiting
- Set up scripts to generate and move stream files
