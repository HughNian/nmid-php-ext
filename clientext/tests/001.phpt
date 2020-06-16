--TEST--
Check if clientext is loaded
--SKIPIF--
<?php
if (!extension_loaded('clientext')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "clientext" is available';
?>
--EXPECT--
The extension "clientext" is available
