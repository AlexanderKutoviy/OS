#include <QCoreApplication>
#include <pthread.h>
#include <iostream>
#include <unistd.h>

using namespace std;

struct arg
{
    int pn;
    int pa;
    pthread_mutex_t *lib;
    pthread_mutex_t *mtx;
};

void *writer(void *rs)
{
    arg *ra=static_cast<arg*>(rs);
    while(1)
    {
        sleep(6+rand()%12);
        pthread_mutex_lock(ra->lib);
        pthread_mutex_lock(ra->mtx);
        cout<<"W is in"<<endl;
        pthread_mutex_unlock(ra->mtx);
        for(int i=1;i<ra->pa;i++)
        {
            pthread_mutex_lock(ra->lib+i);
        }
        pthread_mutex_lock(ra->mtx);
        cout<<"W is writing"<<endl;
        pthread_mutex_unlock(ra->mtx);
        sleep(4+rand()%8);
        pthread_mutex_lock(ra->mtx);
        cout<<"W is out"<<endl;
        pthread_mutex_unlock(ra->mtx);
        for(int i=1;i<ra->pa;i++)
        {
            pthread_mutex_unlock(ra->lib+i);
        }
        pthread_mutex_unlock(ra->lib);

    }

}

void *reader(void *rs)
{
    arg *ra=static_cast<arg*>(rs);
    while(1)
    {
        sleep(4+rand()%4);
        if(pthread_mutex_trylock(ra->lib)==0)
        {
            pthread_mutex_lock(ra->mtx);
            cout<<"r"<<ra->pn<<" is in"<<endl;
            pthread_mutex_unlock(ra->mtx);
            pthread_mutex_lock(ra->lib+ra->pn);
            pthread_mutex_unlock(ra->lib);
            sleep(4+rand()%8);
            pthread_mutex_lock(ra->mtx);
            cout<<"r"<<ra->pn<<" is out"<<endl;
            pthread_mutex_unlock(ra->mtx);
            pthread_mutex_unlock(ra->lib+ra->pn);
        }
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int pa;
    cout<<"Number of readers: ";
    cin>>pa;
    pa++;

    pthread_mutex_t mtx;
    pthread_mutex_t *lib=new pthread_mutex_t[pa];
    pthread_mutex_init(&mtx,NULL);
    pthread_t *pers=new pthread_t[pa];
    arg *al=new arg[pa];

    for(int i=0;i<pa;i++)
    {
        pthread_mutex_init(lib+i,NULL);
        al[i].lib=lib;
        al[i].mtx=&mtx;
        al[i].pn=i;
        al[i].pa=pa;
    }

    pthread_create(pers,NULL,writer,al);
    for(int i=1;i<pa;i++)
    {
        pthread_create(pers+i,NULL,reader,al+i);
    }

    for(int i=0;i<pa;i++)
    {
        pthread_join(pers[i],NULL);
    }

    return 0;
    return a.exec();
}
