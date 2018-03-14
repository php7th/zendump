--TEST--
zendump_args() tests
--SKIPIF--
<?php if (!extension_loaded("zendump")) print "skip"; ?>
--FILE--
<?php

function func01($a, $b, $c) {
	zendump_args();
}
func01('hello, world', ['hello'], 10, true);

?>
===DONE===
--EXPECTF--
args(4): {
  zval(0x%x) -> string(12,"hello, world") addr(0x%x) refcount(%d)
  zval(0x%x) -> array(1) addr(0x%x) refcount(%d) hash(2,0) bucket(8,1) data(0x%x)
  {
    [0] =>
    zval(0x%x) -> string(5,"hello") addr(0x%x) refcount(%d)
  }
  zval(0x%x) : long(10)
  zval(0x%x) : true
}
===DONE===
