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

$version = "0.2";
$closetag = "&lt;/powerguru&gt;";
$head = sprintf("&lt;powerguru version=$version&gt;");

function headerCreate()
{
//   $head = "&lt;powerguru version $version";
//   $head .= "0.3";
//   $head .= "&gt;";

  $head = sprintf("&lt;powerguru version $version=0.3&gt;", $version);
  return $head;
}

function requestCreate($top, $tag)
{
  //  $head = headerCreate();
  global $head, $closetag;

  $body = sprintf("&lt;%s&gt;%s&lt;/%s&gt;%s", $top, $tag, $top, $closetag);
  return "$head $body";
}

function configRequestCreate($top, $tag, $subtag)
{
  //  $head = headerCreate();
  global $head, $closetag;

  $body = sprintf("&lt;%s&gt; &lt;%s&gt; &lt;%s&gt; &lt;/%s&gt; &lt;/%s&gt; &lt;/%s&gt; %s",
                  $top, $tag, $subtag, $subtag, $tag, $top, $closetag);
  return "$head $body";
}

?>
