--TEST--
zendump() tests
--SKIPIF--
<?php if (!extension_loaded("zendump")) print "skip"; ?>
--FILE--
<?php

zendump();
zendump(null);
zendump(true);
zendump(false);
zendump(10);
zendump(3.14159265);

zendump("");
zendump([]);

?>
===DONE===
--EXPECTF--
Warning: zendump() expects at least 1 parameter, 0 given in %s on line %d
zval(0x%x) : null
zval(0x%x) : true
zval(0x%x) : false
zval(0x%x) : long(10)
zval(0x%x) : double(3.14159265) hex(400921fb53c8d4f1)
zval(0x%x) -> string(0,"") addr(0x%x) refcount(%d)
zval(0x%x) -> array(0) addr(0x%x) refcount(%d) hash(2,0) bucket(8,0) data(0x%x)
===DONE===
