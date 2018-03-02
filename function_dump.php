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
	if("" != $s) {
		echo "not equal!";
	}
	echo $a[2];
	zendump_args();
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
	eval('function func03(&$a, $b){$a += $b;return $a;}');
	zendump_args();
	zendump_vars();
	zendump_literals();
	zendump_opcodes();
	return 2;
}
func02($b, $c, $d, 4, 5, 6, '7', 'eight', 9.0);
zendump_function("func", 50);
zendump_function("func03", 50);
zendump_function("zendump_function");
zendump_function("zendump");
func03($d, 200);
zendump_vars();
function &func04(array $a, bool $b, int $c, \Exception $e, ...$params) {
	$a[] = [$b, $c];
	$a[2] = 0;
	zendump_vars();
	return $a;
}
zendump_function('func04');
func04([], '', 3, new Exception, 5);
eval('function func05(array $a, bool $b, int $c) {$a[] = [$b, $c];zendump_vars();return $a;}');
func05([], '', 0);
?>
