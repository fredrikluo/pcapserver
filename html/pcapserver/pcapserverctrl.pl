#!/usr/bin/perl

use Socket;  

my $pcapservername  =  "./pcapserver";
my @pcapserver_list;
my $port_start    = 7007;
my $port_count  = 100; 

#Dump pcapserver list
sub dump_pcapserver_list
{
        $index = 1;
        while ($index < @pcapserver_list)
        {
           print "@pcapserver_list->[$index][0] @pcapserver_list->[$index][1] @pcapserver_list->[$index][2]:";
           $index++;
        }
}


#List out running pcapserver
sub list_pcapserver
{
        $pslist  = ` ps ax|grep $pcapservername`;
        @lines = split("\n", $pslist);
        @pcapserver_list = -1;

         $i = 0;
         while ($i < @lines)
         {
                @pcapserver_entry = split(" ", $lines[$i]);
                if ($pcapserver_entry[4]  eq  $pcapservername )
                {
                        @pcapsever = ($pcapserver_entry[0], $pcapserver_entry[5], $pcapserver_entry[6]);
                        push (@pcapserver_list, [@pcapsever]);
                }

                $i++;
         }
}


#Add new pcapserver
sub add_new_pcapserver
{
        my($pcapfile) = @_;
        $port = &test_port;
        print  "nohup $pcapservername   $pcapfile $port &";
        exec("nohup $pcapservername   $pcapfile $port &>./output/$port.txt &");
}

#Test port
sub test_port
{
     local *S;
     my $port = -1;

     if ( !socket(S, PF_INET   , SOCK_STREAM , getprotobyname('tcp')))
     {
           return $port;
     }

     setsockopt (S, SOL_SOCKET, SO_REUSEADDR, 1);

     for ($port = $port_start; $port < ($port_start + $port_count) ; $port ++)
     {
         if (bind(S, sockaddr_in($port, INADDR_ANY)))
         {
               return $port;
         }
      }

      return $port;
}

#Kill pcapserver
sub kill_pcapserver
{
       my($pid) = @_;
       exec("kill -9 $pid");
}

##################################
##Main function

if (! (defined @ARGV[0]))
{
        print "No cmd line\n";
        exit;
}

if (@ARGV[0] eq "list")
{
        list_pcapserver;
        dump_pcapserver_list;	
        exit;
}

if (@ARGV[0] eq "add")
{
       if (defined @ARGV[1])
       {
               add_new_pcapserver(@ARGV[1]);
       }
       else
       {
                print "No suffice parameters\n";
       }

       exit;
}

if (@ARGV[0] eq "kill")
{
        if (defined @ARGV[1])
        {
                kill_pcapserver(@ARGV[1]);
        }
        else
        {
                print "No PID specified\n";
        }

        exit;
}

#######################################
