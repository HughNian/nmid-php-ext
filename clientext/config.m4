dnl config.m4 for extension clientext

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(clientext, for clientext support,
dnl Make sure that the comment is aligned:
dnl [  --with-clientext             Include clientext support])

dnl Otherwise use enable:

CFLAGS="$CFLAGS -Wall -lev -lpthread -lmsgpackc -lnmidclient"

PHP_ARG_ENABLE(clientext, whether to enable clientext support,
dnl Make sure that the comment is aligned:
[  --enable-clientext          Enable clientext support], no)

if test "$PHP_CLIENTEXT" != "no"; then
  dnl Write more examples of tests here...

  dnl # get library FOO build options from pkg-config output
  dnl AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  dnl AC_MSG_CHECKING(for libfoo)
  dnl if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists foo; then
  dnl   if $PKG_CONFIG foo --atleast-version 1.2.3; then
  dnl     LIBFOO_CFLAGS=\`$PKG_CONFIG foo --cflags\`
  dnl     LIBFOO_LIBDIR=\`$PKG_CONFIG foo --libs\`
  dnl     LIBFOO_VERSON=\`$PKG_CONFIG foo --modversion\`
  dnl     AC_MSG_RESULT(from pkgconfig: version $LIBFOO_VERSON)
  dnl   else
  dnl     AC_MSG_ERROR(system libfoo is too old: version 1.2.3 required)
  dnl   fi
  dnl else
  dnl   AC_MSG_ERROR(pkg-config not found)
  dnl fi
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, CLIENTEXT_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-clientext -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/clientext.h"  # you most likely want to change this
  dnl if test -r $PHP_CLIENTEXT/$SEARCH_FOR; then # path given as parameter
  dnl   CLIENTEXT_DIR=$PHP_CLIENTEXT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for clientext files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CLIENTEXT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$CLIENTEXT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the clientext distribution])
  dnl fi

  dnl # --with-clientext -> add include path
  dnl PHP_ADD_INCLUDE($CLIENTEXT_DIR/include)

  dnl # --with-clientext -> check for lib and symbol presence
  dnl LIBNAME=CLIENTEXT # you may want to change this
  dnl LIBSYMBOL=CLIENTEXT # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CLIENTEXT_DIR/$PHP_LIBDIR, CLIENTEXT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_CLIENTEXTLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong clientext lib version or lib not found])
  dnl ],[
  dnl   -L$CLIENTEXT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(CLIENTEXT_SHARED_LIBADD)

  dnl # In case of no dependencies
  AC_DEFINE(HAVE_CLIENTEXT, 1, [ Have clientext support ])

  PHP_NEW_EXTENSION(clientext, clientext.c chash.c, $ext_shared)
fi
