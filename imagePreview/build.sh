#!/bin/bash
source ~/.bashrc

# Activate Conda environment
source "$(conda info --base)/etc/profile.d/conda.sh"
conda activate imagePreview || {
    echo "Failed to activate Conda environment 'imagePreview'."
    echo "Ensure the environment exists using: 'conda create -n imagePreview python=3.9'"
    exit 1
}

# Ensure Python is available
if ! command -v python &> /dev/null; then
    echo "Python is not available in the Conda environment."
    conda deactivate
    exit 1
fi

# Run the Python application
python main.py || {
    echo "Failed to execute main.py. Check for errors in the script."
    conda deactivate
    exit 1
}

# Deactivate Conda environment after execution
conda deactivate
