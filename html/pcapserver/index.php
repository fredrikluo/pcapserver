<?php
  header("Cache-Control: no-cache, must-revalidate");
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <title>Pcap server</title>
  <style type="text/css">
    body
    {
     background-color:#F9F9F9;
     font-family:arial;
    }

    a:link
    { 
       text-decoration: none; 
    } 
   
    a:visited 
    {  
       text-decoration: none; 
    } 

    a:hover 
    { 
       text-decoration: underline; 
    }
  </style>
</head>
<body>
 <img src="pcapserver.gif" alt=""/> <br />
 <a href ="http://wiki/developerwiki/index.php/Newfoundland/Testing/Pcapserver"><font color="red"> Full introduction and instructions can be found at wiki, click here to see .</font></a>
<br /><br />
<br>
<a href="upload.php"> <span style="color:#555555;font-weight:bold;font-family:arial">[Click here to upload new pcap file]</span></a> <br />
<hr width="80%" align="left"/>
<br /><br />
 <span style="color:#555555;font-weight:bold;font-family:arial">Active sessions list: </span> <br />
 <div style="border:dotted 1px;background-color:#eeeeee;padding:6px;width:80%">
 <?php
  include ('pcapserverctrl.php');
  $plist = new pcapfilelist;
  for ($i=0; $i<count($plist ->pcapfilelistactive_array);$i++)
  {
        $pentry = $plist ->pcapfilelistactive_array[$i];
        $filename =  $pentry->get_convert_filename();
        echo "<font color='green'>$filename </font> is running at port: $pentry->port &nbsp;&nbsp;&nbsp;\n";
        echo "<a href = \"./details.php?filename=$filename&port=$pentry->port\"><font color='brown'> [details]</font></a> \n";
        echo "<a href = \"./kill.php?pid=$pentry->PID\"><font color='red'>[kill]</font></a><br/>\n";
  }
 ?>
 </div>
 <br />
 <span style="color:#555555;font-weight:bold;font-family:arial">Pcapfile repository:</span> <br />
 <div style="border:dotted 1px;background-color:#eeeeee; padding:6px;width:80%">
 <?php
  for ($i=0; $i<count($plist ->pcapfilelist_array);$i++)
  {
        $pentry = $plist ->pcapfilelist_array[$i];
        $filename =  $pentry->get_convert_filename();
        echo "<font color='grey'>$filename</font> &nbsp; \n";
        echo "<a href = \"./start.php?filename=$pentry->filename\"><font color='green'> [start]</font></a>\n";
        echo "<br />";
  }
 ?>
 </div>
 
 <br />
 
<hr width="80%" align="left"/>
<div   style ="text-align:left;width:80%"> <div style="text-align:center"> Powered by zhiyul@opera.com </div>  </div>
</body>
</html>