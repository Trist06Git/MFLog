
f(X) :-
    g(X, R),
    write(R), nl.

g(1, 1).
g(2, 2).
g(3, 3).

add(X, Y, Z) :-
    Z is X + Y.

h(R) :-
    i(X, R),
    X = 1.

i(1, 2).
i(3, 4).

main_old(R) :-
    j(X, A, R),
    j(Y, B, R),
    X = 1,
    Y = 3,
    A = 2,
    B = 4,
    R = 5.

j(1, 2, V) :- write(V), nl.
j(3, 4, V) :- write(V), nl.

age(whiskey, 6).
age(tristan, 31).
age(_, -1). /*X is same as _*/

print_age(Person, Age) :-
    write(Person),
    write(" is "),
    write(Age),
    nl.

main_old2(Person) :-
    age(Person, Age),
    print_age(Person, Age).

my_plus(Xi, Yi, Rv) :-
    nonvar(Xi), nonvar(Yi),
    Rv is Xi + Yi.
my_plus(Xi, Yv, Ri) :-
    nonvar(Xi), nonvar(Ri),
    Yv is Ri - Xi.
my_plus(Xv, Yi, Ri) :-
    nonvar(Yi), nonvar(Ri),
    Xv is Ri - Yi.

loop(0, 0).
loop(X, R) :-
    write(X), nl,
    Step is X - 1,
    loop(Step, R).

bad_loop(3).
bad_loop(X) :-
  write(X),
  Step is X + 1,
  bad_loop(Step).

main(R) :-
    R = 0,
    bad_loop(R).

i(1).
i(2).

j(3).
j(4).

/*:- use_module(library(clpfd)).*/
