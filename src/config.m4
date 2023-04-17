PHP_ARG_ENABLE(crack2, whether to enable Crack support,
[ --with-crack2=DIR   Include crack support via libcrack2])

if test "$PHP_CRACK2" != "no"; then
  SEARCH_PATH="/usr/local /usr"
  if test -r $PHP_CRACK2/include/crack.h || test -r $PHP_CRACK2/crack.h; then
    CRACK2_DIR=$PHP_CRACK2
  else
    AC_MSG_CHECKING([for crack files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/include/crack.h || test -r $i/crack.h; then
        CRACK2_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi

  if test -z "$CRACK2_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libcrack2 distribution])
  fi

  if test -r "$CRACK2_DIR/include/crack.h"; then
      PHP_ADD_INCLUDE($CRACK2_DIR/include)
  else
      PHP_ADD_INCLUDE($CRACK2_DIR)
  fi

  LIBNAME=crack
  LIBSYMBOL=FascistCheck

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CRACK2_DIR/lib, CRACK2_SHARED_LIBADD)
    AC_DEFINE(HAVE_CRACK2LIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong crack lib version or lib not found])
  ],[
    -L$CRACK2_DIR/lib
  ])

  PHP_SUBST(CRACK2_SHARED_LIBADD)

  PHP_NEW_EXTENSION(crack2, crack2.c, $ext_shared)
fi