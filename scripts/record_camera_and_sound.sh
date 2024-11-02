#!/bin/bash

set -e

source ./env/.env.sh

# List all video devices
video_devices=($(ls /dev/video*))
working_video_device=""

# Loop through each device and test it
for device in "${video_devices[@]}"; do
    # Test the device with ffmpeg and capture output
    if ffmpeg -f v4l2 -i "$device" -t 1 -frames:v 1 -f null - > /dev/null 2>&1; then
        echo "Found working video device: $device"
        working_video_device="$device"
        break
    else
        echo "$device is not a working video input."
    fi
done

# List all PulseAudio sources and filter out `.monitor` sources
audio_devices=($(pactl list short sources | grep -v '\.monitor' | awk '{print $2}'))
working_audio_device=""

# Loop through each filtered audio source and test it
for device_name in "${audio_devices[@]}"; do
    # Test the device with ffmpeg for 1 second and discard the output
    if ffmpeg -f pulse -i "$device_name" -t 1 -f null - > /dev/null 2>&1; then
        echo "Found working audio input device: $device_name"
        working_audio_device="$device_name"
        break
    else
        echo "$device_name is not a working audio input."
    fi
done

mkdir -p $LOCAL_RECORDINGS_DIR

ffmpeg \
-f v4l2 \
-framerate 10 \
-video_size 1280x720 \
-i $working_video_device \
-f pulse -i $working_audio_device \
-vf "drawtext=fontfile=/path/to/font.ttf:text='%{localtime\:%X}':x=10:y=10:fontsize=24:fontcolor=white:box=1:boxcolor=black@0.5" \
-c:v libx264 \
-preset ultrafast \
-c:a aac \
-b:a 128k \
-f hls \
-hls_time 1 \
-hls_list_size 9999 \
-hls_flags delete_segments+program_date_time+independent_segments \
-hls_allow_cache 0 \
-hls_segment_type mpegts \
-hls_segment_filename "$LOCAL_RECORDINGS_DIR/segment_%04d.ts" \
"$LOCAL_RECORDINGS_DIR/stream.m3u8"