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

 $docroot = "/home/www/";
//$docroot = _SERVER["DOCUMENT_ROOT"];

// This resolution perfectly fits the Zaurus screen
$zaurus_x = 435;
$zaurus_y = 495;

function plot_invert_volts($filespec, $data)
{
  global $zaurus_x, $zaurus_y;

  // This is the scaling factors for the plot
  $xscale = 5;
  $yscale = 2;
  // Add a margin so the plots look better
  $margin = 10;
  // Put some additional space on the bottom of the plot
  $bot    = 40;

  // Calculate the minimum and maximum array values so we know how big
  // to make the plot the right size
  for ($i=0; $i<count($data); $i++) {
    $plot_x[] = substr($data[$i][0], 17, 2);
    $plot_y[] = $data[$i][5];
    //echo $plot_y[$i], "<br>";
    //echo $plot_x[$i], "<br>";
  }

  // Get plot boundaries
  $min_plot_x_axis = min($plot_x); // - $margin;
  $max_plot_x_axis = max($plot_x); // + $margin;
  $min_plot_y_axis = min($plot_y) - $margin;
  $max_plot_y_axis = max($plot_y) + $margin;
  
  //  $x_axis_len = ((max($route_x) - min($route_x)) / $xscale) + (margin * 2);
  //  $y_axis_len = ((max($route_y) - min($route_y)) / $yscale) + (margin * 2) + $bot;
  $x_axis_len = $zaurus_x;
  $y_axis_len = $zaurus_y;

  if (1) {
    echo "Plot is ", $x_axis_len, " x ", $y_axis_len, "<br>";
    echo "Min Plot X-axis is ", $min_plot_x_axis, "<br>";
    echo "Max Plot X-axis is ", $max_plot_x_axis, "<br>";
    echo "Delta Plot X-axis is ", $max_plot_x_axis-$min_plot_x_axis, "<br>";     
    echo "Min Plot Y-axis is ", $min_plot_y_axis, "<br>";
    echo "Max Plot Y-axis is ", $max_plot_y_axis, "<br>";
    echo "Delta Plot Y-axis is ", $max_plot_y_axis-$min_plot_y_axis, "<p>";
  }
 
  // Setup the plot image
  $image      = imageCreate($x_axis_len, $y_axis_len);

  // Get a few default colors to use elsewhere
  $white      = ImageColorAllocate($image, 255, 255, 255);
  $black      = ImageColorAllocate($image, 0, 0, 0);
  $red        = ImageColorAllocate($image, 255, 0, 0);
  $green      = ImageColorAllocate($image, 0, 255, 0);
  $blue       = ImageColorAllocate($image, 0, 0, 255);

  ImageFilledRectangle($image, 0,0, $x_axis_len, $y_axis_len, $black);

  $yscale = $zaurus_y/($max_plot_y_axis-$min_plot_y_axis);
  $xscale = $zaurus_x/($max_plot_x_axis-$min_plot_x_axis);

  // echo "Yscale factor is: ", $yscale, "<br>";
  // echo "Xscale factor is: ", $xscale, "<br>";

  // Make an array of coordinates to draw the lines
  for ($i=0; $i<count($plot_x); $i++) {
    echo "Plot: ", $plot_x[$i], " ===== ", $plot_y[$i], "<br>";
    
    if (($plot_x[$i] <= 0) && ($plot_x[$i] <= 0)) {
      break;
    }
    // if (($plot_y[$i] <= $min_y_axis) && ($plot_y[$i] >= $max_y_axis)) {
      //      break;
    //}
    //    $y1 = (($plot[$i][0]   - $min_plot_y_axis) / $yscale) - $margin;
    //    $x1 = (($plot[$i][1]   - $min_plot_x_axis) / $xscale) + $margin;
    //    $y2 = (($plot[$i+1][0] - $min_plot_y_axis) / $yscale) - $margin;
    //    $x2 = (($plot[$i+1][1] - $min_plot_x_axis) / $xscale) + $margin;

    $y1 = (($max_plot_y_axis - $plot_y[$i]) * $yscale) - $margin;
    $x1 = (($max_plot_x_axis - $plot_x[$i]) * $xscale); // + $margin;
    $y2 = (($max_plot_y_axis - $plot_y[$i+1]) * $yscale) - $margin;
    $x2 = (($max_plot_x_axis - $plot_x[$i+1]) * $xscale); // + $margin;
    // Filter out numbers that are out of range
    if (($x2 <= 0) && ($y2 <= 0)) {
      break;
    }
    if (($y1 >= $zaurus_y) && ($x1 >= $zaurus_x)) {
      break;
    }
    if (($y2 >= $zaurus_y) && ($x2 >= $zaurus_x)) {
      break;
    }
    //    echo "Plot: ", $x1, ", ", $y1, " --- ", $x2, ", ", $y2, "<br>";
    ImageLine($image, $x1,$y1, $x2,$y2, $green);
  }

  //ImageString($image, 2, $x_axis_len-1500, 20, $pos2, $red);

  //  sort($rx);
  //  array_multisort($ry, $rx);

  //  ImageLine($image, 10,10, 200,200, $green);
  //  ImagePolygon($image, $plotdata, count($plotdata)/2, $green);
  
  // Draw the chart
  //echo "Plotting $acid to $filespec<br>";
  ImagePNG($image, $filespec);
  // $graph->Stroke();

  echo "<img src=iamps.png><br>";
}

?>
