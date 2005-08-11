<?php
session_start();
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="description" content="PowerGuru Data Miner">
   <title>PowerGuru Data Miner</title>
</head>
<body background=ctas_bg.jpg>

<h3 align=center>PowerGuru Data Miner</h3>

<?php

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

include("support.php");
include("config.inc.php");

// Make sure mysql is loaded
// dl("mysql.so");

//
// Dump the tables as radio buttons so we can select one
// 
//echo "<h3>Available Tables in the ", $database, " Database:</h3>";
echo "<h3>Available Tables in the PowerGuru Database:</h3>";
echo "Please select a table from the list and click Update to change ";
echo "the fields that are displayed<p>";
echo "<ul>";
echo "<form action=dataminer.php method=POST>";

//
$whichtable = opts("whichtable","meters");
// This is the time interval
$interval = opts("interval", "0");
// Get the Custom SQL query, if there is one
$custom = opts("custom", "");
// This is the value for the LIMIT field in the SQL query
$limit = opts("limit", "50");

// Start timestamp for queries
$start_day    = opts("start_day", "1");
$start_month  = opts("start_month", "1");
$start_year   = opts("start_year", "1");
$start_hour   = opts("start_hour", "0");
$start_minute = opts("start_minute", "0");

// Start timestamp for queries
$end_day    = opts("end_day", "1");
$end_month  = opts("end_month", "1");
$end_year   = opts("end_year", "1");
$end_hour   = opts("end_hour", "0");
$end_minute = opts("end_minute", "0");

// Get the current date
$now = date("Y-m-d h:m:s");
$now_month = date("F");
$now_year = date("Y");
$now_day = date("m");
$now_hour = date("ga");

//echo "Table selected is: $whichtable<br>";
// Get the fields in a table
$tablelist = dbtables();

$table = 0;
$cols = 0;
echo "<table>";

foreach ($tablelist as $tablename)
{
  if ($whichtable == $tablename) {
    $checked=checked;
  }
  else {
    $checked="";
  }
  //echo "<LI><A href=\"powerguru.php?", $tablename, "\">", $tablename, "</A><BR>";
  if ($cols == 0) {
    echo "<tr>";
  }
  $cols++;
  echo "<td width=25%><input type=radio name=whichtable value=$tablename $checked> $tablename</td>";
  if ($cols == 4) {
    echo "</tr>";
    $cols = 0;
  }
  $table++;
}
echo "</table></ul>";

echo "<p><input type=submit name=table value='Update Fields'>";
echo "</form><p>";

//
// Dump the fields on each table so we can select them for queries
// 
echo "<h3>Fields to Display</h3>";

echo "Please select fields from $whichtable that you wish to see data for:<p>";

echo "<form action=dataminer.php?whichtable=$whichtable&interval=$interval&custom=$custom& method=POST>";
$fieldnames = dbfields($whichtable);
$row=0;
$col=0;
echo "<table>";

foreach ($fieldnames as $field) {
  if ($cols == 0) {
    echo "<tr>";
  }
  $cols++;
  //  $fieldname = $field;
  $fieldvalue = $_POST["$field"];
  if ($fieldvalue == "on") {
    $checked=checked;
  }
  else {
    $checked="";
  }
  echo "<td width=20%><input type=checkbox name=$field $checked> $field</td>";
  $i++;
  if ($cols == 6) {
    echo "</tr>";
    $cols = 0;
  }
}
echo "</table>";

echo "<hr><h3>Other Selection Criteria</h3>";
// See if the user wants distinct results
$checked="";
if($_POST[distinct] == "on") {
  $checked=checked;
}

echo "<table callpadding=3><tr>";
echo "<td width=4%><input type=checkbox name=distinct $checked> DISTINCT</td>";
$distinct=$_POST[distinct];
if ($distinct != "") {
  $distinct="DISTINCT ";
}

// See if the user wants to specify a count of rows to display
echo "<td width=35%><input type=text name=limit $checked value='$limit' maxlenght=3 size=3 >Number of Records</td>";

//echo "<td width=10%><input type=text name=interval value='$interval' maxlenght=6 size=4>Minutes</td>";

// A text field to enter a custom query instead
//echo "</tr><tr><td width=20% colspan=5><input type=text name=custom value='$custom' maxlength=128 size=40> Custom Query</td></tr>";

echo "</tr></table>";
//echo "<input type=submit name=table value='Execute Query'>";
//echo "</form>";

//
// Build the query string
//
$query="SELECT $distinct";
$index=0;
$fields=array();
foreach ($fieldnames as $i)
{
  $name=$_POST["$i"];
  if ($name == "on")
    {
      $fields[$index] = $i;
      if ($index > 0) {
	$query="$query,$i";
      }
      else {
	$query="$query $i";
      }
      $index++;
    }
}

// $allfields = "";
// foreach ($fieldnames as $field) {
//   if ($allfields == "") {
//     $allfields = "$field";
//   } else {
//     $allfields = "$allfields,$field";
//   }
// }
// echo $allfields, "<br>";

// Add the table name
if ($query == "SELECT ") {
  $query="$query * FROM $whichtable";
} else {
  $query="$query FROM $whichtable";
}

$stime = "$now, $interval";

//echo $stime;

if ($interval != 0) {
  $query="$query WHERE (now() - timestamp) < $interval";
}

if ($start_day != 0) { 
  $query="$query WHERE (dayofmonth(timestamp) >= $start_day)"; 
}

// if ($start_hour != 0) { 
//   $query="$query AND (hour(timestamp) >= '$start_hour')"; 
// }

if ($end_day != 0) { 
  $query="$query AND (dayofmonth(timestamp) <= $end_day)"; 
}

// if ($end_hour != 0) { 
//   $query="$query AND (hour(timestamp) <= '$end_hour')"; 
// }


// Add the optional LIMIT field
if ($limit != 0) {
  $query = "$query LIMIT $limit";
}

if ($custom != "") {
  $query = $custom;
}

$months = array("January", "Februrary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December");
$dayofweek = array("Mon", "Tues", "Wed", "Thur", "Fri", "Sat", "Sun");
$years = array("2000", "2001", "2002", "2003", "2004", "2005");
$hours = array("midnight", "1am", "2am", "3am", "4am", "5am", "6am", "7am", "8am", "9am", "10am", "11am", "noon", "1pm", "2pm", "3pm", "4pm", "5pm", "6pm", "7pm", "8pm", "9pm", "10pm", "11pm");
$minutes = array("0", "10", "20", "30", "40", "50", "60");
$date = date_range();

for ($i=$date[0][1]; $i<$date[1][1]; $i++) {
  $day_display[] = $i;
}
for ($i=0; $i<count($months); $i++) {
  if (($i >= $date[0][0]) && ($i <= $date[1][0])) {
    $month_display[] = $months[$i];
  }
}
for ($i=0; $i<count($years); $i++) {
  if (($i >= $date[0][4]) && ($i <= $date[1][4])) {
    $year_display[] = $years[$i];
  }
}

echo "<table>";
echo "<form method=post action=dataminer.php>";
echo "<tr><td width=40%>Start Date & Time</td><td>";
echo "<select name=start_month><br>";
foreach ($month_display as $i) {
  echo "<option>$i</option>";
}
//echo "<option SELECTED value=$now_month>$now_month</option>";
echo "</select>";

echo "<select name=start_day><br>";
foreach ($day_display as $i) {
  if ($i == $start_day) {
    echo "<option SELECTED value=$start_day>$start_day</option>";
  } else {
    echo "<option>$i</option>";
  }
}
echo "</select>";

echo "<select name=start_year><br>";
foreach ($year_display as $i) {
  echo "<option value=$i>$i</option>";
}
echo "<option SELECTED value=$now_year>$now_year</option>";
echo "</select>";

echo "<select name=start_hour><br>";
foreach ($hours as $i) {
  if ($i == $start_hour) {
    echo "<option SELECTED value=$start_hour>$start_hour</option>";
  } else {
    echo "<option>$i</option>";
  }
}
echo "</select>";

echo "<select name=start_minutes><br>";
foreach ($minutes as $i) {
  echo "<option>$i</option>";
}
echo "</select></td></tr><tr>";

echo "<td width=40%>End Date & Time</td><td>";
echo "<select name=end_month><br>";
foreach ($month_display as $i) {
  echo "<option>$i</option>";
}
echo "</select>";

echo "<select name=end_day><br>";
foreach ($day_display as $i) {
  if ($end_day == "") {
    $end_day = $date[1][1];
  }
  if ($i == $end_day) {
    echo "<option SELECTED value=$end_day>$end_day</option>";
  } else {
    echo "<option>$i</option>";
  }
}
    
echo "</select>";

echo "<select name=end_year><br>";
foreach ($year_display as $i) {
  echo "<option value=$i>$i</option>";
}
echo "<option SELECTED value=$now_year>$now_year</option>";
echo "</select>";

echo "<select name=end_hour><br>";
foreach ($hours as $i) {
  if ($i == $end_hour) {
    echo "<option SELECTED value=$end_hour>$end_hour</option>";
  } else {
    echo "<option>$i</option>";
  }
}
echo "</select>";

echo "<select name=end_minutes><br>";
foreach ($minutes as $i) {
  echo "<option>$i</option>";
}
echo "</select>";
echo "</table>";

echo "</ul><p><input type=submit name=start value='Execute Query'>";
echo "</form><p>";

echo "<form method=post action=dataminer.php?end_day=$end_day&start_day=$start_day>";
echo "<p>Query is:<p>";
echo "<input type=text name=custom$checked value='$query' maxlenght=512 size=100 ></form><p>";

$count=count($fieldnames);
//echo "The query is: $query<p>";
$result = dbquery($query);
if (count($result) == 0) {
  echo "No Results<br>";
} else {
    
  $rows=0;
  echo "<table callpadding=5 border><tr>";
  
  foreach ($fields as $i) {
    echo "<th>", $i, "</th>";
  }
  echo "</tr>";
  
  //while ($thisrow=mysql_fetch_row($result))  //get one row at a time
  foreach ($result as $thisrow) {
    echo "<tr>";
    $items=count($thisrow);
    $i=0;
    for ($i=0; $i<$items; $i++) {
      echo "<td>", $thisrow[$i], "</td>";
    }
    $rows++;
  if ($rows > $limit) {
    break;
  }
  echo "</tr>";
  }
  echo "</table>";
  echo "<p>";
echo "</form>";}

// phpinfo(INFO_VARIABLES);

?>

</body>
</html>
