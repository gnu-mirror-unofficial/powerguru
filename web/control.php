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

echo "<head><title>PowerGuru Remote Control</title></head>";
echo "<h2 align=center>PowerGuru Remote Control</h2>";

include "support.php";
include "xml.php";

$send = opts("send","");
$mode = opts("mode","");
$status = opts("status","opmode");
$meters = opts("meters","poll");
$toptag = opts("toptag","status");
$config = opts("config","");
$gridconfig = opts("gridconfig","");
$genconfig = opts("genconfig","");

echo "<form action=control.php method=POST>";

//
// Create a System Status XML message
//
echo "<hr>";
echo "Get system status: <br><ul><table>";

echo "<tr><td>";
if ($toptag == "status") {
  echo "<input type=radio name=toptag value=status checked>Status mode";
}
else {
  echo "<input type=radio name=toptag value=status>Status mode";
}
echo "</td></tr>";

echo "<td></td><td>";
if ($status == "errormode") {
  echo "<input type=radio name=status value=errormode checked>Error mode";
}
else {
  echo "<input type=radio name=status value=errormode>Error mode";
}
echo "</td>";

echo "<td>";
if ($status == "opmode") {
  echo "<input type=radio name=status value=opmode checked>Op mode";
}
else {
  echo "<input type=radio name=status value=opmode>Op mode";
}
echo "</td>";

echo "<td>";
if ($status == "warningmode") {
  echo "<input type=radio name=status value=warningmode checked>Warning mode";
}
else {
  echo "<input type=radio name=status value=warningmode>Warning mode";
}
echo "</td>";

echo "</table>";
//
// Create a Meter XML message
//
echo "</ul><hr>";
echo "Get a meter reading: <br><ul><table>";

echo "<tr><td>";
if ($toptag == "meters") {
  echo "<input type=radio name=toptag value=meters checked>Meters";
}
else {
  echo "<input type=radio name=toptag value=meters>Meters";
}
echo "</td></tr>";

echo "<tr><td></td><td>";
if ($meters == "charge-amps") {
  echo "<input type=radio name=meters value=charge-amps checked>Charge Amps";
}
else {
  echo "<input type=radio name=meters value=charge-amps>Charge Amps";
}
echo "</td>";

echo "<td>";
if ($meters == "load-amps") {
  echo "<input type=radio name=meters value=load-amps checked>Load Amps";
}
else {
  echo "<input type=radio name=meters value=load-amps>Load Amps";
}
echo "</td>";

echo "<td>";
if ($meters == "pv-amps") {
  echo "<input type=radio name=meters value=pv-amps checked>PV Amps";
}
else {
  echo "<input type=radio name=meters value=pv-amps>PV Amps";
}
echo "</td>";

echo "<td>";
if ($meters == "pv-volts") {
  echo "<input type=radio name=meters value=pv-volts checked>PV Volts";
}
else {
  echo "<input type=radio name=meters value=pv-volts>PV Volts";
}
echo "</td>";

echo "<td>";
if ($meters == "daily-kwh") {
  echo "<input type=radio name=meters value=daily-kwh checked>Daily KWH";
}
else {
  echo "<input type=radio name=meters value=daily-kwh>Daily KWH";
}
echo "</td>";

echo "<td>";
if ($meters == "hertz") {
  echo "<input type=radio name=meters value=hertz checked>Hertz";
}
else {
  echo "<input type=radio name=meters value=hertz>Hertz";
}
echo "</td>";

echo "<td></tr><tr><td></td><td>";
if ($meters == "buy-amps") {
  echo "<input type=radio name=meters value=buya-mps checked>Buy Amps";
}
else {
  echo "<input type=radio name=meters value=buy-amps>Buy Amps";
}
echo "</td>";

echo "<td>";
if ($meters == "sell-amps") {
  echo "<input type=radio name=meters value=sell-amps checked>Sell Amps";
}
else {
  echo "<input type=radio name=meters value=sell-amps>Sell Amps";
}
echo "</td>";

echo "<td>";
if ($meters == "ac-volts-out") {
  echo "<input type=radio name=meters value=ac-volts-out checked>AC Volts Out";
}
else {
  echo "<input type=radio name=meters value=ac-volts-out>AC Volts Out";
}
echo "</td>";

echo "<td>";
if ($meters == "ac1-volts-in") {
  echo "<input type=radio name=meters value=ac1-volts-in checked>AC1 Volts In";
}
else {
  echo "<input type=radio name=meters value=ac1-volts-in>AC1 Volts In";
}
echo "</td>";

echo "<td>";
if ($meters == "ac2-volts-in") {
  echo "<input type=radio name=meters value=ac2-volts-in checked>AC2 Volts In";
}
else {
  echo "<input type=radio name=meters value=ac2-volts-in>AC2 Volts In";
}
echo "</td>";

echo "<td>";
if ($meters == "poll") {
  echo "<input type=radio name=meters value=poll checked>All Meters";
}
else {
  echo "<input type=radio name=meters value=poll>All Meters";
}
echo "</td>";

echo "</table>";

//
// Create a Command XML message
//
echo "</ul><hr>";
echo "Send a command: <br><ul><table>";

echo "<tr><td>";
if ($toptag == "command") {
  echo "<input type=radio name=toptag value=command checked>Commands";
}
else {
  echo "<input type=radio name=toptag value=command>Commands";
}
echo "</td></tr>";

echo "<tr><td></td><td>";
if ($command == "generator") {
  echo "<input type=radio name=command value=generator checked>Generator";
}
else {
  echo "<input type=radio name=command value=generator>Generator";
}
echo "</td>";

echo "<td>";
if ($command == "grid") {
  echo "<input type=radio name=command value=grid checked>Grid";
}
else {
  echo "<input type=radio name=command value=grid>Grid";
}
echo "</td>";

echo "<td>";
if ($command == "relay") {
  echo "<input type=radio name=command value=relay checked>Relay";
}
else {
  echo "<input type=radio name=command value=relay>Relay";
}
echo "</td>";

echo "<td>";
if ($command == "auxilary") {
  echo "<input type=radio name=command value=auxilary checked>Auxilary";
}
else {
  echo "<input type=radio name=command value=auxilary>Auxilary";
}
echo "</td>";

echo "<td></tr><tr><td></td></td><td>";
if ($command == "battery") {
  echo "<input type=radio name=command value=battery checked>Battery";
}
else {
  echo "<input type=radio name=command value=battery>Battery";
}
echo "</td>";

echo "<td>";
if ($command == "poll") {
  echo "<input type=radio name=command value=poll checked>Poll Meters";
}
else {
  echo "<input type=radio name=command value=poll>Poll Meters";
}
echo "</td>";

echo "<td>";
if ($command == "inverter") {
  echo "<input type=radio name=command value=inverter checked>Inverter";
}
else {
  echo "<input type=radio name=command value=inverter>Inverter";
}
echo "</td>";

echo "<td>";
if ($command == "charger") {
  echo "<input type=radio name=command value=charger checked>Charger";
}
else {
  echo "<input type=radio name=command value=charger>Charger";
}
echo "</td>";

echo "<table>";

//
// Create a Config XML message
//
echo "</ul><hr>";

echo "Get/Set a configuration value: <br><ul><table>";
echo "<tr><td>";
if ($toptag == "config") {
  echo "<input type=radio name=toptag value=config checked>Config";
}
else {
  echo "<input type=radio name=toptag value=config>Config";
}
echo "</td></tr>";

echo "<tr><td></td><td>";
if ($config == "generator") {
  echo "<input type=radio name=config value=generator checked>Generator";
}
else {
  echo "<input type=radio name=config value=generator>Generator";
}
echo "</td></tr>";

echo "<tr><td></td><td></td><td>";
if ($genconfig == "buy") {
  echo "<input type=radio name=genconfig value=buy checked>Buy Time";
}
else {
  echo "<input type=radio name=genconfig value=buy>Buy Time";
}
if ($genconfig == "sell") {
  echo "<input type=radio name=genconfig value=sell checked>Sell Time";
}
else {
  echo "<input type=radio name=genconfig value=sell>Sell Time";
}
echo "</td></tr>";

echo "<tr><td></td><td>";
if ($config == "grid") {
  echo "<input type=radio name=config value=grid checked>Grid";
}
else {
  echo "<input type=radio name=config value=grid>Grid";
}
echo "</td></tr>";

echo "<tr><td></td><td></td><td>";
if ($gridconfig == "buy") {
  echo "<input type=radio name=gridconfig value=buy checked>Buy Time";
}
else {
  echo "<input type=radio name=gridconfig value=buy>Buy Time";
}
if ($gridconfig == "sell") {
  echo "<input type=radio name=gridconfig value=sell checked>Sell Time";
}
else {
  echo "<input type=radio name=gridconfig value=sell>Sell Time";
}
echo "</td></tr>";


echo "</table></ul>";


echo "<p><input type=submit name=send value='Send'>";

echo "</form><p>";

$tag = "";
$subtag = "";

if ($toptag == "meters") {
  $tag = $meters;
}
if ($toptag == "status") {
  $tag = $status;
}
if ($toptag == "command") {
  $tag = $command;
}
if ($toptag == "config") {
  $tag = $config;
  if ($config == "grid") {
    $subtag = $gridconfig;
  }
  if ($config == "generator") {
    $subtag = $genconfig;
  }  
}

// echo "1 $toptag<br>";
// echo "2 $tag<br>";
// echo "3 $subtag<br>";
// echo "3 $gridconfig<br>";
// echo "3 $genconfig<br>";

if ($tag == "") {
  echo "<h2>No tag supplied</h2>";
} else {
  if ($toptag != "config") {
    echo "<h2>", requestCreate($toptag, $tag), "</h2>";
  } else {
    echo "<h2>", configRequestCreate($toptag, $tag, $subtag), "</h2>";
  }
}


//echo "<h2>", headerCreate(), "</h2>";

 
//phpinfo(INFO_VARIABLES);

?>

</body>
</html>
