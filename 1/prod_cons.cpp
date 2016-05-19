#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <time.h>

using namespace std;
HANDLE sres;
HANDLE osp;
int res=10;

DWORD WINAPI pro(LPWORD arg)
{
	srand(time(NULL));
	while(1)
	{		
		int k=1+(GetCurrentThreadId()%7+rand())%8;
		Sleep(1000*k);
		WaitForSingleObject(osp,INFINITE);
		//cout<<"rand= "<<k<<endl;
			if(ReleaseSemaphore(sres,1,NULL))
			{
				res++;
				cout<<"res inc to "<<res<<endl;
			}
			else
			{
				cout<<"these is no place for resources :("<<endl;
			}
		ReleaseSemaphore(osp,1,NULL);
	}
	return 0;
}

DWORD WINAPI con(LPWORD arg)
{	
	srand(time(NULL));
	while(1)
	{
		
		int k=1+(GetCurrentThreadId()%7+rand())%8;
		Sleep(1000*k);
		
		WaitForSingleObject(osp,INFINITE);
			//cout<<"rand= "<<k<<endl;
			if(WaitForSingleObject(sres,8)==0)
			{
				res--;
				cout<<"res dec to "<<res<<endl;
			}
			else
			{
				cout<<"consumer need some more resources :("<<endl;
			}
		ReleaseSemaphore(osp,1,NULL);
	}
	return 0;	
}


int main()
{
	int p,c;
	cout<<"producers= ";
	cin>>p;
	cout<<"consumers= ";
	cin>>c;
	
	DWORD *tc, *tp;
	tc=new DWORD[c];
	tp=new DWORD[p];
		
	DWORD dwThrdParam = 1;
	HANDLE *htc,*htp;
	htc=new HANDLE[c];
	htp=new HANDLE[p];
	
	sres=CreateSemaphore(NULL,10,31,"sres");
	osp=CreateSemaphore(NULL,1,1,"osp");
	
	for(int i=0;i<p;i++)
	{
		htc[i]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)pro,0,0,tc+i);
		Sleep(100);
	}
	for(int i=0;i<c;i++)
	{
		htp[i]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)con,0,0,tp+i);
		Sleep(100);
	}
		
	for(int i=0;i<c;i++)
		WaitForSingleObject(htc[i],INFINITE);
	for(int i=0;i<p;i++)
		WaitForSingleObject(htp[i],INFINITE);
	
	system("PAUSE");
	
	return 0;
}
