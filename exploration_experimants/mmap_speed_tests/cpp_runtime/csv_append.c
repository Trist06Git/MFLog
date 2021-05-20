
#include "csv_append.h"

/////WARNING!
//This currently only supports the same format for a whole line.

//Inefficient, but minimises the amount of "variables" in scope when
//doing benchmarks
//expecting a fresh new line
bool append_csv(const char* filename, vector* elements, vector* formats) {
    if (!is_csv(filename)) return false;
    if (formats != NULL && vec_size(elements) != vec_size(formats)) {
        printf("CSV :: Number of formats and elements don't match.\n");
        return false;
    }
    FILE* file = fopen(filename, "a+");
    if (file == NULL) {
        perror("CSV :: Could not append to file \"%s\" :\n");
        return false;
    }
    int el_count = vec_size(elements);
    for (int i = 0; i < el_count; i++) {
        void* el = vec_at(elements, i);
        if (formats != NULL) {    
            enum ca_format* fmt = (enum ca_format*)vec_at(formats, i);
            if (*fmt == CA_INT) {
                int* i_el = (int*)el;
                fprintf(file, "%i", *i_el);
            } else if (*fmt == CA_FLOAT) {
                float* f_el = (float*)el;
                fprintf(file, "%.3f", *f_el);
            } else if (*fmt == CA_BOOL) {
                bool* b_el = (bool*)el;
                fprintf(file, "%s", *b_el?"true":"false");
            } else {
                printf("CSV :: Unknown type in format vector\n");
                return false;
            }
        } else {//default int
            int* i_el = (int*)el;
            fprintf(file, "%i", *i_el);
        }
        if (i != el_count-1) fprintf(file, "%s", ", ");
    }
    fprintf(file, "%s", "\n");
    fclose(file);
    return true;
}

//only checks .csv extension
bool is_csv(const char* filename) {
    if (strlen(filename) < 5 || strcmp(filename+strlen(filename)-4, ".csv") != 0) {//a.csv
        printf("CSV :: Wrong extension for file %s\n", filename);
        return false;
    }
    return true;
}
