#!/bin/bash

set -e

source ./env/.env.sh

# List all video devices
video_devices=($(ls /dev/video*))
working_device=""

# Loop through each device and test it
for device in "${video_devices[@]}"; do
    # Test the device with ffmpeg and capture output
    if ffmpeg -f v4l2 -i "$device" -t 1 -frames:v 1 -f null - > /dev/null 2>&1; then
        echo "Found working video device: $device"
        working_device="$device"
        break
    else
        echo "$device is not a working video input."
    fi
done

mkdir -p $LOCAL_RECORDINGS_DIR

ffmpeg \
-f v4l2 \
-framerate 10 \
-video_size 1280x720 \
-i $working_device \
-f pulse -i alsa_input.pci-0000_00_1f.3.analog-stereo \
-vf "drawtext=fontfile=/path/to/font.ttf:text='%{localtime\:%X}':x=10:y=10:fontsize=24:fontcolor=white:box=1:boxcolor=black@0.5" \
-c:v libx264 -preset ultrafast -c:a aac -b:a 128k \
-f hls \
-hls_list_size 9999 \
-hls_flags delete_segments+program_date_time \
-hls_allow_cache 0 \
-hls_segment_filename "$LOCAL_RECORDINGS_DIR/segment_%04d.ts" \
"$LOCAL_RECORDINGS_DIR/stream.m3u8"
