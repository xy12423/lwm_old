<?php
	function myErrHandler($error_level, $error_message){}
	function decodeStr($str)
	{
		return base64_decode(strtr($str, "-_", "+/"));
	}
	
	set_error_handler('myErrHandler', E_NOTICE);
	if (!isset($_POST["field"]))
		die("E:Field not set");
	if (!isset($_POST["operation"]))
		die("E:Operation not set");
	$con = mysql_connect("localhost", "", "");
	if (!$con) die(mysql_error());
	$result = mysql_select_db("workmanager", $con);
	if (!$result) die(mysql_error());
	switch ($_POST["operation"])
	{
		case "add":
			if (!isset($_POST["name"]))
				die("E:Name not set");
			$type = 0;
			switch ($_POST["field"])
			{
				case "group":
					$type = 1;
					break;
				case "member":
					$type = 2;
					break;
				case "work":
					$type = 3;
					break;
				default:
					die("E:Invalid field");
			}
			switch ($type)
			{
				case 1:
					$result = mysql_query("SELECT * FROM `data` WHERE `name`='group'", $con);
					if (!$result) die(mysql_error());
					$row = mysql_fetch_array($result);
					$newID = $row['count'];
					$result = mysql_query("UPDATE `data` SET `count`=" . ($newID + 1) . " WHERE `name`='group'", $con);
					if (!$result) die(mysql_error());
					$result = mysql_query("INSERT INTO `group` (`id`, `name`) VALUES (" . ($newID) . ",'" . decodeStr($_POST["name"]) . "')", $con);
					if (!$result) die(mysql_error());
					echo $newID;
					break;
				case 2:
					if (!isset($_POST["src"]))
						die("E:Source not set");
					if (!isset($_POST["info"]))
						die("E:Info not set");
					$result = mysql_query("SELECT * FROM `data` WHERE `name`='member'", $con);
					if (!$result) die(mysql_error());
					$row = mysql_fetch_array($result);
					$newID = $row['count'];
					$result = mysql_query("UPDATE `data` SET `count`=" . ($newID + 1) . " WHERE `name`='member'", $con);
					if (!$result) die(mysql_error());
					$result = mysql_query("INSERT INTO `member` (`id`, `name`, `src`, `info`) VALUES (" . ($newID) . ",'" . decodeStr($_POST["name"]) . "','" . decodeStr($_POST["src"]) . "','" . decodeStr($_POST["info"]) . "')", $con);
					if (!$result) die(mysql_error());
					echo $newID;
					break;
				case 3:
					if (!isset($_POST["info"]))
						die("E:Info not set");
					$result = mysql_query("SELECT * FROM `data` WHERE `name`='work'", $con);
					if (!$result) die(mysql_error());
					$row = mysql_fetch_array($result);
					$newID = $row['count'];
					$result = mysql_query("UPDATE `data` SET `count`=" . ($newID + 1) . " WHERE `name`='work'", $con);
					if (!$result) die(mysql_error());
					$result = mysql_query("INSERT INTO `work` (`id`, `name`, `info`) VALUES (" . ($newID) . ",'" . decodeStr($_POST["name"]) . "','" . decodeStr($_POST["info"]) . "')", $con);
					if (!$result) die(mysql_error());
					echo $newID;
					break;
			}
			break;
		case "del":
			if (!isset($_POST["id"]))
				die("E:ID not set");
			switch ($_POST["field"])
			{
				case "group":
				case "member":
				case "work":
					$result = mysql_query("DELETE FROM `" . $_POST["field"] . "` WHERE `id`=" . $_POST["id"], $con);
					if (!$result) die(mysql_error());
					break;
				default:
					die("E:Invalid field");
			}
			break;
		case "edit":
			if (!isset($_POST["id"]))
				die("E:ID not set");
			if (!isset($_POST["item"]))
				die("E:Item not set");
			if (!isset($_POST["value"]))
				die("E:Value not set");
			$_POST["value"] = decodeStr($_POST["value"]);
			switch ($_POST["field"])
			{
				case "group":
				case "member":
				case "work":
					$result = mysql_query("UPDATE `" . $_POST["field"] . "` SET `" . $_POST["item"] . "`='" . $_POST["value"] . "' WHERE `id`=" . $_POST["id"], $con);
					if (!$result) die(mysql_error());
					break;
				default:
					die("E:Invalid field");
			}
			break;
		case "list":
			$result = mysql_query("SELECT * FROM `" . $_POST["field"] . "`", $con);
			if (!$result) die(mysql_error());
			
			switch ($_POST["field"])
			{
				case "group":
					while($row = mysql_fetch_array($result))
					{
						echo $row['id'] . "\n" . $row['name'] . "\n" . $row['member'] . "\n";
					}
					break;
				case "member":
					while($row = mysql_fetch_array($result))
					{
						echo $row['id'] . "\n" . $row['name'] . "\n" . $row['src'] . "\n" . $row['info'] . "\n" . $row['group'] . "\n" . $row['work'] . "\n";
					}
					break;
				case "work":
					while($row = mysql_fetch_array($result))
					{
						echo $row['id'] . "\n" . $row['name'] . "\n" . $row['info'] . "\n" . $row['member'] . "\n";
					}
					break;
				default:
					die("E:Invalid field");
			}
			break;
		default:
			echo("E:Invalid operation");
	}
	mysql_close($con);
?>