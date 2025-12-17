#!/bin/bash
# setup-ci-environment.sh
# Sets up CI environment for testing

set -e

echo "Setting up CI environment..."

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
    echo "Detected OS: Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
    echo "Detected OS: macOS"
else
    OS="Unknown"
    echo "Warning: Unknown OS: $OSTYPE"
fi

# Install dependencies based on OS
if [ "$OS" = "Linux" ]; then
    echo "Installing Linux dependencies..."
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y gcc make xorg libxext-dev libbsd-dev xvfb
    elif command -v yum &> /dev/null; then
        sudo yum install -y gcc make libX11-devel libXext-devel
    else
        echo "Error: No supported package manager found"
        exit 1
    fi
elif [ "$OS" = "macOS" ]; then
    echo "macOS environment ready (no additional dependencies needed)"
fi

# Install norminette if not present
if ! command -v norminette &> /dev/null; then
    echo "Installing norminette..."
    python3 -m pip install --upgrade pip setuptools
    python3 -m pip install norminette
fi

# Verify installations
echo ""
echo "Verifying installations..."
gcc --version | head -1
make --version | head -1
norminette --version

echo ""
echo "✓ CI environment setup complete"
