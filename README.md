# Kea Browser

[![Join the chat at https://gitter.im/Jonathan50/kea-browser](https://badges.gitter.im/Jonathan50/kea-browser.svg)](https://gitter.im/Jonathan50/kea-browser?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge). 
A lightweight WebKit-based browser for the web  

## Installation
Get the latest release from [here](https://github.com/Jonathan50/kea-browser/releases).  
If binaries are available for your platform you may want to download them instead.  
### Building from source
You should get the `kea-x.y.z.tar.gz` file instead of GitHub's generated archive \(unless you want to run Autotools\).  
#### \*nix/GTK+
You need to have WebKitGTK and GTK+ 3 installed.  
Then:
```sh
$ ./configure
$ make && sudo make install
```
### OS X
Run the automated shell script with this command
```sh
curl https://raw.githubusercontent.com/Jonathan50/kea-browser/master/macosxinstaller.sh | sh
```
#### Windows
TBA  
Windows currently not supported. Use GTK+ in the mean time.
