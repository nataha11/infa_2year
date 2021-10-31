#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
    fprintf(stderr, "Тест 2\n");
    execlp(
        //path to executable
        "grep",
        //argv
        "grep", argc > 1 ? argv[1] : "model name", "/proc/cpuinfo", NULL
        );
    perror("failed to exec grep");
    return 0;
}
