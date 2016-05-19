#include <QCoreApplication>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <QMutex>

using namespace std;

struct arg
{
    int pn;
    int pa;
    pthread_mutex_t *mtx;
    pthread_cond_t *ar_w;
    bool *ar_f;
};

void* spawn_phil(void *fa)
{

    arg *ra=static_cast<arg*>(fa);
    pthread_cond_broadcast(ra->ar_w+ra->pn);
    int i=0;
    int ff,sf;
    if(ra->pn==ra->pa-1)
    {
        ff=0;
        sf=ra->pn;
    }
    else
    {
        ff=ra->pn;
        sf=ra->pn+1;
    }
    while (1)
    {
        sleep(2+rand()%4);
        pthread_mutex_lock(ra->mtx);
        bool tmp=*(ra->ar_f+ff);

        while(tmp)
        {
            cout<<"p"<<ra->pn<<" is sleeping..."<<endl;
            pthread_mutex_unlock(ra->mtx);
            pthread_cond_wait(ra->ar_w+ra->pn,ra->mtx);
            pthread_mutex_unlock(ra->mtx);
            sleep(2+rand()%4);
            pthread_mutex_lock(ra->mtx);
            tmp=*(ra->ar_f+ff);
        }
        *(ra->ar_f+ff)=true;
        cout<<"p"<<ra->pn<<": nice fork"<<endl;

        pthread_mutex_unlock(ra->mtx);
        sleep(2+rand()%4);
        pthread_mutex_lock(ra->mtx);

        tmp=*(ra->ar_f+sf);
        while(tmp)
        {
            cout<<"p"<<ra->pn<<" waiting for a second fork..."<<endl;
            pthread_mutex_unlock(ra->mtx);
            pthread_cond_wait(ra->ar_w+ra->pn,ra->mtx);
            pthread_mutex_unlock(ra->mtx);
            sleep(2+rand()%4);
            pthread_mutex_lock(ra->mtx);
            tmp=*(ra->ar_f+sf);

        }
        *(ra->ar_f+sf)=true;

        cout<<"p"<<ra->pn<<": Om-nom-nom..."<<endl;
        pthread_mutex_unlock(ra->mtx);
        sleep(4+rand()%8);
        pthread_mutex_lock(ra->mtx);

        *(ra->ar_f+ff)=false;
        *(ra->ar_f+sf)=false;

        if(ra->pn==0)
            pthread_cond_broadcast(ra->ar_w+ra->pa-1);
        else
            pthread_cond_broadcast(ra->ar_w+ra->pn-1);

        if(ra->pn==ra->pa-1)
            pthread_cond_broadcast(ra->ar_w);
        else
            pthread_cond_broadcast(ra->ar_w+ra->pn+1);

        cout<<"p"<<ra->pn<<": Thanks!"<<endl;
        pthread_mutex_unlock(ra->mtx);
    }
    pthread_mutex_lock(ra->mtx);
    cout<<"p"<<ra->pn<<" is dead!";
    pthread_mutex_unlock(ra->mtx);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    pthread_t *ar_p;
    arg *ar_ag;
    pthread_cond_t *ar_w;
    pthread_mutex_t mtx;
    bool *ar_f;

    pthread_mutex_init(&mtx,NULL);

    int pa;
    cout<<"Кол-во философов: ";
    cin>>pa;

    ar_p=new pthread_t[pa];
    ar_f=new bool[pa];
    ar_w=new pthread_cond_t[pa];
    ar_ag=new arg[pa];

    for(int i=0;i<pa;i++)
    {
        pthread_cond_init(ar_w+i,NULL);
        ar_f[i]=false;
        ar_ag[i].ar_f=ar_f;
        ar_ag[i].pn=i;
        ar_ag[i].pa=pa;
        ar_ag[i].mtx=&mtx;
        ar_ag[i].ar_w=ar_w;
    }

    for(int i=0;i<pa;i++)
    {
        pthread_create(ar_p+i,NULL,&spawn_phil,ar_ag+i);
    }

    for(int i=0;i<pa;i++)
    {
        pthread_join(ar_p[i],NULL);
    }


    return 0;
    return a.exec();
}
