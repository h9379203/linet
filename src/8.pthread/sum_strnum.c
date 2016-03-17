#include <stdio.h>
#include <stdlib.h>

char *sum_strnum(const char *s1, const char *s2)
{
    static char buf_sum[16];
    snprintf(buf_sum, sizeof(buf_sum), "%d", atoi(s1) + atoi(s2));
    return buf_sum;
}

int main()
{
    char *p_str;

    p_str = sum_strnum("1", "3");
    printf("1 + 3 = %s\n", p_str);
    p_str = sum_strnum("4", "4");
    printf("4 + 4 = %s\n", p_str);
    p_str = sum_strnum("1", "5");
    printf("1 + 5 = %s\n", p_str);
    return EXIT_SUCCESS;
}
