#!/bin/bash
#This is a shell script created by hiccup01 to install kea on Mac OSX.
#The first digit of the version number will always be the first digit of the browser version number.
VERSION="0.0.1" #Installer version
BVERSION="0.2.3" #Browser version
echo "Thanks for installing kea"
echo "You are running Mac OSX kea installer version $VERSION for kea version $BVERSION."
echo "This installer was created by hiccup01 (hiccup01.com)"
sleep 1 
echo "At some points you may be prompted to enter your password. Type it and press \"enter\". (It will not display your password on screen)"
sleep 3
echo "Attempting to install brew package mananger, if brew is already installed this will have no effect"
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
echo "Installing kea dependencies"
brew install webkitgtk unp wget
read -p "What directory would like to install kea in? The default and recommended is ~" DIRECTORY #Ask for user input
if [ -n "$DIRECTORY" ]; then
	echo "Installing in directory $DIRECTORY"
else
	echo "Defaulting. Installing in ~"
	$DIRECTORY="~"
fi
cd $DIRECTORY
echo "Creating directory."
mkdir "kea"
cd kea
echo "Downloading kea."
wget https://github.com/Jonathan50/kea-browser/releases/download/v$BVERSION/kea-$BVERSION.tar.gz
echo "Unziping."
unp kea-$BVERSION.tar.gz
cd kea-$BVERSION
echo "Configuring."
./configure
echo "making and installing"
make && sudo make install
echo "kea is installed. Run \"kea\" to run it."
