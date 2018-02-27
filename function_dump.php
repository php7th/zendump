<?php
$a = 9;
$b = $a;
zendump_vars();
$c = &$a;
zendump_vars();
unset($a);
zendump_vars();
$d = $c;
zendump_vars();
$c = 0;
zendump_vars();
zendump_literals();
zendump_opcodes();
function func($p) {
	$n = 20;
	$m = &$n;
	$l = 3.14159265;
	$s = "hello function!";
	$a = [$n, $l, $s];
	if("" == $s) {
		echo "equal!";
	}
	echo $a[2];
	zendump_vars();
	zendump_literals();
	// zendump_opcodes(50);
}
func(100);
function func02(&$a, $b, $c) {
	zendump_vars();
	$d = $b;
	if($c == $d) {
		echo $d . $a;
	}
	$e = &$a;
	$e = "here";
	while($c) {
		echo $c--;
	}
	zendump_vars();
	zendump_literals();
	zendump_opcodes();
	return 2;
}
func02($b, $c, $d);
zendump_function("func", 50);
none($c);
?>
