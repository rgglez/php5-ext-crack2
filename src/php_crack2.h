/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rodolfo González González                                   |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_CRACK2_H
#define PHP_CRACK2_H 1

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(crack2)
    char *_last_message;
    char *_ini_path;
ZEND_END_MODULE_GLOBALS(crack2)

#ifdef ZTS
#define CRACK2_G(v) TSRMG(crack2_globals_id, zend_crack2_globals *, v)
#else
#define CRACK2_G(v) (crack2_globals.v)
#endif

#define PHP_CRACK2_VERSION "0.1"
#define PHP_CRACK2_EXTNAME "crack2"

#ifdef PHP_WIN32
#define PHP_CRACK2_API __declspec(dllexport)
#else
#define PHP_CRACK2_API
#endif

PHP_MINIT_FUNCTION(crack2);
PHP_MSHUTDOWN_FUNCTION(crack2);
PHP_RINIT_FUNCTION(crack2);
PHP_MINFO_FUNCTION(crack2);

PHP_FUNCTION(crack2_fascist_check);
PHP_FUNCTION(crack2_last_message);
PHP_FUNCTION(crack2_compiled_dictionary);
PHP_FUNCTION(crack2_default_dictionary);

extern zend_module_entry crack2_module_entry;
#define phpext_crack2_ptr &crack2_module_entry

#define CRACK2_OK "OK"
#define CRACK2_NODICT "can not open dictionary"

#endif