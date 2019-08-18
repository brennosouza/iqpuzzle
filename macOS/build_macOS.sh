#!/bin/bash

set -o errexit -o nounset

APP='iQPuzzle'

# Hold on to current directory
project_dir=$(pwd)

# Output macOS version
sw_vers

# Update platform (enable if needed)
#echo "Updating platform..."
#brew update

# Install p7zip for packaging
brew install p7zip

# Install Qt
echo "Installing Qt..."
brew install qt

# Add Qt binaries to path
PATH=/usr/local/opt/qt/bin/:${PATH}

# Build app
echo "Building..."
cd ${project_dir}
mkdir build
cd build
qmake ../iqpuzzle.pro CONFIG+=release
make

# Build and run tests here

# Package
echo "Packaging..."

# Remove build directories that should not be deployed
rm -rf .moc
rm -rf .obj
rm -rf .qrc

echo "Creating dmg archive..."
macdeployqt "${APP}.app" -dmg
mv "${APP}.dmg" "${APP}_${REV_NAME}.dmg"

# Copy other project files
cp "${project_dir}/README.md" "${project_dir}/build/README.md"
cp "${project_dir}/COPYING" "${project_dir}/build/COPYING"

echo "Packaging zip archive..."
7z a ${APP}_${REV_NAME}_macOS.zip "${APP}_${REV_NAME}.dmg" "README.md" "COPYING"

echo "Uploading..."
brew install ffsend
ffsend upload ${APP}_${REV_NAME}_macOS.zip
ffsend upload ${APP}_${REV_NAME}_macOS.zip
ffsend upload ${APP}_${REV_NAME}_macOS.zip
##curl --upload-file ${APP}_${REV_NAME}_macOS.zip https://transfer.sh/${APP}_${REV_NAME}_macOS.zip
echo ""

echo "Done!"

exit 0