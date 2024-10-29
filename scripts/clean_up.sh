#!/bin/bash

source ./env/.env.sh

WHAT_TO_CLEAN="local"
if [[ $1 == "server" ]]; then
    WHAT_TO_CLEAN="server"
fi


if [[ $WHAT_TO_CLEAN == "server" ]]; then
    ssh -p $SSH_PORT $SSH_USER@$SSH_HOST "rm -rf $REMOTE_RECORDINGS_DIR"
else
    # Attempt to delete the files
    rm "$LOCAL_RECORDINGS_DIR/segment*" 2>/dev/null
    # Check if the rm command was successful
    if [[ $? -eq 0 ]]; then
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
