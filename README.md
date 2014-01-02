
## OSX

### Get Dependencies

    brew install qt

### Build for Debugging 

    qmake ARCH=x86 LINK=STATIC && make

### Build for Releasing

    
    qmake ARCH=x86 LINK=STATIC -config release && make

### Build Release

    ./build_dmg

### Install Release

    rsync -av --delete bin/KeePassX.app/ /Applications/KeePassX.app

## Return codebase to pristine

    make distclean && git clean -dff && git clean -Xff
