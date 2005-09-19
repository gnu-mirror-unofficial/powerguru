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

$version = "0.3";
$closetag = "</powerguru>";
$head = sprintf("<powerguru version=\"$version\">");

function headerCreate()
{
//   $head = "&lt;powerguru version $version";
//   $head .= "0.3";
//   $head .= ">";

  $head = sprintf("<powerguru version $version=0.3>", $version);
  return $head;
}

function requestCreate($top, $tag)
{
  //  $head = headerCreate();
  global $head, $closetag;

  $body = sprintf("<%s>%s</%s>%s", $top, $tag, $top, $closetag);
  return "$head $body";
}

function configRequestCreate($top, $tag, $subtag)
{
  //  $head = headerCreate();
  global $head, $closetag;

  $body = sprintf("<%s><%s><%s></%s></%s></%s>%s",
                  $top, $tag, $subtag, $subtag, $tag, $top, $closetag);
  return "$head $body";
}


$depth = array();

function startElement($parser, $name, $attrs)
{
   global $depth;
   xml_set_character_data_handler($parser, "characterData");
//    for ($i=0; $i < $depth[$parser]; $i++) {
//        echo "  ";
//    }
   echo strtolower("$name<br>");
   while (list($key, $val) = each($attrs)) {
     echo strtolower("$key => $val<br>");
   }
   
   //   $depth[$parser]++;
}

function endElement($parser, $name)
{
   global $depth;
   //   $depth[$parser]--;
}

function characterData($parser, $data)
{
  global $first_child;
  if($first_child)  //if $data is first child, close start tag
    echo "><br />";
  if($data=trim($data))
    echo "<font color='blue'>$data</font><br />";
  $first_child=false;
}

function xmlParse($data)
{
  //  $data = str_replace("\"", "\\\"", $data);
  echo htmlspecialchars($data), "<br>";

  $xml_parser = xml_parser_create();
  xml_set_element_handler($xml_parser, "startElement", "endElement");
  
  if (xml_parse($xml_parser, $data, false)) {
    die(sprintf("XML error: %s at line %d",
                xml_error_string(xml_get_error_code($xml_parser)),
                xml_get_current_line_number($xml_parser)));
  }
  
  xml_parser_free($xml_parser);
}

// Grab some XML data, either from a file, URL, etc. however you want.
// Assume storage in $strYourXML;
//
// $objXML = new xml2Array();
// $arrOutput = $objXML->parse($strYourXML);
// print_r($arrOutput); //print it out, or do whatever!
//
class xml2Array {
  
   var $arrOutput = array();
   var $resParser;
   var $strXmlData;
  
   function parse($strInputXML) {
  
           $this->resParser = xml_parser_create ();
           xml_set_object($this->resParser,$this);
           xml_set_element_handler($this->resParser, "tagOpen", "tagClosed");
           
           xml_set_character_data_handler($this->resParser, "tagData");
      
           $this->strXmlData = xml_parse($this->resParser,$strInputXML );
           //           if(!$this->strXmlData) {
           if($this->strXmlData) {
               die(sprintf("XML error: %s at line %d",
           xml_error_string(xml_get_error_code($this->resParser)),
           xml_get_current_line_number($this->resParser)));
           }
                          
           xml_parser_free($this->resParser);
          
           return $this->arrOutput;
   }
   function tagOpen($parser, $name, $attrs) {
       $tag=array("name"=>$name,"attrs"=>$attrs);
       array_push($this->arrOutput,$tag);
   }
  
   function tagData($parser, $tagData) {
       if(trim($tagData)) {
           if(isset($this->arrOutput[count($this->arrOutput)-1]['tagData'])) {
               $this->arrOutput[count($this->arrOutput)-1]['tagData'] .= $tagData;
           }
           else {
               $this->arrOutput[count($this->arrOutput)-1]['tagData'] = $tagData;
           }
       }
   }
   
   function tagClosed($parser, $name) {
       $this->arrOutput[count($this->arrOutput)-2]['children'][] = $this->arrOutput[count($this->arrOutput)-1];
       array_pop($this->arrOutput);
   }
}

function getXmlFromArray($root)
{  
  if(count($root) > 0){
    
    $curr_name = $root['name'];
    $attribs = $root['attrs'];
    $curr_childs = $root['children'];
    $curr_data = $root['tagData'];
    
    $xml .= '<'.$curr_name;
    
    if(count($attribs) > 0){
      $i = 1;
      foreach($attribs as $key => $value){
        $curr_attribs .= $key.'="'.$value.'"';
        $i++;
        if($i <= count($attribs)){
          $curr_attribs .= ' ';
        }
      }
      $xml .= ' '.$curr_attribs;
    }       
    
    if($curr_data != ''){
      $xml .= '><![CDATA['.$curr_data.']]></'.$curr_name.'>';
    } else {
      if(count($curr_childs) > 0){
        $xml .= '>';
        foreach($curr_childs as $child){
          $xml .= getXmlFromArray($child);
        }
        $xml .= '</'.$curr_name.'>';
      } else {
        $xml .= '/>';
      }   
    }
    
  }
  return $xml;
}

function clientCreate()
{
  //  error_reporting(E_ALL);
  global $socket;
  
  echo "<h2>TCP/IP Connection</h2>\n";
  
  // Get the port for the WWW service.
  $service_port = getservbyname('powerguru', 'tcp');
  
  // Get the IP address for the target host.
  $address = gethostbyname('bertha');
  
  // Create a TCP/IP socket.
  $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
  if ($socket < 0) {
    echo "socket_create() failed: reason: " . socket_strerror($socket) . "\n";
  }
  
  echo "Attempting to connect to '$address' on port '$service_port'...";
  $result = socket_connect($socket, $address, $service_port);
  if ($result < 0) {
    echo "socket_connect() failed.\nReason: ($result) " . socket_strerror($result) . "\n";
  }
  socket_getsockname($socket, $addr, $port);
  $hostname = gethostbyaddr($addr);
  
  //in = "HEAD / HTTP/1.1\r\n";
  //$in .= "Host: www.example.com\r\n";
  //$in .= "Connection: Close\r\n\r\n";
  $in = "<powerguru version=\"0.3\"><client ip=\"$address\"></client>";
  $in .= "<server ip=\"$addr\">$hostname></server></powerguru>";
  $out = '';
  
  echo "Sending authentication message... ";
  socket_write($socket, $in, strlen($in));
  echo "OK.<br>";

  //  socket_set_nonblock($socket);
  
//  echo "Reading response:\n\n";
//   while ($out = socket_read($socket, 2048)) {
//     sleep(1);
//     echo $out;
//   }
//  $out = socket_read($socket, 2048);
//  echo $out;

  return $socket;
}

function clientClose()
{
  global $socket;
  
  echo "<br>Closing socket...";
  socket_shutdown($socket, 2);
  socket_close($socket);
  echo "OK.\n\n";
}

?>
