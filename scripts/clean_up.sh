#!/bin/bash

source ./env/.env.sh

WHAT_TO_CLEAN="local"
if [[ $1 == "server" ]]; then
    WHAT_TO_CLEAN="server"
fi


if [[ $WHAT_TO_CLEAN == "server" ]]; then
    echo "Cleaning up remote dir: $REMOTE_RECORDINGS_DIR"
    ssh -p $SSH_PORT $SSH_USER@$SSH_HOST "rm -rf $REMOTE_RECORDINGS_DIR"
else
    echo "Cleaning up local dir: $LOCAL_RECORDINGS_DIR"
    # Attempt to delete the files
    rm $LOCAL_RECORDINGS_DIR/*.ts 2>/dev/null
    result1=$?
    rm $LOCAL_RECORDINGS_DIR/*.mp4 2>/dev/null
    # Check if the rm command was successful
    if [[ $? -eq 0 || $result1 -eq 0  ]]; then
        echo "Deleted segments"
    else
        echo "No segments to delete"
    fi

    # Attempt to delete the files
    rm "$LOCAL_RECORDINGS_DIR/stream.m3u8" 2>/dev/null
    # Check if the rm command was successful
    if [[ $? -eq 0 ]]; then
        echo "Deleted stream.m3u8"
    else
        echo "No stream.m3u8 to delete"
    fi
fi
