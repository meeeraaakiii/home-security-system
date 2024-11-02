#!/bin/bash

# List of required environment variables
required_env_vars=("SSH_USER" "SSH_HOST" "SSH_PORT" "REMOTE_RECORDINGS_DIR" "LOCAL_RECORDINGS_DIR")

# Loop through each variable and check if it's set
for var in "${required_env_vars[@]}"; do
    if [ -z "${!var}" ]; then
        echo "Error: Required environment variable '$var' is not set."
        exit 1  # Use 'return' for sourcing compatibility
    else
        echo "Environment variable '$var' is set to '${!var}'."
    fi
done

echo "All required environment variables are set."
