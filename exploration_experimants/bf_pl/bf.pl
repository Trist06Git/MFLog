
:- use_module(library(readutil)).
:- load_files(['./machine.pl', './utils.pl']).

default_file('reddit_prog3.b').

:- write("Starting."), nl.

:- default_file(Filename),
   file_to_prog(Filename, Prog),
   create_mem(30000, Init),
   proc_prog(Prog, Init, _).
:- halt.
%:- write("Done."), nl, halt.
