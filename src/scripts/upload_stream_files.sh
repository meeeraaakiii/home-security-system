#!/bin/bash

# quit on any error
set -e

source ./env/.env.sh

# Define the local directory where .ts files and stream.m3u8 are stored
LOCAL_DIR="./tmp"
# Define the remote directory and SSH information
REMOTE_DIR="~/recordings"

# Create the remote directory if it doesn't exist
ssh -p $SSH_PORT $SSH_USER@$SSH_HOST "mkdir -p $REMOTE_DIR"

# Start monitoring both .ts files and the .m3u8 file
inotifywait -m -e close_write --format "%w%f" "$LOCAL_DIR" | while read NEW_FILE
do
    # Upload the stream.m3u8 file whenever it’s modified
    echo "File updated/created: '$NEW_FILE'"
    if [[ "$NEW_FILE" == *.m3u8.tmp ]]; then
        NEW_FILE="${NEW_FILE%.tmp}"
        scp -P "$SSH_PORT" "$NEW_FILE" "$SSH_USER@$SSH_HOST:$REMOTE_DIR"
        echo "Uploaded $NEW_FILE to server at $(date)"
    fi

    # Upload .ts segment files whenever they’re created
    if [[ "$NEW_FILE" == *.ts ]]; then
        scp -P "$SSH_PORT" "$NEW_FILE" "$SSH_USER@$SSH_HOST:$REMOTE_DIR"
        echo "Uploaded $NEW_FILE to server at $(date)"
    fi
done
