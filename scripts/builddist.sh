# build file to generate the distribution binary tarball
autoreconf

# build windows installation file
./configure \
  $PKGCFG \
  --host=i686-w64-mingw32.static \
  --with-ptw32=$PREFIX/i686-w64-mingw32.static \
  --with-libiconv-prefix=$PREFIX/iconv \
  --enable-static \
  PTW32_LIBS="-lpthread -lpcreposix -lpcre" \
  FLTK_CONFIG=$PREFIX/i686-w64-mingw32.static/bin/fltk-config

make -j 8
$PREFIX/bin/i686-w64-mingw32.static-strip src/fllog.exe
make nsisinst
mv src/*setup*exe .

# build the distribution tarball

make clean

./configure

make distcheck
make clean
