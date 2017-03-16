<?php
  
  class pcapcontroller
  {
	function start_new_pcapserver($filename)
	  {
   	       exec("./pcapserverctrl.pl add  $filename &>/dev/null &");
	  }

	function kill_pcapserver($pid)
	{
	       exec("./pcapserverctrl.pl kill $pid &>/dev/null &");
	}

	function list_pcapserver()
	{
		$result = exec("./pcapserverctrl.pl list");
		$pcapservers =  split(':', $result);
		return  $pcapservers;
	}
  };

  class pcapfilelist_entry
  {
	var $PID;
	var $filename;
        var $port;
        var $link;
        var $status;
        var $lastmodified;
	
	function get_convert_filename()
	{
	      $basefilename = basename($this->filename);
	      $tmpfilename = strstr($basefilename, '_');
	      if ($tmpfilename)
		return substr($tmpfilename,1,strlen($tmpfilename));
	      else
		return $basefilename;
		
	}
  };

function cmp_date($a, $b)
{
	return ($a->lastmodified < $b->lastmodified) ? 1 : -1;
}

function cmp_port($a, $b)
{
	return ($a->port < $b->port) ? 1 : -1;
}


  class pcapfilelist
  {
	var $pcapfilelist_array;
	var $pcapfilelistactive_array;

	function pcapfilelist()
	{
		$this->build_up_pcapfilelist();
	}

	function build_up_pcapfilelist()
	{
		//read the pcapfile from list
		$dir = "./pcapdir";

		$this->pcapfilelist_array = array();
		$this->pcapfilelistactive_array = array();

		$tmparray = array();

		if (is_dir($dir))
		 {
    			if ($dh = opendir($dir))
			 {
        			while (($file = readdir($dh)) != false)
				 {
					$pentry = new pcapfilelist_entry;
					if ($file != "." && $file != "..")
					{
						$pentry->filename = $dir."/".$file;
						$pentry->lastmodified = filemtime($pentry->filename);
        					$pentry->port     = "0";
						$pentry->link      ="#";
						$pentry->status    ="NO";
            					array_push($tmparray, $pentry);
					}
       				 }
        			closedir($dh);
   			 }
		}
		
		usort($tmparray, "cmp_date");
		//read active pcapfile , update the status
		$pcontrol = new pcapcontroller;
                $serverlist = $pcontrol->list_pcapserver();
		
		foreach ($tmparray as $pentry)
		{
			foreach ($serverlist as $server)
			{
				list($PID, $filename, $port) = split(" ", $server);
				if ($pentry->filename == $filename)
				{
			   		$pentry->port   = $port;
			    		$pentry->status = "Active";
			    		$pentry->link   = "http://pcapserver.opera.com:$port/pcapserver";
			    		$pentry->PID    = $PID;
					array_push($this->pcapfilelistactive_array, $pentry);
					break;
				}
			}
		
			if ($pentry->status != "Active")
				array_push($this->pcapfilelist_array, $pentry);
		}

	        usort($this->pcapfilelistactive_array, "cmp_port");
		
	}
  };
 

?>
