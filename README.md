The zendump extension for PHP currently only supported on 64 bit PHP 7.0 and above.
1. Dump variable's value, use the `zendump()` function just like `var_dump()`:
```
<?php
$n = 10;
$d = 3.14159265;
$s = 'Hello, zendump!';
zendump($n, $d, $s);
```
2. Use the `zendump_vars()` function to dump the local variables declared in the calling function's scope:
```
<?php
function func01($a, &$b) {
    $c = true;
    $d = 123;
    zendump_vars();
}
func01([1, 2], 'hello');
```
3. Use the `zendump_opcodes()` function to dump the opcodes of the calling function:
```
<?php
function func02($a, &$b) {
    zendump_opcodes();
    $b += $a;
    return $b;
}
func02(1, 2);
```
4. Use the `zendump_function()` function to view an internal function's basic information or dump the opcodes of a user defined function:
```
<?php
function func03(&$a, $b) {
    $a += $b;
    return $a;
}
zendump_function('array_merge');
zendump_function('func03');
```
5. Use the `zendump_class()` function to view the basic information of the given class specified by the name:
```
<?php
class SimpleClass {
    private $props = [];
    public __get($name) {
        if(isset($this->props[$name])) {
            return $this->props[$name];
        }
        return null;
    }
    public __set($name, $value) {
        $this->props[$name] = $value;
    }
}
zendump_class('ArrayAccess');
zendump_class('SimpleClass');
```
6. Use the `zendump_method()` function to view the information of a class method:
```
<?php
class SecondClass {
    public greeting() {
        echo 'Hello, zendump!' . PHP_EOL;
    }
}
zendump_method('SecondClass', 'greeting');
```
7. Some other functions like `zendump_args()`, `zendump_literals()` and `zendump_symbols()` if you interest in.