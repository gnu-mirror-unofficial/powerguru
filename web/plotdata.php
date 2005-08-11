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

echo "<head><title>PowerGuru Data Plot</title></head>";
echo "<h2 align=center>PowerGuru Data Plot</h2>";

include "support.php";
//include "plots.php";
include "charts.php";

$query = opts("query","");
$whichtable = opts("whichtable","meters");
$whichfield = opts("whichfield","battery_volts");
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

// Setup the starting timestamp for a query. This is easier
// than doing the query with separete fields.
if ($start_hour != "midnight") {
  if ($start_hour != "noon") {
    $tmp = "$start_month $start_day, $start_year $start_hour";
  } else {
    $tmp = "$start_month $start_day, $start_year 12:00";
  }
} else {
  $tmp = "$start_month $start_day, $start_year 00";
}

$start_timestamp = strtotime($tmp);
$query_start = Date('Y/m/d H:i:00', $start_timestamp);
//echo "$query_start<br>";

// Setup the starting timestamp for a query. This is easier
// than doing the query with separete fields.
if ($end_hour != "midnight") {
  if ($end_hour != "noon") {
    $tmp = "$end_month $end_day, $end_year $end_hour";
  } else {
    $tmp = "$end_month $end_day, $end_year 12:00";    
  }
} else {
  $tmp = "$end_month $end_day, $end_year 23:59:59";
}
$end_timestamp = strtotime($tmp);
$query_end = Date('Y/m/d H:i:00', $end_timestamp);
//echo "$query_end<br>";

// Make a unique name so we can support multiple hits to the
// same web pages.
$addr = $_SERVER["REMOTE_ADDR"];
$port = $_SERVER["REMOTE_PORT"];
$unique_name = "iamps-$addr-$port.png";
$unique_name = "iamps-$addr.png";
$filespec = "/var/www/html/senecass/tmp/$unique_name";
if(file_exists($filespec)) {
  unlink($filespec);
}

echo "<h3>Fields to Display</h3>";

//$query = str_replace(' ', '+', $query);

echo "Please select fields from $whichtable that you wish to see data for:<p>";
echo "<form action=plotdata.php?whichtable=$whichtable&whichfield=$whichfield method=POST>";
$fieldnames = dbfields($whichtable);
$row=0;
$col=0;
echo "<table>";

foreach ($fieldnames as $field) {
  if ($field == "timestamp") {
    continue;
  }
  
  if ($cols == 0) {
    echo "<tr>";
  }
  $cols++;
  //  $fieldname = $field;
  if ($whichfield == "$field") {
    $checked=checked;
  }
  else {
    $checked="";
  }
  echo "<td width=20%><input type=radio name=whichfield value=$field $checked> $field</td>";
  $i++;
  if ($cols == 4) {
    echo "</tr>";
    $cols = 0;
  }
}
echo "</table>";

for ($i=$date[0][1]; $i<=$date[1][1]; $i++) {
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

echo "<p><table>";
//echo "<form method=post action=plotdata.php?query=$query>";
echo "<tr><td width=40%>Start Date & Time</td><td>";
echo "<select name=start_month><br>";
foreach ($month_display as $i) {
  if ($i == $start_month) {
    echo "<option SELECTED value=$start_month>$start_month</option>";
  } else {
    echo "<option>$i</option>";
  }
}
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
  if ($i == $end_month) {
    echo "<option SELECTED value=$end_month>$end_month</option>";
  } else {
    echo "<option>$i</option>";
  }
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

$query = "SELECT timestamp,$whichfield from $whichtable";
if ($start_day != 0) { 
  $query="$query WHERE timestamp >= '$query_start'"; 
}

if ($end_day != 0) { 
  $query="$query AND timestamp <= '$query_end'"; 
}

$query = "$query ORDER BY timestamp";

echo "</ul><p><input type=submit name=start value='New Chart'>";
echo "</form><p>";

//echo "<br break=clear>$query";

//
// Do the plot
//
$data = dbquery($query);
//$data = get_data();

if (count($data)) {
  $pts = plot_invert_volts($filespec, $data);
  //spline_chart($data);
  
  // Load the new image
  // It's been created in a top level directory under this domain, which
  // has to be writable. It's at the top, so only that one directory will
  // need access by the apache user and group.
  echo "<center>";
  echo "<img src=/tmp/$unique_name><br>";
  echo "</center>";
} else {
  echo "No data was returned from the query";
}

echo "<p>Plotting $pts Data Points<br>";
echo "The query for this chart was:<br>$query";

//phpinfo(INFO_VARIABLES);

?>

</body>
</html>
