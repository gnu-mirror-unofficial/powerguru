<?php
session_start();
//
//   Copyright (C) 2005 Free Software Foundation, Inc.
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="description" content="PowerGuru Database Server">
   <title>PowerGuru Database Server</title>
</head>
<body>

<h2 align=center>PowerGuru Database Server</h2>

<?php

$host=$_SERVER['HTTP_HOST'];

echo "<ul>";
echo "<dl>";

// Check the options passed in. If "rob" is set to "yes", then add links
// to other development level stuff. This is just cause I'm lazy...
$robflag=$_GET["dbadmin"];
if ($robflag == "yes")
{
  echo "<dt><a href=../php/phpMyAdmin-2.5.6/>Database maintainance</a>";
  echo "<dd>This is so I can easily access the raw database";
}

// phpinfo(INFO_VARIABLES);

?>

<dt><a href=browsedata.php>Browse Data</a>
<dd>Browse the data in tabular form right from the database.

<dt><a href=dataminer.php>Data Miner</a>
<dd>Data mine the data.

<dt><a href=plotdata.php>Plot Data</a>
<dd>Plot the data in graphical form.

<dt><a href="http://www.senecass.com/software/powerguru/">PowerGuru Web Site</a>
<dd>This is the main web site for PowerGuru, which includes source code and manuals.

<dt><a href="http://www.senecass.com">Seneca Software & Solar Web Site</a>
<dd>This work has been entirely sponsored by my own business.

</ul>
</dl>

</body>
</html>
