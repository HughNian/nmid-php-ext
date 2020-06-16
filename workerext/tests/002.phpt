--TEST--
workerext_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('workerext')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = workerext_test1();

var_dump($ret);
?>
--EXPECT--
The extension workerext is loaded and working!
NULL
