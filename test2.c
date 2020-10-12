#define forever while(1)

int main() {
    int i = 0;
    forever {
        printf("%i\n", i);
        i++;
    }
    return 0;
}
