#!/bin/bash

# Define the local directory where .ts files are stored
LOCAL_DIR="./tmp"
# Define the remote directory and SSH information
REMOTE_DIR="/home/vagrant/recordings"
SSH_USER="vagrant"
SSH_HOST="127.0.0.1"
SSH_PORT="2222"

# Monitor the directory for new .ts files
inotifywait -m -e create --format "%f" "$LOCAL_DIR" | while read NEW_FILE
do
    # Check if the new file has a .ts extension
    if [[ "$NEW_FILE" == *.ts ]]; then
        # Send the file over SSH
        scp -P "$SSH_PORT" "$LOCAL_DIR/$NEW_FILE" "$SSH_USER@$SSH_HOST:$REMOTE_DIR"
        
        # Optionally, delete the file after sending
        # rm "$LOCAL_DIR/$NEW_FILE"
    fi
done
