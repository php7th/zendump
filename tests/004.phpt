--TEST--
zendump_symbols() tests
--SKIPIF--
<?php if (!extension_loaded("zendump")) print "skip"; ?>
--FILE--
<?php

function func01() {
	$a = ['hello, world'];
	$b = 'hello';
	$$b = 'world';
	zendump_symbols();
}
func01();

?>
===DONE===
--EXPECTF--
symbols(3): {
  "a" =>
  zval(0x%x) -> zval(0x%x) -> array(1) addr(0x%x) refcount(%d) hash(2,0) bucket(8,1) data(0x%x)
  {
    [0] =>
    zval(0x%x) -> string(12,"hello, world") addr(0x%x) refcount(%d)
  }
  "b" =>
  zval(0x%x) -> zval(0x%x) -> string(5,"hello") addr(0x%x) refcount(%d)
  "hello" =>
  zval(0x%x) -> string(5,"world") addr(0x%x) refcount(%d)
}
===DONE===
