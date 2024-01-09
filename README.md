# PMan-Program
Assignment for Operating Systems (Fall 2022) at University of Victoria


To compile and run the code: <br>
    From UVic's Linux Environment, compile code by executing "make" from terminal while in the directory containing assignment files.
    To run code, execute "./pman" from terminal.

Pman Commands: <br>
    1. bg (cmd): <br>
        Starts executing cmd in the background, where cmd is a program.
        If running an executable file, then cmd must be in the form ./\<program_name\> <br>
    2. bglist <br>
        Prints list of all currently executing background program.
        List includes process id and file path, if applicable, of each running program. <br>
    3. bgkill (pid) <br>
        Permanently terminates program with given process id, pid. <br>
    4. bgstart (pid) <br>
        Re-starts program with given process id, pid. <br>
    5. bgstop (pid) <br>
        Temporarily stops program with given process id, pid. <br>
    6. pstat (pid) <br>
        Prints information related to program with process id, pid.
        Includes comm, state, utime, stime, rss, voluntary ctxt switches, nonvoluntary ctxt switches. <br>
    7. q <br>
        Exits Pman

Any other input or failure to add a pid argument to bgkill, bgstop, bgstart
    or pstat, will result in a invalid input error.
