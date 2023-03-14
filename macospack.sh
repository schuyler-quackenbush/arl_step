#!/bin/bash

# Package Audio Research Labs with the needed frameworks
# Sign it
# Create a DMG
# Notarize the DMG for macOS security
# Verify Gatekeeper OK with our version
# Upload to our server

# QTDIR should be defined
QTDIR=/Users/srq/Qt/6.2.4/macos
if [ -z ${QTDIR} ]; then
  echo "You need to define QTDIR to Qt location"
  exit
else
  echo "Using Qt: $QTDIR"
fi

# Verify that OpenSSL / PortAudio is installed
if [ ! -f $HOME/local/openssl/lib/libcrypto.a ]; then
  echo "******** Missing OpenSSL in $HOME/local/openssl"
  echo "Use 3rdparty/build_macos_dep.sh to build/install it"
  exit
fi
if [ ! -f $HOME/local/portaudio/lib/libportaudio.a ]; then
  echo "******** Missing PortAudio in $HOME/local/portaudio"
  echo "Use 3rdparty/build_macos_dep.sh to build/install it"
  exit
fi

# Verify that create-dmg is installed
if [ ! -f $HOME/local/create-dmg/bin/create-dmg ]; then
  echo "Missing create-dmg in $HOME/local/create-dmg"
  echo "******** Get the sources and compile it"
  echo "git clone https://github.com/create-dmg/create-dmg.git"
  echo "cd create-dmg && make prefix=$HOME/local/create-dmg install"
  exit
fi

#Apple ID at
#https://appleid.apple.com/account/manage
dev_account="schuyler.quackenbush@me.com"

# Generate an application password
# https://support.apple.com/en-us/HT204397
# Store the application password for your AppleID in keychain
# xcrun notarytool store-credentials --sync --apple-id "david.geldreich@free.fr" --password "xxxx-xxxx-xxxx-xxxx" --team-id "4A95ACP6M5" audio_research_labs
# xcrun altool --store-password-in-keychain-item audio_research_labs --sync -u "david.geldreich@free.fr" -p "xxxx-xxxx-xxxx-xxxx"

dev_keychain_label="audio_research_labs"
signature="Developer ID Application: Schuyler Quackenbush (HZE7F67W4X)"

# Get list of ProviderShortName using
# xcrun altool --list-providers -u "$dev_account" -p "@keychain:$dev_keychain_label"
# xcrun altool --list-providers -u "david.geldreich@free.fr" -p "@keychain:audio_research_labs"
dev_team="HZE7F67W4X" # Audio Research Labs

macdeployqt_tool=$QTDIR/bin/macdeployqt

notarizefile()
{
    filepath=${1:?"need a filepath"}

    # Submit for notarization
    echo "## uploading $filepath for notarization"
    xcrun notarytool submit \
      --keychain-profile "$dev_keychain_label" \
      --timeout "20m" \
      --wait --progress \
      $filepath

    retVal=$?
    if [ $retVal -ne 0 ]; then
          echo "## could not notarize $filepath"
          exit $retVal
    fi
}

rm -rf dist
mkdir dist
pushd dist

cmake -G Xcode -DCMAKE_PREFIX_PATH=$QTDIR ..
cmake --build . -j 12 --config Release --target Step

app_bundle=Step/Release/Step.app

# Bundle Qt frameworks
echo "## Bundle Qt frameworks into $app_bundle"
$macdeployqt_tool $app_bundle

# Sometimes macdeployqt fails to set rpath !!!
rpath_arm64=`otool -arch arm64 -l Step/Release/Step.app/Contents/MacOS/Step  | grep -A 4 RPATH | grep path | awk '{print $2;}'`
rpath_x86_64=`otool -arch x86_64 -l Step/Release/Step.app/Contents/MacOS/Step | grep -A 4 RPATH | grep path | awk '{print $2;}'`

if [ "$rpath_arm64" != "@executable_path/../Frameworks" ]; then
    echo "Incorrect rpath: $rpath_arm64"
    exit
fi

if [ "$rpath_x86_64" != "@executable_path/../Frameworks" ]; then
    echo "Incorrect rpath: $rpath_x86_64"
    exit
fi

# Codesign using hardened runtime
echo "## Codesign $app_bundle using hardened runtime"
codesign --force --timestamp --options runtime --deep \
    --sign "$signature" $app_bundle

# Create a unique filename using current date/time + git revision
#version=$(git describe --dirty | sed -e 's/^v//' -e 's/g//' -e 's/[[:space:]]//g')
version="3.01"
prefix=`date +%Y%m%d%H%M`
filename=${prefix}_${version}_Step.dmg

# Package for notarization
#/usr/bin/ditto -c -k --keepParent QcDemo.app $filename

# Create a DMG using create-dmg tool
# installed from https://github.com/andreyvit/create-dmg
test -f $filename && rm $filename

# Retina background
# https://stackoverflow.com/questions/11199926/create-dmg-with-retina-background-support/11204769#11204769
# tiffutil -cathidpicheck background.png background@2x.png -out background.tiff

rm -rf dmg_content
mkdir dmg_content
rsync -a $app_bundle dmg_content/
# Additional content
# rsync -a $HOME/Desktop/Capture dmg_content/

$HOME/local/create-dmg/bin/create-dmg \
    --volname "Step Installer" \
    --window-pos 200 120 \
    --window-size 800 400 \
    --icon-size 100 \
    --icon "Step.app" 200 190 \
    --hide-extension "Step.app" \
    --app-drop-link 600 185 \
    "$filename" \
    "dmg_content/"

notarizefile $filename

# Include notarization ticket into the DMG and the app
xcrun stapler staple $filename
xcrun stapler staple $app_bundle

# Verify Gatekeeper is OK with our files
spctl -vv -a --type execute $app_bundle
spctl -vv -a --type install $app_bundle
xcrun stapler validate $filename
xcrun stapler validate $app_bundle
