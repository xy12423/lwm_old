<?php
	function myErrHandler($error_level, $error_message){}
	
	function query_list($field, $con)
	{
		$result = mysql_query("SELECT * FROM `" . $field . "`", $con);
		if (!$result) die(mysql_error());
		
		switch ($field)
		{
			case "group":
			case "member":
			case "work":
				while($row = mysql_fetch_array($result))
				{
					echo "<a href=\"view.php?field=" . $field . "&id=" . $row['id'] . "\">" . $row['name'] . "</a><br />";
				}
				break;
			default:
				die("E:Invalid field");
		}
	}
	
	function split_list($field, $str, $con)
	{
		$units = explode(";", $str);
		if (!$units)
			return;
		
		foreach ($units as $id)
		{
			if ($id == "" || $id == '\0')
				break;
			$result = mysql_query("SELECT * FROM `" . $field . "` WHERE `id`=" . $id, $con);
			if (!$result) die(mysql_error());
			while($row = mysql_fetch_array($result))
			{
				echo "<a href=\"view.php?field=" . $field . "&id=" . $id . "\">" . $row['name'] . "</a><br />";
			}
		}
	}
	
	function query_info($field, $id, $con)
	{
		$result = mysql_query("SELECT * FROM `" . $field . "` WHERE `id`=" . $id, $con);
		if (!$result) die(mysql_error());
		
		switch ($field)
		{
			case "group":
				while($row = mysql_fetch_array($result))
				{
					echo "<p>Name:" . $row['name'] . "</p>";
					echo "<p>Members:<br />";
					split_list('member', $row['member'], $con);
					echo "</p>";
				}
				break;
			case "member":
				while($row = mysql_fetch_array($result))
				{
					echo "<p>Name:" . $row['name'] . "<br />Source:" . $row['src'] . "<br />Info:" . $row['info'] . "</p>";
					echo "<p>Groups:<br />";
					split_list('group', $row['group'], $con);
					echo "</p>";
					echo "<p>Works:<br />";
					split_list('work', $row['work'], $con);
					echo "</p>";
				}
				break;
			case "work":
				while($row = mysql_fetch_array($result))
				{
					echo "<p>Name:" . $row['name'] . "<br />Info:" . $row['info'] . "</p>";
					echo "<p>Members:<br />";
					split_list('member', $row['member'], $con);
					echo "</p>";
				}
				break;
		}
	}
	
	set_error_handler('myErrHandler', E_NOTICE);
	$con = mysql_connect("localhost", "", "");
	if (!$con) die(mysql_error());
	$result = mysql_select_db("workmanager", $con);
	if (!$result) die(mysql_error());
	if (isset($_GET["field"]))
	{
		echo "<p><a href=\"view.php\">Home</a></p>";
		//Run query
		query_info($_GET["field"], $_GET["id"], $con);
	}
	else
	{
		//Display list
		echo "<p>Groups:<br />";
		query_list("group", $con);
		echo "</p>";
		echo "<p>Members:<br />";
		query_list("member", $con);
		echo "</p>";
		echo "<p>Works:<br />";
		query_list("work", $con);
		echo "</p>";
	}
?>
