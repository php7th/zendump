<?php
trait A {
	function smallTalk() {
		echo 'a';
	}
	function bigTalk() {
		echo 'A';
	}
}
trait B {
	function smallTalk() {
		echo 'b';
	}
	function bigTalk() {
		echo 'B';
	}
}
class SimpleClass {
	use A, B {
		A::smallTalk insteadof B;
		B::bigTalk insteadof A;
		B::smallTalk as talk;
		B::bigTalk as Big;
	}
	private static $count = 0;
	private $props = [];
	function __get($name) {
		if(isset($this->props[$name])) {
			return $this->props[$name];
		}
		return null;
	}
	function __set($name, $value) {
		$this->props[$name] = $value;
	}
}
zendump_class("exception");
zendump_class("iterator");
zendump_class("arrayiterator");
class SecondClass extends SimpleClass {}
zendump_class("secondclass", true);
zendump_class("simpleclass", true);
$o = new SecondClass;
$o->hello = "hello class, object!\n";
echo $o->hello;
zendump_class('a');
zendump($o);
