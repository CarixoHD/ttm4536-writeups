#include <unistd.h>

int main() {
    write(1, "Welcome to the Pacman challenge!\n", 33);
    write(1, "It does absolutely nothing.\n", 28);
    return 0;
}