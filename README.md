
## OSX

### Get Dependencies

    brew install qt

### Build for Debugging 

    qmake ARCH=x86 LINK=STATIC && make

### Build for Releasing

    qmake ARCH=x86 LINK=STATIC -config release && make

### Install Release

    rsync -av --delete bin/KeePassX.app/ /Applications/KeePassX.app


