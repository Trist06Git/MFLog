
def loop(x):
    if x == 100:
        return 1
    print(x)
    y = x + 1
    return loop(y)

def main():
    loop(0)
    print()

main()
