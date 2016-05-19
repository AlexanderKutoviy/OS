#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

struct List
{
	List * next;
	char name[MAX_PATH];
	int size;
};

List * fList = new List;
List * fhead = fList;
List * ftail = NULL;

int counter;

void DeleteAll();

void AddToList(char name[MAX_PATH], int size)
{
	strcpy(fList->name, name);
	fList->size = size;
	fList->next = new List;
	fList = fList->next;
}

void SortList()
{
int tmp_i;
char tmp_c[MAX_PATH];
BOOL isChanged = FALSE; 
	while(isChanged == FALSE)
	{
		fList=fhead;
		while(fList->next != ftail)
		{
			if(fList->next->size < fList->size)
			{
				tmp_i = fList->size;
				strcpy(tmp_c, fList->name);
				fList->size = fList->next->size;
				strcpy(fList->name, fList->next->name);
				fList->next->size = tmp_i;
				strcpy(fList->next->name, tmp_c);
				fList = fList->next;
				isChanged=TRUE;
			}
			else fList = fList->next;
		}
		if(isChanged == TRUE) isChanged=FALSE;
		else isChanged=TRUE;
	}

}

void PrintFiles(int param)
{
int block1 = 0, block2 = 0, i = 1;
	fList = fhead;
	while(fList != ftail)
	{
		block1 = fList->size / param;
		if(block1 > block2) i++;
		printf("%24.24s\t%i\t%0i ||",fList->name,fList->size,i);
		for(int t = 0; t < i; t++) printf("*");
		printf("\n");
		fList = fList->next;
		block2 = block1;
	}
}

void FindFiles(char dir[MAX_PATH])
{
WIN32_FIND_DATA FileData; 
HANDLE hSearch; 
char str[MAX_PATH], path[MAX_PATH];
BOOL fFinished = FALSE; 
DWORD dwAttrs; 
	SetCurrentDirectory(dir);
	hSearch = FindFirstFile("*.*", &FileData);
	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		printf("Error 1\n"); 
		return;
	}
	while (!fFinished) 
	{ 
		strcpy(str, FileData.cFileName);
		dwAttrs = FileData.dwFileAttributes; 
		if ( (dwAttrs & FILE_ATTRIBUTE_DIRECTORY) && ((!strcmp(str, ".")) || (!strcmp(str, ".."))) )  ;//printf("*\t");
		else if (dwAttrs & FILE_ATTRIBUTE_DIRECTORY)
		{
			GetFullPathName(str, MAX_PATH, path, NULL);
			FindFiles(path);
			SetCurrentDirectory(dir);
		}
		else 
		{
			printf("%24.24s\t%u\n",str,FileData.nFileSizeLow);
			counter++;
			AddToList(str, FileData.nFileSizeLow);
		}
		if (!FindNextFile(hSearch, &FileData)) 
		{
	        if (GetLastError() == ERROR_NO_MORE_FILES)	
			{
				fFinished = TRUE; 
				return;
			}
		    else 
			{	
			    printf("Error 2"); 
				return;
			} 
		}
	}
}

int main(int argc, char* argv[])
{
char str[MAX_PATH];
	counter=0;
	GetCurrentDirectory(100,str);
	printf("%s\n\n",str);
	printf("%28.28s\t%s\n","*-Name-*","*-Size-*");
	FindFiles(str);
	ftail = fList;
	printf("\n%i files found\n",counter);
	SortList();
	printf("\n\nHystogram:\n\n");
	if(argc < 2) PrintFiles(1024);
	else PrintFiles(atoi(argv[1]));
	DeleteAll();
	system("Pause");
return 0;
}

void DeleteAll()
{
	fList = fhead;
	while(fList->next != ftail)
	{
		fhead = fList;
		fList = fList->next;
		fhead->next = NULL;
		delete fhead;
	}
	delete fList;
	delete ftail;
}
