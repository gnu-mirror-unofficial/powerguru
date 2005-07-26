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

// Get the config data
include("config.inc.php");

//
// Setup some arrays of commonly used data
// 
$months = array("January", "Februrary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December");
$dayofweek = array("Mon", "Tues", "Wed", "Thur", "Fri", "Sat", "Sun");
$years = array("2000", "2001", "2002", "2003", "2004", "2005");
$hours = array("midnight", "1am", "2am", "3am", "4am", "5am", "6am", "7am", "8am", "9am", "10am", "11am", "noon", "1pm", "2pm", "3pm", "4pm", "5pm", "6pm", "7pm", "8pm", "9pm", "10pm", "11pm");
$minutes = array("0", "10", "20", "30", "40", "50", "60");
$date = date_range();

// Unfortunately, not all browsers are created equal
function which_browser()
{
  $browser = "Mozilla";
  
  if (strpos($_SERVER["HTTP_USER_AGENT"], "MSIE")) {
    $browser = "msie";
  }
  

  // Check browser & Version
  //  if ("MSIE",$HTTP_USER_AGENT)) {
  //    $browser = "msie";
  //  }
  
  // Any MSIE browser
  $IE4x = eregi("MSIE 4.",$HTTP_USER_AGENT);
  // MSIE 4.x only
  $IE5x = eregi("MSIE 5.",$HTTP_USER_AGENT);
  // MSIE 5.x only
  $IE6x = eregi("MSIE 6.",$HTTP_USER_AGENT);
  // MSIE 6.x only

  $NS4x = eregi("Mozilla/4",$HTTP_USER_AGENT) && (!$IE) == 1;
  // Netscape 4.x only
  $NS   = eregi("Netscape", $HTTP_USER_AGENT) || ($NS4x) == 1;
  // Any Netscape browser
  $NS6x = eregi("Netscape6", $HTTP_USER_AGENT);
  // Netscape 6.x only
  $NS7x = eregi("Netscape/7", $HTTP_USER_AGENT);
  // Netscape 7.x only

  $MOZILLA = eregi("Mozilla/5.0",$HTTP_USER_AGENT) && (!$NS) == 1;
  // Mozilla

  $KMELEON = eregi("Gecko", $HTTP_USER_AGENT) AND (!$NS) AND $MOZILLA == 1;
  // Kmeleon
  $OPERA   = eregi("Opera", $HTTP_USER_AGENT);
  // Opera

  return $browser;
}

function get_data() 
{
  global $dbuser, $dbpass, $dbhost, $dbname;
  
  // These are the default values for the database
  // Make sure mysql is loaded
  // dl("mysql.so");

  $chandle = mysql_pconnect($dbhost, $dbuser, $dbpass) 
    or die("Connection Failure to Database");
  mysql_select_db($dbname, $chandle) or die ("Database not found.");

  $query = "SELECT * FROM inverter ORDER BY timestamp";
  // echo $query, "<br>";
  $result = mysql_db_query($dbname, $query) or die("Failed Query for inverter");
  while ($thisrow=mysql_fetch_row($result)) {
    $metadata[] = array($thisrow[0], $thisrow[1], $thisrow[2], $thisrow[3], $thisrow[4], $thisrow[5], $thisrow[6], $thisrow[7], $thisrow[8], $thisrow[9]);
    //echo "$thisrow[0], $thisrow[1], $thisrow[2], $thisrow[3], $thisrow[4], $thisrow[5], $thisrow[6],$thisrow[7], $thisrow[8], $thisrow[9] )<br>";
  }
  mysql_free_result($result);

  return $metadata;
}

function update_data_data($data, $official, $sortname, $listname, $notes, $url) 
{
  global $username, $password, $database;
  
  // These are the default values for the database
  // Make sure mysql is loaded
  // dl("mysql.so");
  
  $chandle = mysql_pconnect("localhost", $username, $password) 
    or die("Connection Failure to Database");
  mysql_select_db($database, $chandle) or die ("Database not found.");

  $query = "UPDATE datas SET name='$data',url='$url',notes='$notes',sort_name='$sortname',list_name='$listname' WHERE name ='$data'";
  //  echo $query, "<br>";
  $result = mysql_db_query($database, $query) or die("Failed Query for datas");
  echo "Done...<p>";
  
  $data = str_replace(' ', '+', $data);
  echo "<a href=editdata.php?data=$data>Back to Edit Data</>";
  echo "<p><a href=browsedatas.php>Back to Browse Datas</>";
}

function insert_data_data($data, $official, $sortname, $listname, $notes, $url) 
{
  global $username, $password, $database;
  
  // These are the default values for the database
  // Make sure mysql is loaded
  // dl("mysql.so");
  
  $chandle = mysql_pconnect("localhost", $username, $password) 
    or die("Connection Failure to Database");
  mysql_select_db($database, $chandle) or die ("Database not found.");

  $query = "INSERT INTO datas () VALUES ('$data','$notes','$sortname','$listname','$url')";
  //  echo $query, "<br>";
  $result = mysql_db_query($database, $query) or die("Failed Query for datas");
  echo "Done...<p>";
  
  $data = str_replace(' ', '+', $data);
  echo "<a href=editdata.php?data=$data>Back to Edit Data</>";
  echo "<p><a href=browsedatas.php>Back to Browse Datas</>";
}

function dbquery($query)
{
  global $dbuser, $dbpass, $dbhost, $dbname;
  
  $chandle = mysql_pconnect($dbhost, $dbuser, $dbpass) 
    or die("Connection Failure to Database");
  mysql_select_db($dbname, $chandle) or die ("Database not found.");

  //echo "Query is: $query<br>";
  $result = mysql_query($query) or die("Failed Query");
  while ($thisrow=mysql_fetch_row($result)) {
    $list[] = array($thisrow[0],$thisrow[1]);
    //echo "$thisrow[0],$thisrow[1]<br>";
  }
  
  mysql_free_result($result);
  mysql_close($chandle);

  return $list;
}

function dbfields($table)
{
  global $dbuser, $dbpass, $dbhost, $dbname;
    
  $chandle = mysql_pconnect($dbhost, $dbuser, $dbpass) 
    or die("Connection Failure to Database");
  mysql_select_db($dbname, $chandle) or die ("Database not found.");

  $query="SELECT * from $table LIMIT 1";
  //echo "Query is: $query<br>";
  $result = mysql_query($query) or die("Failed Query");

  for ($i=0; $i<mysql_num_fields($result); $i++) {
    $field=mysql_fetch_field($result, $i);
    $fieldnames[$i] = "$field->name";
  }

  //mysql_close($chandle);

  return $fieldnames;
}

function date_range()
{
  global $dbuser, $dbpass, $dbhost, $dbname;
    
  $chandle = mysql_pconnect($dbhost, $dbuser, $dbpass) 
    or die("Connection Failure to Database");
  mysql_select_db($dbname, $chandle) or die ("Database not found.");

  $query="select month(min(timestamp)),min(dayofmonth(timestamp)),hour(min(timestamp)),minute(min(timestamp)),year(min(timestamp)),
month(max(timestamp)),max(dayofmonth(timestamp)),hour(max(timestamp)),minute(max(timestamp)),year(max(timestamp)) from inverter";
  //echo "Query is: $query<br>";
  $result = mysql_query($query) or die("Failed Query");
  while ($thisrow=mysql_fetch_row($result)) {
    $date[] = array($thisrow[0]-1, $thisrow[1], $thisrow[2], $thisrow[3], $thisrow[4]);
    $date[] = array($thisrow[5]-1, $thisrow[6], $thisrow[7], $thisrow[8], $thisrow[9]);
    //echo "$thisrow[0], $thisrow[1], $thisrow[2], $thisrow[3], $thisrow[4])<br>";
    //echo "$thisrow[5], $thisrow[6], $thisrow[7], $thisrow[8])<br>";
  }
  mysql_free_result($result);
  
  //mysql_close($chandle);

  return $date;
}

// Get the tables in a database
function dbtables()
{
  global $dbuser, $dbpass, $dbhost, $dbname;
   
  // These are the default values for the database
  // Make sure mysql is loaded
  // dl("mysql.so");

  $chandle = mysql_pconnect($dbhost, $dbuser, $dbpass) 
    or die("Connection Failure to Database");
  //mysql_select_db($dbname, $chandle) or die ("Database not found.");

  $query = "SHOW TABLES FROM $dbname";
  //echo "Query is: $query<br>";
  $result = mysql_query($query) or die("Failed Query");
  if (!$result) {
    echo "DB Error, could not list tables\n";
    echo 'MySQL Error: ' . mysql_error();
    exit;
  }
  
  while ($row = mysql_fetch_row($result)) {
    $list[] = $row[0];
    //echo "Table: {$row[0]}\n";
  }
  //mysql_close($chandle);
 
  return $list;
}

function opts($var, $default)
{
  $opt="$_POST[$var]";
  if ($opt == "") {
    $opt=$_GET[$var];
  }
  if ($opt == "" OR $opt == "on") {
    $opt="$default";
  }

  return $opt;
}

?>
