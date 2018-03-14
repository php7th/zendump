--TEST--
zendump_vars() tests
--SKIPIF--
<?php if (!extension_loaded("zendump")) print "skip"; ?>
--FILE--
<?php

$a = 10;
$b = 'hello';
zendump_vars();

function func01() {
	$c = ['world'];
	$d = null;
	$e = 20;
	$f = &$e;
	zendump_vars();
}
func01();

?>
===DONE===
--EXPECTF--
vars(2): {
  $a ->
  zval(0x%x) : long(10)
  $b ->
  zval(0x%x) -> string(5,"hello") addr(0x%x) refcount(%d)
}
vars(4): {
  $c ->
  zval(0x%x) -> array(1) addr(0x%x) refcount(%d) hash(2,0) bucket(8,1) data(0x%x)
  {
    [0] =>
    zval(0x%x) -> string(5,"world") addr(0x%x) refcount(%d)
  }
  $d ->
  zval(0x%x) : null
  $e ->
  zval(0x%x) -> reference(2) addr(0x%x) zval(0x%x) : long(20)
  $f ->
  zval(0x%x) -> reference(2) addr(0x%x) zval(0x%x) : long(20)
}
===DONE===
