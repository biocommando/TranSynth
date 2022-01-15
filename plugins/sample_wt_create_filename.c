#include <stdio.h>

int main(int argc, char **argv)
{
    char s[32];
    printf("Type filename (max. 8 characters excluding .bin suffix): ");
    scanf("%s", s);
    printf("Param output:\n\n");
    for (int i = 0; i < 8 && s[i]; i++)
    {
        printf(";name%d=%d", i, (int)s[i]);
    }
    return 0;
}