
#include <stdio.h>
#include "flicker.h"

int main(int argc, char *argv[])
{
    for (int i = 1; i <= 100; ++i)
    {
        generate_flicker();

        printf("\n--- cycle %d ---\n", i);
        printf("Steps: %d\n", get_flicker_steps());

        for (int j = 0; j < get_flicker_steps(); ++j)
        {
            printf("%d ", get_flicker_table()[j]);
            if (j % 32 == 0 && j != 0)
            {
                printf("\n");
            }
        }

        printf("\n");
    }
}