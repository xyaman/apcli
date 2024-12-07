CC="zig cc"
CFLAGS="-Wall -Wextra -pedantic"
SRC="youtube.c"
PKG=""

EXE_NAME="yttest"

$CC $CFLAGS $PKG -o $EXE_NAME $SRC

./yttest; rm ./yttest
