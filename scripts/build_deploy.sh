#!/bin/bash

source ./scripts/requirements.sh

# Deployment directory
DEPLOY_DIR="./build/deploy"

# Create the directory structure
echo "Creating deployment directory structure..."
mkdir -p $DEPLOY_DIR/{cfg,env,src/assets/html,src/assets/static/css,scripts,tmp}

# Copy necessary files
echo "Copying files..."

# Copy binary
cp ./build/ShowStream $DEPLOY_DIR/

# Copy remote config file
cp ./cfg/config.remote.json $DEPLOY_DIR/cfg/config.json
# copy env file
cp ./env/.env.sh $DEPLOY_DIR/env/

# Copy HTML files
cp -r ./src/assets/html/*.html $DEPLOY_DIR/src/assets/html/
# Copy CSS files
cp -r ./src/assets/static/css/*.css $DEPLOY_DIR/src/assets/static/css/

# Copy run app script
# cp -r ./scripts/show-stream.sh $DEPLOY_DIR/scripts/

# Confirm structure
echo "Deployment directory structure created:"
find $DEPLOY_DIR

# remove dir
ssh -p $SSH_PORT $SSH_USER@$SSH_HOST "rm -rf ~/show-stream"
# Upload to server
echo "Uploading to the server..."
scp -P $SSH_PORT -r $DEPLOY_DIR ${SSH_USER}@${SSH_HOST}:~/show-stream

# Cleanup local deployment directory
echo "Cleaning up local deployment directory..."
rm -rf $DEPLOY_DIR

echo "Deployment complete."
