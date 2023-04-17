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
   | Author: Rodolfo González González                                    |
   +----------------------------------------------------------------------+
 */

#include <crack.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_crack2.h"

ZEND_DECLARE_MODULE_GLOBALS(crack2)

/* {{{ crack2_functions[]
 */
static function_entry crack2_functions[] = {
    PHP_FE(crack2_fascist_check, NULL)
    PHP_FE(crack2_last_message, NULL)
    PHP_FE(crack2_compiled_dictionary, NULL)
    PHP_FE(crack2_default_dictionary, NULL)
    {NULL, NULL, NULL}
};
/* }}} */

/* {{{ crack2_module_entry
 */
zend_module_entry crack2_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_CRACK2_EXTNAME,
    crack2_functions,
    PHP_MINIT(crack2),
    PHP_MSHUTDOWN(crack2),
    PHP_RINIT(crack2),
    NULL,
    PHP_MINFO(crack2),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_CRACK2_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CRACK2
ZEND_GET_MODULE(crack2)
#endif

/* {{{ PHP_INI */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("crack2.default_dictionary", NULL, PHP_INI_ALL, OnUpdateString, _ini_path, zend_crack2_globals, crack2_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_crack2_init_globals
 */
static void php_crack2_init_globals(zend_crack2_globals *crack2_globals)
{
    crack2_globals->_last_message = NULL;
    crack2_globals->_ini_path = NULL;
}
/* }}} */

/* {{{ PHP_MINI_FUNCTION */
PHP_MINIT_FUNCTION(crack2)
{
    ZEND_INIT_MODULE_GLOBALS(crack2, php_crack2_init_globals, NULL);

    REGISTER_INI_ENTRIES();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(crack2)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(crack2)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(crack2)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "crack2 support", "enabled");
    php_info_print_table_row(2, "Version", PHP_CRACK2_VERSION);
    php_info_print_table_row(2, "Build Date", __DATE__);
    php_info_print_table_row(2, "Build Time", __TIME__);
    DISPLAY_INI_ENTRIES();
    php_info_print_table_end();
}
/* }}} */

/******************************************************************************/

/* {{{ php_crack2_checkpath
 */
static int php_crack2_checkpath(char* path TSRMLS_DC)
{
	char *filename;
	int filename_len;
	int result = SUCCESS;

	if (PG(safe_mode)) {
            filename_len = strlen(path) + 10;
            filename = (char *) emalloc(filename_len);
            if (NULL == filename) {
                return FAILURE;
            }

            memset(filename, '\0', filename_len);
            strcpy(filename, path);
            strcat(filename, ".pwd");
            if (!php_checkuid(filename, "r", CHECKUID_CHECK_FILE_AND_DIR)) {
                efree(filename);
                return FAILURE;
            }

            memset(filename, '\0', filename_len);
            strcpy(filename, path);
            strcat(filename, ".pwi");
            if (!php_checkuid(filename, "r", CHECKUID_CHECK_FILE_AND_DIR)) {
                efree(filename);
                return FAILURE;
            }

            memset(filename, '\0', filename_len);
            strcpy(filename, path);
            strcat(filename, ".hwm");
            if (!php_checkuid(filename, "r", CHECKUID_CHECK_FILE_AND_DIR)) {
                efree(filename);
                return FAILURE;
            }

            if (php_check_open_basedir(path TSRMLS_CC)) {
                return FAILURE;
            }
	}
        else {
            filename_len = strlen(path) + 10;
            filename = (char *) emalloc(filename_len);
            if (NULL == filename) {
                return FAILURE;
            }

            memset(filename, '\0', filename_len);
            strcpy(filename, path);
            strcat(filename, ".pwd");
            if (access(filename, R_OK) == -1) {
                efree(filename);
                return FAILURE;
            }

            memset(filename, '\0', filename_len);
            strcpy(filename, path);
            strcat(filename, ".pwi");
            if (access(filename, R_OK) == -1) {
                efree(filename);
                return FAILURE;
            }

            memset(filename, '\0', filename_len);
            strcpy(filename, path);
            strcat(filename, ".hwm");
            if (access(filename, R_OK) == -1) {
                efree(filename);
                return FAILURE;
            }
        }

	return SUCCESS;
}
/* }}} */

/* {{{ proto bool crack_check(string password [, string dictionary_path])
   Performs an obscure check with the given password, using the dictionary
   referenced by dictionary_path.
   Returns true if the password is secure, false otherwise. Returns NULL if the
   test could not be performed (for instance, if the dictionary can not be 
   opened)
*/
PHP_FUNCTION(crack2_fascist_check)
{
    char *msg;

    char *password = NULL;
    int password_len = 0;
    
    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &password, &password_len, &path, &path_len) == FAILURE) {
        RETURN_NULL();
    }

    if (path) {
	if (php_crack2_checkpath(path TSRMLS_CC) == FAILURE) {
            CRACK2_G(_last_message) = estrndup(CRACK2_NODICT, strlen(CRACK2_NODICT));
            RETURN_NULL();
	}
        msg = (char *)FascistCheck(password, path);
    }
    else if (CRACK2_G(_ini_path)) {
	if (php_crack2_checkpath(CRACK2_G(_ini_path) TSRMLS_CC) == FAILURE) {
            CRACK2_G(_last_message) = estrndup(CRACK2_NODICT, strlen(CRACK2_NODICT));
            RETURN_NULL();
	}
        msg = (char *)FascistCheck(password, CRACK2_G(_ini_path));
    }
    else {
        msg = (char *)FascistCheck(password, NULL);
    }
    
    if (msg) {
        CRACK2_G(_last_message) = estrndup(msg, strlen(msg));
        RETURN_BOOL(0);
    }
    else {
        CRACK2_G(_last_message) = estrndup(CRACK2_OK, strlen(CRACK2_OK));
        RETURN_BOOL(1);
    }
}
/* }}} */

/* {{{ proto string crack2_last_message()
   Returns the last message from libcrack2 */
PHP_FUNCTION(crack2_last_message)
{
    if (CRACK2_G(_last_message)) {
        RETURN_STRING(CRACK2_G(_last_message), 0);
    }
    else {
        RETURN_STRING("", 1);
    }
}
/* }}} */

/* {{{ proto string crack2_compiled_dictionary()
   Returns the value of the compiled-in libcrack2 dictionary path */
PHP_FUNCTION(crack2_compiled_dictionary)
{
    char* path;
    path = (char *)GetDefaultCracklibDict();
    RETURN_STRING(path, 1);
}
/* }}} */

/* {{{ proto string crack2_default_dictionary()
   Returns the value of the default dictionary path used by libcrack2 as 
   configured in php.ini
*/
PHP_FUNCTION(crack2_default_dictionary)
{
    if (CRACK2_G(_ini_path)) {
        RETURN_STRING(CRACK2_G(_ini_path), 0);
    }
    else {
        RETURN_STRING("", 1);
    }
}
/* }}} */