--TEST--
Check if workerext is loaded
--SKIPIF--
<?php
if (!extension_loaded('workerext')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "workerext" is available';
?>
--EXPECT--
The extension "workerext" is available
