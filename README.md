
## OSX

### Get Dependencies

    brew install qt

### Build for Debugging 

    qmake ARCH=x86 LINK=STATIC && make

### Build for Release 

    qmake ARCH=x86 LINK=STATIC -config release && make

    # install with:  `cp -ai bin/KeePassX.app /Applications`


