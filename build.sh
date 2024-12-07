CC="zig cc"
CFLAGS="-Wall -Wextra -pedantic"
SRC="main.c youtube.c"
PKG="-I./include -lportaudio"

EXE_NAME="apcli"

echo "$CC $CFLAGS $PKG -o $EXE_NAME $SRC"
$CC $CFLAGS $PKG -o $EXE_NAME $SRC
