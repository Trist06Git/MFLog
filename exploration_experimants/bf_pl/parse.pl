
open('[').
closed(']').

check_br([], []).
check_br([H|T], Brs) :-
  open(H),
  check_br(T, [b|Brs]).
check_br([H|T], [b|Brs]) :-
  closed(H),
  check_br(T, Brs).
check_br([H|T], Brs) :-
  not(closed(H)),
  not(open(H)),
  check_br(T, Brs).

empty_list([]).

%%intended to be used with a list that starts with
% an open bracket loop
sub_loop(Loop, Res, Rest) :-
  sub_loop(Loop, [], [], Sub, Rest),
  reverse(Sub, Res).

sub_loop([], _, Res, Res, []).
sub_loop([H|T], Brs, Acc, Res, Rest) :-
  open(H),
%  write('open ['), nl,
  sub_loop(T, [H|Brs], [H|Acc], Res, Rest).
sub_loop([H|T], [_|Brs], Acc, Res, Rest) :-
  not(empty_list(Brs)),
  closed(H),
%  write('closed ], continuing'), nl,
  sub_loop(T, Brs, [H|Acc], Res, Rest).
sub_loop([H|T], [Br], Acc, [H|Acc], T) :- %%last bracket in the list
  closed(H),
  open(Br).
%  write('closed ], final'), nl.
sub_loop([H|T], Brs, Acc, Res, Rest) :-
  not(closed(H)),
  not(open(H)),
%  write('instr '), write(H), nl,
  sub_loop(T, Brs, [H|Acc], Res, Rest).
