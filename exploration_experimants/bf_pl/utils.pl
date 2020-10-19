
create_mem(N, state([0], R)) :-
  RN is N - 1,
  create_list(RN, 0, R).

create_list(N, Val, Res) :-
  N >= 0,
  create_list(N, Val, [], Res).

create_list(0, _, Res, Res).
create_list(N, Val, Acc, Res) :-
  N >= 0,
  NN is N - 1,
  create_list(NN, Val, [Val|Acc], Res).

cat(Filename) :-
  open(Filename, read, Stream),
  read_file(Stream, Codes),
  codes_to_chars(Codes, Text),
  write(Text), nl.

read_file(Stream, []) :-
  at_end_of_stream(Stream).
read_file(Stream, Res) :-
  not(at_end_of_stream(Stream)),
  read_line_to_codes(Stream, Str),
  append(Str, Lst, Res),
  read_file(Stream, Lst), !.

codes_to_chars(Lst, Res) :-
  codes_to_chars(Lst, [], Rev),
  reverse(Rev, Res).

codes_to_chars([], Res, Res).
codes_to_chars([Code|T], Acc, Res) :-
  char_code(Char, Code),
  codes_to_chars(T, [Char|Acc], Res).
  
file_to_prog(Filename, Prog) :-
  open(Filename, read, Stream),
  read_file(Stream, Codes),
  codes_to_chars(Codes, Prog).

