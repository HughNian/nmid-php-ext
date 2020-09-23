/* clientext extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_clientext.h"
#include "chash.h"
#include <nmidclient.h>

zend_class_entry *client_ce;

zend_object_handlers client_handlers;

ZEND_DECLARE_MODULE_GLOBALS(clientext);

typedef struct _nmid_client {
	Client *c;
	zend_object std;
} nmid_client;

static nmid_client *
client_fetch_object(zend_object *obj)
{
	return (nmid_client *)((char *)obj - client_handlers.offset);
}

void
client_set_clientp(zval *zobject, Client *c)
{
	client_fetch_object(Z_OBJ_P(zobject))->c = c;
}

static Client *
client_get_clientp(zval *zobject)
{
	return client_fetch_object(Z_OBJ_P(zobject))->c;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_client___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_client_connect, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_client_dowork, 0, 0, 3)
	ZEND_ARG_INFO(0, fname)
	ZEND_ARG_INFO(0, params)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_client_close, 0, 0, 1)
ZEND_END_ARG_INFO()

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

void
RespCallback(Response *Res)
{
	char *fname = NULL;
	zval *cb = NULL;

	fname = Res->handle;
	if(NULL == fname) {
		php_error_docref(NULL, E_ERROR, "fname not found\n");

		return;
	}

	hash_lookup(CLIENTEXT_G(callbacks), fname, (void **)&cb);
	if(NULL == cb) {
		php_error_docref(NULL, E_ERROR, "callback not found\n");

		return;
	}

	zval rv, retval;
	zend_array params;
	zval call_params[1];
	zval cparams;
	zend_string *msg, *data;
	zend_hash_init(&params, 3, NULL, NULL, 0);
	//ret code
	if(Res->dataType == PDT_S_RETURN_DATA) {
		ZVAL_LONG(&rv, Res->resultData->code);
	} else {
		ZVAL_LONG(&rv, Res->dataType);
	}
	zend_hash_index_add(&params, 0, &rv);
	//ret msg
	if(Res->dataType == PDT_S_RETURN_DATA) {
		msg = zend_string_init(Res->resultData->msg, strlen(Res->resultData->msg), 0);
	} else {
		msg = zend_string_init("err", strlen("err"), 0);
	}
	ZVAL_STR(&rv, msg);
	zend_hash_index_add(&params, 1, &rv);
	//ret data
	if(Res->dataType == PDT_S_RETURN_DATA) {
		data = zend_string_init(Res->resultData->data, strlen(Res->resultData->data), 0);
	} else {
		data = zend_string_init("empty data", strlen("empty data"), 0);
	}
	ZVAL_STR(&rv, data);
	zend_hash_index_add(&params, 2, &rv);

	ZVAL_ARR(&cparams, &params);
	ZVAL_COPY(&call_params[0], &cparams);

	if(SUCCESS != call_user_function_ex(EG(function_table), NULL, cb, &retval, 1, call_params, 0, NULL TSRMLS_CC)) {
		php_error_docref(NULL, E_ERROR, "call user function err");

		return;
	}
}

PHP_METHOD(clientext, __construct)
{
	char *host;
	size_t hostl;
	long port = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &host, &hostl, &port) == FAILURE) {
		RETURN_FALSE;
	}

	zval *zclient = ZEND_THIS;

	Client *client = NewClient(host, port);

	CLIENTEXT_G(callbacks) = hash_alloc(CALLBACK_HASH_SIZE);

	client_set_clientp(zclient, client TSRMLS_CC);
}

PHP_METHOD(clientext, connect)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Client *c;
	c = client_get_clientp(ZEND_THIS);
	if(UNEXPECTED(!c)) {
		php_error_docref(NULL, E_ERROR, "Invaild instance of %s", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_FALSE;
	}

	if(Connect(c) == 0) {
		printf("client connect failed");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

PHP_METHOD(clientext, dowork)
{
	char *fname;
	size_t fnamel = 0;
	char *params;
	size_t paramsl = 0;
	zval *cb = NULL;
	Client *c;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssz", &fname, &fnamel, &params, &paramsl, &cb) == FAILURE) {
		RETURN_FALSE;
	}

	zend_string *name = NULL;
	if((cb && !Z_ISNULL_P(cb) && !zend_is_callable(cb, 0, &name TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "function '%s' is not callable", ZSTR_VAL(name));
		zend_string_release(name);
		RETURN_FALSE;
	}

	if(name) {
		zend_string_release(name);
		name = NULL;
	}

	c = client_get_clientp(ZEND_THIS);
	if(UNEXPECTED(!c)) {
		php_error_docref(NULL, E_ERROR, "Invaild instance of %s", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_FALSE;
	}

	int fname_len = sizeof(fname)-1;
	zend_update_property(client_ce, getThis(), fname, fname_len, cb TSRMLS_CC);
	hash_insert(CLIENTEXT_G(callbacks), fname,
		n_zend_read_property(client_ce, getThis(), fname, fname_len, 0 TSRMLS_CC));

	Do(c, fname, params, strlen(params), RespCallback);

	RETURN_TRUE;
}

PHP_METHOD(clientext, close)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Client *c;
	c = client_get_clientp(ZEND_THIS);
	if(UNEXPECTED(!c)) {
		php_error_docref(NULL, E_ERROR, "Invaild instance of %s", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_FALSE;
	}

	Close(c);

	RETURN_TRUE;
}

/* {{{ void clientext_test1()
 */
PHP_FUNCTION(clientext_test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "clientext");
}
/* }}} */

/* {{{ string clientext_test2( [ string $var ] )
 */
PHP_FUNCTION(clientext_test2)
{
	char *var = "World";
	size_t var_len = sizeof("World") - 1;
	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(var, var_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = strpprintf(0, "Hello %s", var);

	RETURN_STR(retval);
}
/* }}}*/

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(clientext)
{
#if defined(ZTS) && defined(COMPILE_DL_CLIENTEXT)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(clientext)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "clientext support", LOGO_IMG"enabled");
	php_info_print_table_row(2, "Version", "1.0.1");
	php_info_print_table_row(2, "Author", "hughnian");
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_clientext_test1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_clientext_test2, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ clientext_functions[]
 */
static const zend_function_entry clientext_functions[] = {
	PHP_FE(clientext_test1,		arginfo_clientext_test1)
	PHP_FE(clientext_test2,		arginfo_clientext_test2)
	PHP_FE_END
};
/* }}} */

zend_function_entry clientext_methods[] = {
		PHP_ME(clientext, __construct, arginfo_client___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR|ZEND_ACC_FINAL)
		PHP_ME(clientext, connect, arginfo_client_connect, ZEND_ACC_PUBLIC)
		PHP_ME(clientext, dowork, arginfo_client_dowork, ZEND_ACC_PUBLIC)
		PHP_ME(clientext, close, arginfo_client_close, ZEND_ACC_PUBLIC)
		PHP_FE_END
};

/* {{{ clientext_module_entry
 */
zend_module_entry clientext_module_entry = {
	STANDARD_MODULE_HEADER,
	"clientext",					/* Extension name */
	clientext_functions,			/* zend_function_entry */
	PHP_MINIT(clientext),			/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(clientext),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(clientext),			/* PHP_MINFO - Module info */
	PHP_CLIENTEXT_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

static void php_clientext_init_globals(zend_clientext_globals *clientext_globals)
{
	clientext_globals->callbacks = NULL;
}

PHP_MINIT_FUNCTION(clientext)
{
	ZEND_INIT_MODULE_GLOBALS(clientext, php_clientext_init_globals, NULL);

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "ClientExt", clientext_methods);
	client_ce = zend_register_internal_class(&ce);

	return SUCCESS;
}

#ifdef COMPILE_DL_CLIENTEXT
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(clientext)
#endif

