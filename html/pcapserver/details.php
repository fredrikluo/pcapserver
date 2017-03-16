<?php
  header("Cache-Control: no-cache, must-revalidate");
?>
<html>
<head>
</head>
<body>
<?php
  $filename= $_GET["filename"]; 
  $port = $_GET["port"];
?>
<p>
<script >
/* Create a new XMLHttpRequest object to talk to the Web server */
var xmlHttp = false;

/*@cc_on @*/
/*@if (@_jscript_version >= 5)
try 
{
  xmlHttp = new ActiveXObject("Msxml2.XMLHTTP");
} 
catch (e) 
{
  try
  {
    xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
  }
  catch (e2)
 {
    xmlHttp = false;
  }
}
@end @*/

if (!xmlHttp && typeof XMLHttpRequest != 'undefined')
{
  xmlHttp = new XMLHttpRequest();
}

function callServer() {
  // Build the URL to connect to
  var url = "output/readout.php?file=<?php echo $port.".txt" ?>";
  // Open a connection to the server
  xmlHttp.open("GET", url, true);
  // Setup a function for the server to run when it's done
  xmlHttp.onreadystatechange = updatePage;
  // Send the request
  xmlHttp.send(null);
}

function updatePage() 
{
  if (xmlHttp.readyState == 4)
  {
    var response = xmlHttp.responseText;
    var lines = response.split('\n'); 
    var i  = lines.length - 20;
    if(i < 0) i = 0;
    var text = "";
    for (;i<lines.length;i++)
    {
	text += lines[i];
	text += "<br>";
    }   

    document.getElementById("output").innerHTML = text;

  }
}

 timerID = setInterval("callServer()", 1000);


</script>
 <a href = "index.php"><img src="pcapserver.gif" border=0 /> </a>
 <br>
 <br>

 <span style="color:#555555;font-weight:bold;font-family:arial"> Pcapserver is running at:</span> 
 <div style="border:dotted 1px;padding:6px;background-color:#eeeeee;;width:80%;font-family:arial">
  <b><font color=green>IP</font></b>:213.236.208.74<br>
  <b><font color=green>Port</font></b>: <? echo $port; ?><br>
  <b><font color=green>Filename</font></b>:<? echo $filename; ?> <br>
  Please set pcapserver.opera.com:<? echo $port; ?> as your proxy server<br>
  The entry page will be: <a href ="http://pcapserver.opera.com:<? echo $port;?>/pcapserver">http://pcapserver.opera.com:<? echo $port;?>/pcapserver</a><br>
 </div>

 <br>
 <span style="color:#555555;font-weight:bold;font-family:arial">Pcapserver output: </span>
 <div id=output style="border:dotted 1px;background-color:#000000;padding:6px;width:80%;font-family:arial;color:#ffffff">
  Loading
 </div>
</body>
</html>
