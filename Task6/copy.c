#include "copy.h"

int main(int argc, char** argv)
{
    int result = 0;
    CopyOptionsEnum cpyOption = getOptions(&argc, &argv);
    if(cpyOption & (Help | InvalidOption))
    {
        printHelp(&cpyOption);
        return result;
    }
    else
    {
        int orginalFile = 0;
        int newFile = 0;
        if((orginalFile = open(argv[argc-2],O_RDONLY)) == -1)
        {
            fprintf(stderr,"File: %s can not be open for reading\n",argv[argc-2]);
            perror("File");
            result = 1;
        }
        if(result == 0 && (newFile = open(argv[argc-1], O_RDWR | O_CREAT, 0666)) == -1)
        {
            fprintf(stderr,"File: %s can not be created or opened\n",argv[argc-1]);
            perror("File");
            result = 2;
        }
        if(result == 0 && cpyOption&UsingMmap)
        {
            result = copyMmap(&orginalFile,&newFile);
        }
        else
        {
            result = copyReadWrite(&orginalFile,&newFile);
        }
        cleanUp(&result,&orginalFile,&newFile);
    }
    return result;
}

CopyOptionsEnum getOptions(const int* argc,char*** argv)
{
    char optionChr;
    CopyOptionsEnum cpyOption = None;

    opterr = 0; // Supress getopt() error output
    while((optionChr = getopt(*argc, *argv, "hm")) != -1)
    {
        switch (optionChr)
        {
        case 'h':
            cpyOption |= Help;
            break;
        case 'm':
            cpyOption |= UsingMmap;
            break;
        case '?':
            cpyOption |= InvalidOption;
            break;
        }
    }
    if(((*argc < 4) && (cpyOption & UsingMmap)) || ((*argc < 3) && (cpyOption == None)))
    {
        cpyOption |= TooFewArgs | Help;
    }

    return cpyOption;
}

void printHelp(const CopyOptionsEnum* cpyOption)
{
    if(*cpyOption & TooFewArgs)
    {
        printf("Too few arguments for copy\n");
    }
    if(*cpyOption & InvalidOption)
    {
        printf("Invalid parameter supplied. Valid parameters are -h or -m\n");
    }
    printf("Usage:\n\
		copy <orginal file name> <new file name>\n\
		copy -m <orginal file name> <new file name>\n\n\
		By Pawel Polczynski\n");
}

int copyReadWrite(const int* orginalFile,const int* newFile)
{

    char buf[__BUFFERSIZE];
    int in;
    while((in = read(*orginalFile, buf, __BUFFERSIZE)) > 0)
    {
        write(*newFile, buf,in);
    }
    return 0;
}

int copyMmap(const int* orginalFile,const int* newFile)
{
    struct stat orginalFileInfo;
    int *orginalMap;
    int *newMap;
    if(fstat(*orginalFile, &orginalFileInfo) == -1)
    {
        perror("fstat");
        return(4);
    }
    if((orginalMap = mmap(NULL, orginalFileInfo.st_size, PROT_READ, MAP_PRIVATE, *orginalFile, 0)) == MAP_FAILED)
    {
        perror("ERR orginal file\nmmap");
        return(5);
    }
    if((newMap = mmap(NULL, orginalFileInfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, *newFile, 0))== MAP_FAILED)
    {
        perror("ERR new file\nmmap");
        munmap(orginalMap,orginalFileInfo.st_size);
        return(6);
    }
    if(ftruncate(*newFile, orginalFileInfo.st_size) == -1)
    {
        perror("ftruncate");
        munmap(orginalMap,orginalFileInfo.st_size);
        return(7);
    }
    memcpy(newMap, orginalMap, orginalFileInfo.st_size);
    if(msync(orginalMap, orginalFileInfo.st_size, MS_SYNC) == -1)
    {
        perror("msync");
        munmap(orginalMap,orginalFileInfo.st_size);
        munmap(newMap,orginalFileInfo.st_size);
        return(8);
    }
    munmap(orginalMap,orginalFileInfo.st_size);
    munmap(newMap,orginalFileInfo.st_size);
    return 0;
}

void cleanUp(const int* result,const int* orginalFile,const int* newFile)
{
    if (*result != 1 || *result != 2)
    {
        close(*orginalFile);
        close(*newFile);
    }
    else if (*result != 2)
    {
        close(*orginalFile);
    }
}

