#!/bin/bash
#This is a shell script created by hiccup01 to install kea on Mac OSX.
#The first digit of the version number will always be the first digit of the browser version number.
VERSIONTEXT="You are running Mac OSX kea installer version $VERSION written by hiccup01 for kea version $BVERSION."
CVERSION= curl https://raw.githubusercontent.com/Jonathan50/kea-browser/master/version.txt #File with current version in it.
VERSION="0.0.3" #Installer version.
BVERSION="0.2.3" #Browser version that the installer has been tested for.
BVERSIONSAFE="023"
echo "Thanks for installing kea"
echo $VERSIONTEXT
echo "This installer was created by hiccup01 (hiccup01.com)"
if [[ "X$CVERSION" -gt "X$BVERSIONSAFE" ]]; then
	echo "This version of the kea installer have not been tested with the latest version."
	echo "Use the latest version? [y,N](Type the version of a kea release to install it.)"
read input
if [ $input == "Y" || $input == "y" || $input == "yes" ]
then
  echo "Using latest version..."
elif [ $input == "N" || $input == "n" || $input == "no" ]
then
  echo "Using safe version..."
else
  echo "Using version $input"
  BVERSION="$input"
fi
fi
sleep 1 
echo "At some points you may be prompted to enter your password. Type it and press \"enter\". (It will not display your password on screen)"
sleep 3
echo "Attempting to install brew package mananger, if brew is already installed this will have no effect"
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
echo "Installing kea dependencies"
brew install webkitgtk unp wget gnu-tar 
read -p "What directory would like to install kea in? The default and recommended is ~" DIRECTORY #Ask for user input
if [ -n "$DIRECTORY" ]
then
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
rm kea-$BVERSION.tar.gz
cd kea-$BVERSION
echo "Configuring."
./configure
echo "making and installing"
make && sudo make install
echo "kea is installed. Run \"kea\" to run it."
