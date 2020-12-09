
typedef struct t_word {
    char* word;
} t_word;

typedef struct t_number {
    int number;
} t_number;

enum e_token {
    E_WORD, E_NUMBER, E_OTHER
};
typedef struct t_token {
    enum e_token tag;
    union u_token {
        t_word w;
        t_number n;
    } u_token;
} t_token;

//char* token_to_string(enum e_token);
//char* token_to_string(t_token token);
