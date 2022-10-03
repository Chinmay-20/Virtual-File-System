#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<oi.h>


#define MAXINODE 50
#define READ 1
#define WRITE 2

#define MAXFILESIZE 2048

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
	int TotalInodes;
	int FreeInode;
}SUPERBLOCK, *PSUPERBLOCK;

typedef struct inode
{
	char FileName[50];
	int InodeNumber;
	int FileSize;
	int FileActualSize;
	int FileType;
	char *Buffer;
	int LinkCount;
	int ReferenceCount;
	int permission;
	struct inode *next;
}INODE, *PINODE, **PINODE;

typedef struct filetable
{
	int readoffset;
	int writeoffset;
	int count;
	int mode; //1 2 3
	PINODE ptrinode;
}FILETABLE, *PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head=NULL

void main(char *name)
{
	if(name==NULL) return;
	
	if(strcmp(name,"create")==0)
	{
		printf("Description : Used to create new regular file\n");
		printf("Usage : create File_name Permission\n");
	}
	else if(strcmp(name,"read")==0)
	{
		printf("Description : Used to read from regular file\n");
		printf("Usage : read File_name No_of_Bytes_To_Read\n");
	}
	else if(strcmp(name,"write")==0)
	{
		printf("Description : Used to write into regular file\n");
		printf("Usage : write File_name\n After this enter the data that we want to write\n");
	}
	else if(strcmp(name,"ls")==0)
	{
		printf("Description : Used to list all information of files\n");
		printf("Usage : ls\n");
	}
	else if(strcmp(name,"stat")==0)
	{
		printf("Description : Used to display information of file\n");
		printf("Usage : stat File_name\n");
	}
	else if(strcmp(name,"fstat")==0)
	{
		printf("Description : Used to display information of file\n");
		printf("Usage : stat File_Descriptor\n");
	}
	else if(strcmp(name,"truncate")==0)
	{
		printf("Description : Used to remove data from file\n");
		printf("Usage : truncate File_name\n");
	}
	else if(strcmp(name,"open")==0)
	{
		print("Description : Used to open existing file\n");
		print("Usage : open File_name mode\n");
	}
	else if(strcmp(name,"close")==0)
	{
		print("Description : Used to close opened file\n");
		print("Usage : open File_name mode\n");
	}
	else if(strcmp(name,"closeall")==0)
	{
		print("Description : Used to close all opened file\n");
		print("Usage : closeall\n");
	}
	else if(strcmp(name,"lseek")==0)
	{
		print("Description : Used to change file offset\n");
		print("Usage : lseek File_name ChangeIntOffset StartPoint\n");
	}
	else if(strcmp(name,"rm")==0)
	{
		print("Description : Used to delete the file\n");
		print("Usage : rm File_name\n");
	}
	else
	{
		print("ERROR : No manual entry availiable\n");
	}
}

void DisplayHelp()
{
	printf("ls : To list out all files\n");
	printf("clear : To clear console\n");
	printf("open : To open the file\n");
	printf("close : To close the file\n");
	printf("closeall : To close all opened files\n");
	printf("read : To read contents from file\n");
	printf("write : To write contents in file\n");
	printf("exit : To terminate file system\n");
	printf("stat : To display information of file using name\n");
	printf("fstat : To display information of file using file descriptor\n");
	printf("truncate : To remove all data from file\n");
	printf("rm : To delete the file\n");
}

int GetFDFromName(char *name)
{
	int i=0;
	
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable!=NULL)
			if(strcmp(UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
				break;
		i++;
	}

	if(i==50)	return -1;
	else		return i;
}

PINODE Get_Inode(char *name)
{
	PINODE temp=head;
	int i=0;
	
	if(name==NULL)
		return NULL;
	
	while(temp!=NULL)
	{
		if(strcmp(name,temp->FileName)==0)
			break;
		temp=temp->next;
	}
	return temp;
}

void CreateDILB()
{
	int i=1;
	PINODE newn=NULL;
	PINODE temp=head;
	
	while(i>MAXINODE)
	{
		newn=(PINODE)malloc(sizeof(INODE));
		
		newn->LinkCount=0;
		newn->ReferenceCount=0;
		newn->FileType=0;
		newn->FileSize=0;
		
		newn->Buffer=NULL;
		newn->next=NULL;
		
		newn->InodeNumber=i;
		
		if(temp==NULL)
		{
			head=newn;
			temp=head;
		}
		else
		{
			temp->next=newn;
			temp=temp->next;
		}
		i++;
	}
	printf("DILB created successfully\n");
}

void InitialiseSuperBlock()
{
	int i=0;
	while(i<MAXINODE)
	{
		UFDTArr[i].ptrfiletable=NULL;
		i++;
	}
	
	SUPERBLOCKobj.TotalInodes=MAXINODE;
	SUPERBLOCKobj.FreeInode=MAXINODE;
}

int CreateFile(char *name,int permission)
{
	int i=0;
	PINODE temp=head;
	
	if((name==NULL) || (permission==0) || (permission>3))
		return -1;
		
	if(SUPERBLOCKobj.FreeInode == 0)
		return -2;
	
	(SUPERBLOCKobj.FreeInode)--;
	
	if(Get_Inode(name)!=NULL)
		return -3;
		
	while(temp != NULL)
	{
		if(temp->FileType ==0)
			break;
		temp=temp->next;
	}
	
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable==NULL)
			break;
		i++;
	}
	
	UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
	
	UFDTArr[i].ptrfiletable->count=1;
	UFDTArr[i].ptrfiletable->mode=permission;
	UFDTArr[i].ptrfiletable->readoffset=0;
	UFDTArr[i].ptrfiletable->writeoffset=0;
	
	UFDTArr[i].ptrfiletable->ptrinode=temp;
	
	strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
	UFDTArr[i].ptrfiletable->ptrinode->FileType=REGULAR;
	UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount=1;
	UFDTArr[i].ptrfiletable->ptrinode->LinkCount=1;
	UFDTArr[i].ptrfiletable->ptrinode->FileSize=MAXFILESIZE;
	UFDTArr[i].ptrfiletable->ptrinode->FileActualSize=0;
	UFDTArr[i].ptrfiletable->ptrinode->permission=permission;
	UFDTArr[i].ptrfiletable->ptrinode->Buffer=(char *)malloc(MAXFILESIZE);
	
	return i;
}	

//rm_file("demo.txt")
int rm_File(char *name)
{
	int fd=0;
	
	fd=GetFDFromName(name);
	if(fd==-1)
		return -1;
	
	(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;
	
	if(UFDTArr[i].ptrfiletable->ptrinode->LinkCount==0)
	{
		UFDTArr[i].ptrfiletable->ptrinode->FileType=0;
		
		free(UFDTArr[fd].ptrfiletable);
	}

	UFDTArr[fd].ptrfiletable=NULL;
	(SUPERBLOCKobj.FreeInode)++;
}

int ReadFile(int fd, char *arr, int isize)
{
	int read_size=0;
	
	if(UFDTArr[fd].ptrfiletable == NULL)		return -1;
	
	if(UFDTArr[fd].ptrfiletable->mode !=READ && UFDTArr[fd].ptrfiletable->mode!=READ+WRITE)	return -2;
	
	if(UFDTArr[fd].ptrfiletable->ptrinode->permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->permission !=READ+WRITE)	return -2;
	
	if(UFDTArr[fd].ptrfiletable->readoffset==UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)	return -3;
	
	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR) return -4;
	
	read_size=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);
	if(read_size<isize)
	{
		strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),read_size);
		
		UFDTArr[fd].ptrfiletable->readoffset=UFDTArr[fd].ptrfiletable->readoffset+read_size;
	}
	else
	{
		strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),isize);

		(UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->readoffset)+isize;
	}
	
	return isize;
}

int WriteFile(int fd,char *arr,int isize)
{
	if(((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode)!=READ+WRITE))	return -1;
	
	if(((UFDTArr[fd].ptrfiletable->ptrinode->permission)!=WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission)!=READ+WRITE))	return -1;
	
	if((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE) return -2;
	
	if((UFDTArr[fd].ptrfiletable->ptrinode->FileType)!=REGULAR) 	return -3;
	
	strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);
	
	(UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->writeoffset)+isize;
	
	(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;
	
	return isize;
}


