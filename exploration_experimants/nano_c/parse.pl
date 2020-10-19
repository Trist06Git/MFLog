
:- consult('file_io.pl').

func_def(Name, Params, E).
func_call(Name, Params).
func_call(Name, Params).

params_def([H|T], Res) :-
  P:= "int " ++ H,
  params_def(T, P, Res).
params_def([], Res, Res).
params_def([H|T], Acc, Res) :-
  P0:=Acc ++ ", int ", P1:=P0 ++ H,
  params_def(T, P1, Res).

params_pass([H|T], Res) :-
  parse_expr(H, A),
  params_pass(T, A, Res).
params_pass([], Res, Res).
params_pass([H|T], Acc, Res) :-
  parse_expr(H, A),
  P:=Acc ++ ", ", P1:=P ++ A,
  params_pass(T, P1, Res).

atm(A).
oper(+).
oper(-).
oper(*).
oper(/).
oper('%').
exp(=, exp(A), exp(B)).
%exp(+, exp(A), exp(B)).
%exp(-, exp(A), exp(B)).
exp(oper(O), exp(A), exp(B)).
exp(func_call(Name, Params)).
exp(return, exp(A)).
exp(atm(A)).
exp_end(exp(E), ';').
%exp(atm(A)).

%parse_expr(exp(A), A).
parse_expr(atm(A), A).
parse_expr(exp(=, E1, E2), R1) :-
  parse_expr(E1, A1),
  parse_expr(E2, A2),
  R:= A1 ++ " = ", R1:=R ++ A2.
parse_expr(exp(Oper, E1, E2), R3) :-
  oper(Oper),
  parse_expr(E1, A1),
  parse_expr(E2, A2),
  R:= A1 ++ " ", R1:=R ++ Oper, R2:=R1 ++ " ", R3:=R2 ++ A2.
parse_expr(exp(func_call(Name, Params)), P2) :-
  params_pass(Params, Paras),
  P:= Name ++ "(", P1:=P ++ Paras, P2:=P1 ++ ")".
parse_expr(exp(return, E), R) :-
  parse_expr(E, Exp),
  R:= "return " ++ Exp.

parse_expr_end(exp_end(E,';'), R1) :-
  parse_expr(E, R),
  R1:=R ++ ";\n".

defs(func_def(_,_,_), defs(_)).
defs(func_def(_,_,_)).

parse_def(defs(D1, D2), R2) :-
  parse_def(defs(D1), R),
  parse_def(D2, R1),
  R2:=R ++ R1.

parse_def(defs(func_def(Name, Params, E)), R5) :-
  parse_expr_end(E, Exp),
  params_def(Params, ParamsString),
  R0:= "int " ++ Name, R1:=R0++ "(", R2:=R1++ ParamsString, R3:=R2 ++ ") {\n",
  R4:=R3 ++ Exp,
  R5:=R4 ++ "}\n".

:- write("Started"), nl.


:- write("Done"), nl.
%:- halt.


/*
parse_def(defs(func_def(f,[x],exp_end(exp(return,atm(1)),';')), defs(func_def(h,[y],exp_end(exp(return,atm(2)),';' ) ))), Res), write(Res).


parse_def(
  defs(
    func_def(
      f,[x],exp_end(
        exp(return, exp(+,atm(x),atm(1))),
      ';')
    ),
    defs(
      func_def(
        h,[y],exp_end(
          exp(return, exp(+,exp(func_call(f,[atm(y)])),atm(2))),
        ';')
      )
    )
  ),
Res), write(Res).

parse_def(
  defs(
    func_def(
      f,[x],exp_end(
        exp(return, atm(1)),
      ';')
    ),
    defs(
      func_def(
        h,[y],exp_end(
          exp(return,atm(2)),
        ';')
      )
    )
  ),
Res), write(Res).
*/
