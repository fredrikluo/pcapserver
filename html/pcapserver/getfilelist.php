<?php
{
  $plist = new pcapfilelist;
  for ($i=0; $i<count($plist ->pcapfilelist_array);$i++)
  {
        $pentry = $plist ->pcapfilelist_array[$i];
        $filename =  $pentry->get_convert_filename();
        echo "FILE: $filename  STATUS: $pentry->status \n";
        echo "<a href = \"./start.php?filename=$pentry->filename\">start</a>\n";
        echo "<br>";
  }
}
?>

