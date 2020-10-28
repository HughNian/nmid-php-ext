/* workerext extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_workerext.h"
#include "whash.h"
#include <nmidworker.h>

zend_class_entry *worker_ce;

zend_object_handlers nmidext_handlers;

ZEND_DECLARE_MODULE_GLOBALS(workerext);

typedef struct _nmid_worker {
	Worker *w;
	zend_object std;
} nmid_worker;

static nmid_worker *
nmidext_fetch_object(zend_object *obj)
{
	return (nmid_worker *)((char *)obj - nmidext_handlers.offset);
}

void
nmidext_set_workerp(zval *zobject, Worker *w)
{
	nmidext_fetch_object(Z_OBJ_P(zobject))->w = w;
}

static Worker *
nmidext_get_workerp(zval *zobject)
{
	return nmidext_fetch_object(Z_OBJ_P(zobject))->w;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_extworker___construct, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_extworker_add_server, 0, 0, 2)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_extworker_add_function, 0, 0, 2)
	ZEND_ARG_INFO(0, fname)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_extworker_del_function, 0, 0, 1)
	ZEND_ARG_INFO(0, fname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_extworker_run, 0, 0, 1)
ZEND_END_ARG_INFO()

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

RetStruct *
doCallBack(Response *Res)
{
	assert(Res);

	int paramsNum = 0;
	char *fname = NULL;
	zval *cb = NULL;
	RetStruct *ret = NewRetStruct();

	fname = Res->handle;
	if(NULL == fname) {
		php_error_docref(NULL, E_ERROR, "fname not found\n");
		ret->code = 100;
		ret->msg  = "fname not found";
		ret->data = '\0';

		return ret;
	}

	hash_lookup(WORKEREXT_G(callbacks), fname, (void **)&cb);
	if(NULL == cb) {
		php_error_docref(NULL, E_ERROR, "callback not found\n");
		ret->code = 101;
		ret->msg  = "callback not found";
		ret->data = '\0';

		return ret;
	}

	paramsNum = Res->paramsNum;
	if(paramsNum == 0) {
		php_error_docref(NULL, E_ERROR, "params num err\n");
		ret->code = 102;
		ret->msg  = "params num err";
		ret->data = '\0';

		return ret;
	}

	//获取用户function的结果数据
	int i = 0;
	zval rv, retval;
	zend_array params;
	zend_hash_init(&params, paramsNum, NULL, NULL, 0);
	for(; i < paramsNum; i++) {
		zend_string *val = zend_string_init(Res->paramsArr[i], strlen(Res->paramsArr[i]), 0);
		ZVAL_STR(&rv, val);
		zend_hash_index_add(&params, i, &rv);
	}
	zval call_params[1];
	zval cparams;
	ZVAL_ARR(&cparams, &params);
	ZVAL_COPY(&call_params[0], &cparams);
	//ZVAL_COPY_VALUE(&call_params[0], &cparams);
	if(SUCCESS != call_user_function_ex(EG(function_table), NULL, cb, &retval, 1, call_params, 0, NULL TSRMLS_CC)) {
		php_error_docref(NULL, E_ERROR, "call user function err");
		ret->code = 103;
		ret->msg  = "call user function err";
		ret->data = '\0';

		return ret;
	}

	if(IS_STRING != Z_TYPE(retval)) {
		php_error_docref(NULL, E_ERROR, "ret not string\n");
		ret->code = 104;
		ret->msg  = "params num err";
		ret->data = '\0';

		return ret;
	}

	//返回结果数据给nmid服务端
	zend_string *data;
	//data = zval_get_string(&call_func_ret);
	data = Z_STR_P(&retval);
	ret->code = 0;
	ret->msg  = "OK";
	ret->data = ZSTR_VAL(data);

	return ret;
}

PHP_METHOD(workerext, __construct)
{
	zval *zworker = ZEND_THIS;

	Worker *worker = NewWorker();

	WORKEREXT_G(callbacks) = hash_alloc(CALLBACK_HASH_SIZE);
	
	nmidext_set_workerp(zworker, worker TSRMLS_CC);
}

PHP_METHOD(workerext, add_server)
{
	char *host;
	size_t hostl;
	long port = 0;
	Worker *w;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &host, &hostl, &port) == FAILURE) {
		RETURN_FALSE;
	}

	w = nmidext_get_workerp(ZEND_THIS);
	if(UNEXPECTED(!w)) {
		php_error_docref(NULL, E_ERROR, "Invaild instance of %s", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
	}

	if(AddServer(w, host, (int)port) == 0) {
		perror("worker add server err\n");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

PHP_METHOD(workerext, add_function)
{
	char *fname;
	size_t fnamel;
	zval *cb = NULL;
	Worker *w;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &fname, &fnamel, &cb) == FAILURE) {
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

	w = nmidext_get_workerp(ZEND_THIS);
	if(UNEXPECTED(!w)) {
		php_error_docref(NULL, E_ERROR, "Invaild instance of %s", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_FALSE;
	}

	AddFunction(w, fname, doCallBack);

	int fname_len = sizeof(fname)-1;
	zend_update_property(worker_ce, getThis(), fname, fname_len, cb TSRMLS_CC);
	hash_insert(WORKEREXT_G(callbacks), fname,
		n_zend_read_property(worker_ce, getThis(), fname, fname_len, 0 TSRMLS_CC));
}

PHP_METHOD(workerext, del_function)
{
	char *fname;
	size_t fnamel;
	zval *cb = NULL;
	Worker *w;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fnamel) == FAILURE) {
		RETURN_FALSE;
	}

	w = nmidext_get_workerp(ZEND_THIS);
	if(UNEXPECTED(!w)) {
		php_error_docref(NULL, E_ERROR, "Invaild instance of %s", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_FALSE;
	}

	DelFunction(w, fname);

	hash_remove(WORKEREXT_G(callbacks), fname, (void **)&cb);

	RETURN_TRUE;
}

PHP_METHOD(workerext, run)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Worker *w;

	w = nmidext_get_workerp(ZEND_THIS);

	if(Ready(w) == 0) {
		php_error_docref(NULL, E_WARNING, "worker ready err\n");
		RETURN_FALSE;
	}

	if(Work(w) == 0) {
		php_error_docref(NULL, E_WARNING, "worker work err\n");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* {{{ void workerext_test1()
 */
PHP_FUNCTION(workerext_test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "workerext");
}
/* }}} */

/* {{{ string workerext_test2( [ string $var ] )
 */
PHP_FUNCTION(workerext_test2)
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
PHP_RINIT_FUNCTION(workerext)
{
#if defined(ZTS) && defined(COMPILE_DL_WORKEREXT)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(workerext)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "workerext support", LOGO_IMG"enabled");
	php_info_print_table_row(2, "Version", PHP_WORKEREXT_VERSION);
	php_info_print_table_row(2, "Author", "hughnian");
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_workerext_test1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_workerext_test2, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ workerext_functions[]
 */
static const zend_function_entry workerext_functions[] = {
	PHP_FE(workerext_test1,		arginfo_workerext_test1)
	PHP_FE(workerext_test2,		arginfo_workerext_test2)
	PHP_FE_END
};
/* }}} */

zend_function_entry workerext_methods[] = {
	PHP_ME(workerext, __construct, arginfo_extworker___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR|ZEND_ACC_FINAL)
	PHP_ME(workerext, add_server, arginfo_extworker_add_server, ZEND_ACC_PUBLIC)
	PHP_ME(workerext, add_function, arginfo_extworker_add_function, ZEND_ACC_PUBLIC)
	PHP_ME(workerext, del_function, arginfo_extworker_del_function, ZEND_ACC_PUBLIC)
	PHP_ME(workerext, run, arginfo_extworker_run, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* {{{ workerext_module_entry
 */
zend_module_entry workerext_module_entry = {
	STANDARD_MODULE_HEADER,
	"workerext",					/* Extension name */
	workerext_functions,			/* zend_function_entry */
	PHP_MINIT(workerext),			/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(workerext),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(workerext),			/* PHP_MINFO - Module info */
	PHP_WORKEREXT_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

static void php_workerext_init_globals(zend_workerext_globals *workerext_globals)
{
	workerext_globals->callbacks = NULL;
}

PHP_MINIT_FUNCTION(workerext)
{
	ZEND_INIT_MODULE_GLOBALS(workerext, php_workerext_init_globals, NULL);

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "WorkerExt", workerext_methods);
	worker_ce = zend_register_internal_class(&ce);

	return SUCCESS;
}

#ifdef COMPILE_DL_WORKEREXT
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(workerext)
#endif

