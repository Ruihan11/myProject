#!/bin/bash

# 确保脚本接收到项目目录作为参数
if [ -z "$1" ]; then
    echo "Usage: bash auto.sh <project_directory>"
    exit 1
fi

PROJECT_DIR=$1

echo ">> Performing auto push to remote GitHub repository"
sleep 2

# 切换到目标项目目录
cd "$PROJECT_DIR" || { echo "Error: Could not access directory $PROJECT_DIR"; exit 1; }

# 添加所有更改到暂存区
git add .

# 提交更改
COMMIT_MESSAGE="Automatic message from $(hostname) on $(date)"
echo ">> Commit message: $COMMIT_MESSAGE"
git commit -m "$COMMIT_MESSAGE"

# 推送到远程仓库的 main 分支
echo ">> Pushing to remote repository"
git push origin main

# 提示完成
if [ $? -eq 0 ]; then
    echo ">> Auto push completed successfully."
else
    echo ">> Auto push failed. Please check for issues."
    exit 1
fi