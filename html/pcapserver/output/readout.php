<?php
   header("Cache-Control: no-cache, must-revalidate");
   $txtfile = $_GET["file"];
   include( $txtfile);
?>