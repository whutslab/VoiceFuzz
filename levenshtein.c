#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int min(int x, int y)
{
    return x < y ? x : y;
}

int max(int x,int y)
{
    return x < y ? y : x;
}
//str_b should be the longer string
int levenshtein_distance(char *str_a, char *str_b)
{
    int len_a,len_b;
    len_a = strlen(str_a);
    len_b = strlen(str_b);
    if (len_a>len_b)
    {
        char *temp;
        temp = str_a;
        str_a = str_b;
        str_b = temp;

        int tem;
        tem = len_a;
        len_a = len_b;
        len_b = tem;
    }
    
    int matrix[len_b+1][len_b+1];
    int curr_i = 0;
    int len_sub_a;
    int len_sub_b;
    int len_sub_a_b;

    for (size_t i = 0; i < len_b+1; i++)
    {
        for (size_t j = 0; j < len_b+1; j++)
        {
            matrix[i][j]=0;
        }
        
    }
    

    for(size_t i = 0; i < len_a+1; i++)
    {
        for (size_t j = 0; j < len_b+1; j++)
        {
            if (min(i,j) == 0)
            {
                matrix[i][j] = max(i,j);
                continue;
            }
            len_sub_a = matrix[1 - curr_i][j] + 1;
            len_sub_b = matrix[curr_i][j - 1] + 1;
            if (str_a[i - 1] != str_b[j - 1])
            {
                len_sub_a_b = matrix[1 - curr_i][j - 1] + 1;
            }
            else
            {
                len_sub_a_b = matrix[1 - curr_i][j - 1];
            }
            matrix[curr_i][j] = min(len_sub_a, min(len_sub_b, len_sub_a_b));
        }
        curr_i = 1 - curr_i;
    }

    // for (size_t i = 0; i < len_b+1; i++)
    // {
    //     for (size_t j = 0; j < len_b+1; j++)
    //     {
    //         printf("%d ",matrix[i][j]);
    //     }
    //     printf("\n");
    // }
    return matrix[1 - curr_i][len_b];
}

int main(int argc, char const *argv[])
{
    FILE *f1,*f2;
    // char *WAV1,*WAV2;
    char WAV1[66958]={0},WAV2[32302]={0};
    char c;
    // WAV1 = malloc(sizeof(char)*66958);
    // WAV2 = malloc(sizeof(char)*32302);
    f1 = fopen("./0000.wav","rb");
    f2 = fopen("./0000_x2.wav","rb");
    for(int i=0;i<66958;i++)
    {
        WAV1[i]=fgetc(f1);
        // printf("%x",c);
        // WAV1[i]=c;

    }
    for(int i=0;i<32302;i++)
    {
        WAV2[i]=fgetc(f2);
    }

    // for(int i=0;i<66958;i++)
    // {
    //     printf("%x",WAV1[i]);

    // }

    printf("%d",levenshtein_distance(WAV1,WAV2));
    return 0;
}
