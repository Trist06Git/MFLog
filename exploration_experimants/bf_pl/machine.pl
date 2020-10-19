
:- load_files('./parse.pl').

incr(X, Res) :- Res is X+1.
decr(X, Res) :- Res is X-1.

shift_l([H], OldR, R, []) :-
  reverse([H|OldR], R).
shift_l([H|L], R, L, [H|R]) :-
  not(L = []).

shift_r(OldL, OldR, L, R) :-
  shift_l(L, R, OldL, OldR).

incr([OldH|L], R, [H|L], R) :-
  incr(OldH, H).
decr([OldH|L], R, [H|L], R) :-
  decr(OldH, H). 

state([_|_], []).
state([_|_], [_|_]).

inst('<'). inst('>').
inst('+'). inst('-').
inst('.'). inst(',').
loop('['). loop(']').

proc('<', state(L, R), state(ResL, ResR)) :-
  shift_l(L, R, ResL, ResR).
proc('>', state(L, R), state(ResL, ResR)) :-
  shift_r(L, R, ResL, ResR).
proc('+', state(L, R), state(ResL, ResR)) :-
  incr(L, R, ResL, ResR).
proc('-', state(L, R), state(ResL, ResR)) :-
  decr(L, R, ResL, ResR).
proc('.', state([Ptr|L], R), state([Ptr|L], R)) :-
  char_code(Char, Ptr), write(Char).

loop(_, state([0|L], R), state([0|L], R)).
loop([Br|Loop], state([Ptr|L], R), Res) :-
  not(Ptr = 0),
  proc_prog(Loop, state([Ptr|L], R), LoopRes),
  loop([Br|Loop], LoopRes, Res).

proc_prog([], Res, Res) :- nl, write('Done.'), nl.

proc_prog([']'], Res, Res). %%not 100% for now
proc_prog([Inst|T], State, Res) :-
  open(Inst),
%  write('opening loop : '), write(Inst), nl,
  sub_loop([Inst|T], Sub, Rest),
%  write('sub loop : '), write(Sub), nl,
  loop(Sub, State, LoopedState),
  proc_prog(Rest, LoopedState, Res).

proc_prog([Inst|T], State, Res) :-
  inst(Inst),
%  write('processing instruction : '), write(Inst), nl,
  proc(Inst, State, NewState),
  proc_prog(T, NewState, Res).

proc_prog([Ignore|T], State, Res) :-
  not(inst(Ignore)),
  not(loop(Ignore)),
%  write('ignoring char : '), write(Ignore), nl,
  proc_prog(T, State, Res).


%shift_r(L, [H|R], [H|L], R).

%%%%
% Tests

l([3, 2, 1]).
r([4, 5, 6]).

thing(X) :- X = 'test'.
