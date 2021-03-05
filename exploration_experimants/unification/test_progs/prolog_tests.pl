
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
