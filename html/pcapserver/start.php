<?php

include('pcapserverctrl.php');
$pcontrol = new pcapcontroller;
$pcontrol->start_new_pcapserver($_GET["filename"], 0);
echo "<script> document.location = \"index.php\"</script>";

?>