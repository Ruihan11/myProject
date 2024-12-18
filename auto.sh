#!/bin/bash

# 检查输入参数
if [ -z "$1" ]; then
    echo "Usage: bash auto.sh <project_directory> | all"
    exit 1
fi

# 定义函数：处理某个项目目录
process_directory() {
    local DIR=$1
    echo ">> Performing auto push for directory: $DIR"
    cd "$DIR" || { echo "Error: Could not access directory $DIR"; exit 1; }

    git add .
    COMMIT_MESSAGE="Automatic message from $(hostname) on $(date)"
    echo ">> Commit message: $COMMIT_MESSAGE"
    git commit -m "$COMMIT_MESSAGE"
    git push origin main

    echo ">> Auto push completed for directory: $DIR"
    cd - > /dev/null
}

# 检查是否是 "all"
if [ "$1" == "all" ]; then
    echo ">> Performing 'git add .' for the current directory"
    git add .
    COMMIT_MESSAGE="Automatic commit for all changes from $(hostname) on $(date)"
    echo ">> Commit message: $COMMIT_MESSAGE"
    git commit -m "$COMMIT_MESSAGE"
    git push origin main
    echo ">> Auto push completed for all changes."
else
    # 针对指定目录
    process_directory "$1"
fi
