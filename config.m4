dnl $Id$
dnl config.m4 for extension zendump

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(zendump, for zendump support,
dnl Make sure that the comment is aligned:
dnl [  --with-zendump             Include zendump support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(zendump, whether to enable zendump support,
Make sure that the comment is aligned:
[  --enable-zendump           Enable zendump support])

if test "$PHP_ZENDUMP" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-zendump -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/zendump.h"  # you most likely want to change this
  dnl if test -r $PHP_ZENDUMP/$SEARCH_FOR; then # path given as parameter
  dnl   ZENDUMP_DIR=$PHP_ZENDUMP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for zendump files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ZENDUMP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ZENDUMP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the zendump distribution])
  dnl fi

  dnl # --with-zendump -> add include path
  dnl PHP_ADD_INCLUDE($ZENDUMP_DIR/include)

  dnl # --with-zendump -> check for lib and symbol presence
  dnl LIBNAME=zendump # you may want to change this
  dnl LIBSYMBOL=zendump # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ZENDUMP_DIR/$PHP_LIBDIR, ZENDUMP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ZENDUMPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong zendump lib version or lib not found])
  dnl ],[
  dnl   -L$ZENDUMP_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(ZENDUMP_SHARED_LIBADD)

  PHP_NEW_EXTENSION(zendump, zendump.c function_dump.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
