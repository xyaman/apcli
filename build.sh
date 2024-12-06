CC="zig cc"
CFLAGS="-Wall -Wextra -pedantic"
SRC="main.c"
PKG="-lportaudio"

EXE_NAME="apcli"

$CC $CFLAGS $PKG -o $EXE_NAME $SRC
