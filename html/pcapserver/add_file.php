<?php 
  $uploaddir = './pcapdir/';
  $uploadfile = $uploaddir .time()."_".str_replace(" ","_",basename($_FILES['pcapfile']['name']));
  echo $uploadfile;
  move_uploaded_file($_FILES['pcapfile']['tmp_name'], $uploadfile);
  echo "<script> document.location = \"index.php\"</script>";
?>