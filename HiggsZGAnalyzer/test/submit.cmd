Universe                = vanilla
Notify_user             = brian.pollack@cern.ch
Notification            = Error
Executable              = main 
Rank                    = Mips
Requirements            = (OpSys == "LINUX") && (Disk >= DiskUsage) && ((Memory * 1024) >= ImageSize) && (HasFileTransfer)
+LENGTH                 = "LONG"
GetEnv                  = True
Input                   = /dev/null
Output                  = job.out
Error                   = job.err
Log                     = job.log
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
transfer_input_files    = tmp.txt 
Queue
