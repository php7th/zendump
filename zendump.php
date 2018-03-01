<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('zendump')) {
	dl('zendump.' . PHP_SHLIB_SUFFIX);
}
$module = 'zendump';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";
$function = 'zendump_vars';
if (extension_loaded($module)) {
	$str = $function($module);
} else {
	$str = "Module $module is not compiled into PHP";
}
echo "$str\n";

$a = ['hello', 'world', 'if', 'else', 'do', 'while', 'switch', 'case', 'for'];
zendump($a);
$a = ['hello' => 0, 'world' => 1, 'if' => 2, 'else' => 3, 'do' => 4, 'while' => 5, 'switch' => 6, 'case' => 7, 'for' => 8, 'return' => 9, 'char' => 10, 'short' => 11, 'int' => 12];
zendump($a);
$a = ['hello' => 0, 'world'];
zendump($a);
unset($a['hello']);
zendump($a);
$a[] = ['ok'];
zendump($a);
$b = 9;
zendump($b);
$c = &$b;
$d = $b;
$c = 8;
zendump($d);
zendump($c);
zendump($b);
zendump($a[0]);

$f = function() {
	return 0;
};
zendump($f);
// var_dump($a);

$s = "hello";
zendump($s);
zendump_args();
zendump_vars();
zendump_literals();
zendump_symbols();
zendump_opcodes();
?>
