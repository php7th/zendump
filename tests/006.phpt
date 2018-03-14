--TEST--
zendump_literals() tests
--SKIPIF--
<?php if (!extension_loaded("zendump")) print "skip"; ?>
--FILE--
<?php

$a = 'hello, world';
$b = ['hello'];
$c = 10;
$d = true;
zendump_literals();

?>
===DONE===
--EXPECTF--
literals(7): {
  zval(0x%x) -> string(12,"hello, world") addr(0x%x) refcount(%d)
  zval(0x%x) -> array(1) addr(0x%x) refcount(%d) hash(2,0) bucket(8,1) data(0x%x)
  {
    [0] =>
    zval(0x%x) -> string(5,"hello") addr(0x%x) refcount(%d)
  }
  zval(0x%x) : long(10)
  zval(0x%x) : true
  zval(0x%x) -> string(16,"zendump_literals") addr(0x%x) refcount(%d)
  zval(0x%x) -> string(11,"===DONE===\n") addr(0x%x) refcount(%d)
  zval(0x%x) : long(1)
}
===DONE===
