--TEST--
zendump_statics() tests
--SKIPIF--
<?php if (!extension_loaded("zendump")) print "skip"; ?>
--FILE--
<?php

function func01() {
	zendump_statics();
	static $a = ['hello, world'];
	zendump_statics();
}
func01();

?>
===DONE===
--EXPECTF--
statics(1): {
  "a" =>
  zval(0x%x) -> array(1) addr(0x%x) refcount(%d) hash(2,0) bucket(8,1) data(0x%x)
  {
    [0] =>
    zval(0x%x) -> string(12,"hello, world") addr(0x%x) refcount(%d)
  }
}
statics(1): {
  "a" =>
  zval(0x%x) -> reference(2) addr(0x%x) zval(0x%x) -> array(1) addr(0x%x) refcount(%d) hash(2,0) bucket(8,1) data(0x%x)
  {
    [0] =>
    zval(0x%x) -> string(12,"hello, world") addr(0x%x) refcount(%d)
  }
}
===DONE===
