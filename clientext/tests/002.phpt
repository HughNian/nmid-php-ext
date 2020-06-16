--TEST--
clientext_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('clientext')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = clientext_test1();

var_dump($ret);
?>
--EXPECT--
The extension clientext is loaded and working!
NULL
