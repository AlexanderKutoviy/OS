#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

HANDLE queue;
HANDLE place;
HANDLE tout;
int qs=0;

DWORD WINAPI barber(LPWORD arg)
{	
	while(1)
	{
		WaitForSingleObject(queue,INFINITE);
		WaitForSingleObject(tout,INFINITE);
		qs--;
		cout<<"queue dec to "<<qs<<endl;
		ReleaseSemaphore(tout,1,NULL);
		ReleaseSemaphore(place,1,NULL);
		Sleep(4000);
	}
}

DWORD WINAPI client(LPWORD arg)
{
	if(ReleaseSemaphore(queue,1,NULL))
	{	
		WaitForSingleObject(tout,INFINITE);
		qs++;
		cout<<"queue inc to "<<qs<<endl;
		ReleaseSemaphore(tout,1,NULL);
		WaitForSingleObject(place,INFINITE);
	}
	else
	{
		WaitForSingleObject(tout,INFINITE);
		cout<<"client go home :("<<endl;
		ReleaseSemaphore(tout,1,NULL);
	}
	return 0;
}

int main()
{
	int n;
	cout<<"Queue size= ";
	cin>>n;
	//n++;
	
	DWORD tb,tc;
	//DWORD *tp=new DWORD[n];
	//for(int i=0;i<n;i++)
	//	tp[i];
	queue=CreateSemaphore(NULL,0,n,"queue");
	place=CreateSemaphore(NULL,0,1,"place");
	tout=CreateSemaphore(NULL,1,1,"tout");
	
	srand(time(NULL));
	
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)barber,0,0,&tb);
	while(1)
	{
		Sleep(1000*(1+rand()%4));
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)client,0,0,&tc);	
	}
	
	//HANDLE *htp=new DWORD[n];
	//for(int i=0;i<n;i++)
	//	htp[i];
	
	
	
	return 0;
}