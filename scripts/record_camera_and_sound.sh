#!/bin/bash

set -e

source ./env/.env.sh

mkdir -p $LOCAL_RECORDINGS_DIR

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
-hls_segment_filename "$LOCAL_RECORDINGS_DIR/segment_%04d.ts" \
"$LOCAL_RECORDINGS_DIR/stream.m3u8"
