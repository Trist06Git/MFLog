
string_to_file(Filename, Str) :-
  open(Filename, write, File),
  write(File, Str),
  close(File).

:- op(699, xfx, :=).
:- op(698, yfx, ++).
Out := Left ++ Right :-
  string_concat(Left, Right, Out).

list_to_string(Lst, Str) :-
  reverse(Lst, Rev),
  list_to_string(Rev, "", Str).
list_to_string([], Res, Res).
list_to_string([H|T], Acc, Res) :-
  string_concat(H, Acc, Inter),
  list_to_string(T, Inter, Res).
  
