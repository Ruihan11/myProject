#!/bin/bash

# 检查输入参数
if [ -z "$1" ]; then
    echo "Usage: bash auto.sh <project_directory> | all"
    exit 1
fi

# 确保 README.md 和 auto.sh 同步更新
ensure_readme_and_script_synced() {
    echo ">> Ensuring README.md and auto.sh are synced and committed..."
    git add README.md auto.sh
    if git diff --cached --quiet; then
        echo ">> No changes in README.md or auto.sh to commit."
    else
        COMMIT_MESSAGE="Sync README.md and auto.sh from $(hostname) on $(date)"
        git commit -m "$COMMIT_MESSAGE"
        echo ">> Synced README.md and auto.sh."
    fi
}

# 定义函数：处理某个项目目录
process_directory() {
    local DIR=$1
    echo ">> Performing auto push for directory: $DIR"
    cd "$DIR" || { echo "Error: Could not access directory $DIR"; exit 1; }

    # 确保 README.md 和 auto.sh 同步
    ensure_readme_and_script_synced

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
    ensure_readme_and_script_synced
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
