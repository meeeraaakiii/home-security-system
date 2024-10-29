# Useful commands

## List available PulseAudio devices
```bash
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
```
