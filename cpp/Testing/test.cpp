#include <stdio.h>
#include <string>
#include<iostream>
using namespace std;
void compare_two_binary_files(FILE *,FILE *);

int main()
{
    FILE *fp1, *fp2;
    string f1 = "Data11.bin";
    string f2 = "Data1.bin";
    f1.erase(f1.begin(), f1.begin()+4);
    f1.erase(f1.end()-4, f1.end());
    cout<<f1;
    // fp1 = fopen(f1.c_str(),  "r");
    // if (fp1 == NULL)
    // {
    //     printf("\nError in opening file %s", f1.c_str());
    //     return 0;
    // }
    // fp2 = fopen(f2.c_str(), "r");
    // if (fp2 == NULL)
    // {
    //     printf("\nError in opening file %s", f2.c_str());
    //     return 0;
    // }
    // if ((fp1 != NULL) && (fp2 != NULL))
    // {
    //     compare_two_binary_files(fp1, fp2);
    // }
}
void compare_two_binary_files(FILE *fp1, FILE *fp2)
{
    char ch1, ch2;
    int flag = 0;
    while (((ch1 = fgetc(fp1)) != EOF) &&((ch2 = fgetc(fp2)) != EOF))
    {
        if (ch1 == ch2)
        {
            flag = 1;
            continue;
        }
        else
        {
            fseek(fp1, -1, SEEK_CUR);        
            flag = 0;
            break;
        }
    }
    if (flag == 0)
    {
        printf("Not equal");
    }
    else
    {
        printf("Equal");
    }
}