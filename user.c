#include<stdio.h>
#include<stdlib.h>
#include<dlfcn.h>
#include<string.h>
#include<unistd.h>
#include"sharedfile.h"

int main(int argc, char const *argv[])
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;
    char command[4][80], str[80], arr[1024];

    void *p = NULL;
    VirtualFS *fptr = NULL;
    VirtualFS* (*fp1)() = NULL;
    void (*fp2)(VirtualFS *) = NULL;
    p = dlopen("/mnt/f/C Project/Virtual-File-System/VirtualFS.so",RTLD_LAZY);
    if(!p)
    {
        fprintf(stderr, "%s", dlerror());
        exit(1);
    }    
    else
    {
        fp1 = (VirtualFS*(*)()) dlsym(p,"create");
        fp2 = (void(*)(VirtualFS *)) dlsym(p,"destroy");
        fptr = fp1();

        fptr->initSuperBlock();
        fptr->initInodeTable();
        while(1)
        {
            fflush(stdin);
            strcpy(str, "");
            printf("\nVFS:>");
            fgets(str, 80, stdin);
            count = sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]);
            if(count == 1)
            {
                if(strcasecmp(command[0],"exit") == 0)
                {
                    break;
                }
                else if(strcasecmp(command[0],"ls") == 0)
                {
                    fptr->lsFile();
                    continue;
                }
                else if(strcasecmp(command[0],"closeall") == 0)
                {
                    fptr->closeAllFile();
                    printf("\nAll files closed successfully");
                    continue;
                }
                else if(strcasecmp(command[0],"clear") == 0)
                {
                    system("clear");
                    continue;
                }
                else if(strcasecmp(command[0],"help") == 0)
                {
                    fptr->displayHelp();
                    continue;
                }
                else
                {
                    printf("\n ERROR: Command Not Found");
                    continue;
                }
            }
            else if(count == 2)
            {
                if(strcasecmp(command[0],"stat") == 0)
                {
                    ret = fptr->statFile(command[1]);
                    if(ret == -1)
                        printf("\n ERROR: Incorrect parameters");
                    else if( ret == -2)
                        printf("\n ERROR: There is no such file");
                    else
                        continue;
                }
                else if(strcasecmp(command[0],"fstat") == 0)
                {
                    ret = fptr->fstatFile(atoi(command[1]));
                    if(ret == -1)
                        printf("\n ERROR: Incorrect parameters");
                    else if( ret == -2)
                        printf("\n ERROR: There is no such file");
                    else
                        continue;
                }
                else if(strcasecmp(command[0],"close") == 0)
                {
                    ret = fptr->closeFileByName(command[1]);
                    if(ret == -1)
                        printf("\n ERROR: Incorrect parameters");
                    else if( ret == -2)
                        printf("\n ERROR: There is no such file");
                    else
                        continue;
                }
                else if(strcasecmp(command[0],"rm") == 0)
                {
                    ret = fptr->rmFile(command[1]);
                    if(ret == -1)
                        printf("\n ERROR: There is no such file");
                    continue;
                }
                else if(strcasecmp(command[0],"man") == 0)
                {
                    fptr->man(command[1]);
                    continue;
                }
                else if(strcasecmp(command[0],"truncate") == 0)
                {
                    ret = fptr->truncateFile(command[1]);
                    if(ret == -1)
                        printf("\n ERROR: There is no such file");
                    continue;
                }
                else if(strcasecmp(command[0],"write") == 0)
                {
                    fd = fptr->getFDFromName(command[1]);
                    if(fd == -1)
                    {
                        printf("\n ERROR: Incorrect parameter");
                        continue;
                    }
                    printf("\n Enter the data:\n");
                    scanf("%[^\n]",arr);
                    ret = strlen(arr);
                    if(ret == 0)
                    {
                        printf("\n ERROR: Incorrect Data");
                        continue;
                    }
                    ret = fptr->writeFile(fd, arr, ret);
                    if(ret == -1)
                    {
                        printf("\n ERROR: Permission denied");
                        continue;
                    }
                    else if(ret == -2)
                    {
                        printf("\n ERROR: There is no sufficient memory to erite");
                        continue;
                    }
                    else if(ret == -3)
                    {
                        printf("\n ERROR: It is not reguular file");
                        continue;
                    }
                    else
                        continue;
                    continue;
                }
                else
                {
                    printf("\n ERROR: Command Not Found");
                    continue;
                }
            }
            else if(count == 3)
            {
                if(strcasecmp(command[0],"create") == 0)
                {
                    ret = fptr->createFile(command[1],atoi(command[2]));
                    if(ret >= 0)
                        printf("\n File successfully created with the file descriptor: %d ",ret);
                    else if(ret == -1)
                        printf("\n ERROR: Incorrect parameter");
                    else if(ret == -2)
                        printf("\n ERROR: There is no inode available");
                    else if(ret == -3)
                        printf("\n ERROR: File already exists");
                    else if(ret == -4)
                        printf("\n ERROR: Memory allocation failure");
                    else
                        printf("\n ERROR: Please try again");
                    continue;
                }
                else if(strcasecmp(command[0],"open") == 0)
                {
                    ret = fptr->openFile(command[1],atoi(command[2]));
                    if(ret >= 0)
                        printf("\n File is successfully opened with file descriptor: %d",ret);
                    else if(ret == -1)
                        printf("\n ERROR: Incorrect parameter");
                    else if(ret == -2)
                        printf("\n ERROR: file not present");
                    else if(ret == -3)
                        printf("\n ERROR: Permission denited");
                    continue;
                }
                else if(strcasecmp(command[0],"read") == 0)
                {
                    fd = fptr->getFDFromName(command[1]);
                    if(fd == -1)
                    {
                        printf("\n ERROR: Incorrect parameter");
                        continue;
                    }
                    ptr = (char *)malloc(sizeof(atoi(command[2])+1));
                    if(ptr == NULL)
                    {
                        printf("\n ERROR: Memory allocation failure");
                        continue;
                    }
                    ret = fptr->readFile(fd, ptr, atoi(command[2]));
                    if(ret == -1)
                        printf("\n ERROR: File not existing");
                    if(ret == -2)
                        printf("\n ERROR: Permission denied");
                    if(ret == -3)
                        printf("\n ERROR: Reached at the end of file");
                    if(ret == -4)
                        printf("\n ERROR: It is not regular file");
                    if(ret == 0)
                        printf("\n ERROR: File empty");
                    if(ret > 0)
                        write(2, ptr, ret);
                    continue;
                }
                else
                {
                    printf("\n ERROR: Command not found");
                    continue;
                }
            }
            else if(count == 4)
            {
                if(strcasecmp(command[0],"lseek") == 0)
                {
                    fd = fptr->getFDFromName(command[1]);
                    if(fd == -1)
                    {
                        printf("\n ERROR: Incorrect parameter");
                        continue;
                    }
                    ret = fptr->lseekFile(fd, atoi(command[2]), atoi(command[3]));
                    if(ret == -1)
                    {
                        printf("\n ERROR: Unable to perform lseek");
                        continue;
                    }
                }
                else
                {
                    printf("\n ERROR: Command not found");
                    continue;
                }
            }
            else
            {
                printf("\nERROR: Command not found");
                continue;
            }
        }


        fp2(fptr);
        dlclose(p);
    }
    return 0;
}
