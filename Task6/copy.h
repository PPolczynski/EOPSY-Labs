#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define __BUFFERSIZE 1024

enum CopyOptionsEnum
{
    None = 0,
    UsingMmap = 1,
    Help = 2,
    InvalidOption = 4,
    TooFewArgs = 8
} typedef CopyOptionsEnum;

void printHelp(const CopyOptionsEnum* cpyOption);
CopyOptionsEnum getOptions(const int* argc,char*** argv);
int copyReadWrite(const int* orginalFile,const int* newFile);
int copyMmap(const int* orginalFile,const int* newFile);
void cleanUp(const int* result,const int* orginalFile,const int* newFile);
