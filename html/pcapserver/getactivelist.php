<?php
{
  $plist = new pcapfilelist;
  for ($i=0; $i<count($plist ->pcapfilelistactive_array);$i++)
  {
        $pentry = $plist ->pcapfilelistactive_array[$i];
        $filename =  $pentry->get_convert_filename();
        echo "FILE: $filename PORT: $pentry->port STATUS: $pentry->status \n";
        echo "<a href = \"./details.php?filename=$filename&port=$pentry->port\"> details </a>  \n";
        echo "<a href = \"./kill.php?pid=$pentry->PID\">kill</a>\n";
        echo "<br>";
  }
}
?>
