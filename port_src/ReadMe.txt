Put step_cmdline.exe in the Open the CO_Test or HOA_Test directory.
Open a DOS window and cd to the CO_Test or HOA_Test directory.
Run:
step_cmdline.exe
and it prints the available ASIO drivers as
index driver_label

Run:
step_cmdline <index> Test1-1_256_CO_session.asi 
and it will walk you through the rest. 

It is just keyboard control and no scoring or randomization. But it exercises the full driver for 32-channel playout.

Please try it and tell me if it works. It has two new features:
- it reads the mono files
- it reads the entire set of files into memory, so there is absolutely no disk access during play