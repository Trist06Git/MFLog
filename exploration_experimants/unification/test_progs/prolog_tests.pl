
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

main(R) :-
    j(X, A, R),
    j(Y, B, R),
    X = 1,
    Y = 3,
    A = 2,
    B = 4,
    R = 5.

j(1, 2, V) :- write(V), nl.
j(3, 4, V) :- write(V), nl.
