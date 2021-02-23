
make_list(R) :-
    R = [x, 2, 3].

print_list([]).
print_list([x|T]) :-
    write("..."), nl,
    print_list(T).
print_list([H|T]) :-
    not(H = x),
    write(H), nl,
    print_list(T).
