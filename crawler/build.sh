#!/bin/bash
source ~/.bashrc

# Activate Conda environment
source "$(conda info --base)/etc/profile.d/conda.sh"
conda activate web_crawler || {
    echo "Failed to activate Conda environment 'web_crawler'."
    echo "Ensure the environment exists using: 'conda create -n web_crawler python=3.9'"
    exit 1
}

# Ensure Python is available
if ! command -v python &> /dev/null; then
    echo "Python is not available in the Conda environment."
    conda deactivate
    exit 1
fi

# Run the Python application
python crawler.py || {
    echo "Failed to execute crawler.py. Check for errors in the script."
    conda deactivate
    exit 1
}

# Deactivate Conda environment after execution
conda deactivate