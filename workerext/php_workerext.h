/* workerext extension for PHP */

#ifndef PHP_WORKEREXT_H
# define PHP_WORKEREXT_H

extern zend_module_entry workerext_module_entry;
# define phpext_workerext_ptr &workerext_module_entry

# define PHP_WORKEREXT_VERSION "0.1.0"

#define CALLBACK_HASH_SIZE 512

#define NMID_LOGO_BASE64 "data:image/png;base64,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"

#define SUPPORT_URL "http://www.niansong.top"

#define LOGO_IMG "<a href=" SUPPORT_URL "> \
				          <img src=\"" NMID_LOGO_BASE64 "\" alt=\"nmid logo\" /></a>\n"

# if defined(ZTS) && defined(COMPILE_DL_WORKEREXT)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#ifndef ZEND_THIS
#define ZEND_THIS (&EX(This))
#endif

ZEND_BEGIN_MODULE_GLOBALS(workerext)
    struct _sHashTable *callbacks;
ZEND_END_MODULE_GLOBALS(workerext)

PHP_MINIT_FUNCTION(workerext);
PHP_MSHUTDOWN_FUNCTION(workerext);
PHP_RINIT_FUNCTION(workerext);
PHP_RSHUTDOWN_FUNCTION(workerext);
PHP_MINFO_FUNCTION(workerext);

#ifdef ZTS
#include "TSRM.h"
#define WORKEREXT_G(v) TSRMG(workerext_globals_id, zend_workerext_globals*, v)
extern int workerext_globals_id;
#else
#define WORKEREXT_G(v) (workerext_globals.v)
extern zend_workerext_globals workerext_globals;
#endif

static
zval* n_zend_read_property(zend_class_entry *ce, zval *obj, const char *s, int len, int silent)
{
    zval rv, *property = zend_read_property(ce, obj, s, len, silent, &rv);
    if(UNEXPECTED(property == &EG(uninitialized_zval)))
    {
        zend_update_property_null(ce, obj, s, len);
        return zend_read_property(ce, obj, s, len, silent, &rv);
    }

    return property;
}

#endif	/* PHP_WORKEREXT_H */

