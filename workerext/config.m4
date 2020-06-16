dnl config.m4 for extension workerext

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(workerext, for workerext support,
dnl Make sure that the comment is aligned:
dnl [  --with-workerext             Include workerext support])

dnl Otherwise use enable:

CFLAGS="$CFLAGS -Wall -lev -lpthread -lmsgpackc -lnmidworker"

PHP_ARG_ENABLE(workerext, whether to enable workerext support,
dnl Make sure that the comment is aligned:
[  --enable-workerext          Enable workerext support], no)

if test "$PHP_WORKEREXT" != "no"; then
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
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, WORKEREXT_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-workerext -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/workerext.h"  # you most likely want to change this
  if test -r $PHP_WORKEREXT/$SEARCH_FOR; then # path given as parameter
    WORKEREXT_DIR=$PHP_WORKEREXT
  else # search default path list
    AC_MSG_CHECKING([for workerext files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        WORKEREXT_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  dnl if test -z "$WORKEREXT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the workerext distribution])
  dnl fi

  dnl # --with-workerext -> add include path
  dnl PHP_ADD_INCLUDE($WORKEREXT_DIR/include)

  dnl # --with-workerext -> check for lib and symbol presence
  dnl LIBNAME=WORKEREXT # you may want to change this
  dnl LIBSYMBOL=WORKEREXT # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $WORKEREXT_DIR/$PHP_LIBDIR, WORKEREXT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_WORKEREXTLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong workerext lib version or lib not found])
  dnl ],[
  dnl   -L$WORKEREXT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(WORKEREXT_SHARED_LIBADD)

  dnl # In case of no dependencies
  AC_DEFINE(HAVE_WORKEREXT, 1, [ Have workerext support ])

  PHP_NEW_EXTENSION(workerext, workerext.c whash.c, $ext_shared)
fi
