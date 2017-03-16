<?php

include('pcapserverctrl.php');
$pcontrol = new pcapcontroller;
$pcontrol->kill_pcapserver($_GET["pid"]);
echo "<script> document.location = \"index.php\"</script>";

?>