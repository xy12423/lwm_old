<?php
	function myErrHandler($error_level, $error_message){}
	
	set_error_handler('myErrHandler', E_NOTICE);
	$con = mysql_connect("localhost", "wm", "123");
	if (!$con) die(mysql_error());
	$result = mysql_query("CREATE DATABASE workmanager", $con);
	if (!$result) die(mysql_error());
	$result = mysql_select_db("workmanager", $con);
	if (!$result) die(mysql_error());
	$result = mysql_query("CREATE TABLE data (name text, count int)", $con);
	if (!$result) die(mysql_error());
	$result = mysql_query("INSERT INTO data (name, count) VALUES ('group', 0)", $con);
	if (!$result) die(mysql_error());
	$result = mysql_query("INSERT INTO data (name, count) VALUES ('member', 0)", $con);
	if (!$result) die(mysql_error());
	$result = mysql_query("INSERT INTO data (name, count) VALUES ('work', 0)", $con);
	if (!$result) die(mysql_error());
	$result = mysql_query("CREATE TABLE `group` (id int, name text, member text)", $con);
	if (!$result) die(mysql_error());
	$result = mysql_query("CREATE TABLE member (id int, name text, `group` text, work text, src text, info text)", $con);
	if (!$result) die(mysql_error());
	$result = mysql_query("CREATE TABLE work (id int, name text, info text, member text)", $con);
	if (!$result) die(mysql_error());
	mysql_close($con);
?>
