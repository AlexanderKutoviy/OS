#include <QCoreApplication>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>

#include <time.h>
#include <QDebug>
#include <QString>
#include <iostream>
#include <QVector>

using namespace std;

void procdir(int lvl, QString directory_name,QVector<int> *strong_files,QVector<QStringList> *strong_links)
{
    DIR *dir=NULL;
    if(!(dir=opendir(directory_name.toStdString().c_str())))
    {
        qDebug()<<"     sorry..."<<errno;
        return;
    }
    struct dirent *content=NULL;
    struct stat content_info;
    while(content=readdir(dir))
    {
        if(lstat(QString(directory_name+"/"+content->d_name).toStdString().c_str(),&content_info)==0)
        {
//            for(int i=0;i<lvl;i++)
//                cout<<"|      ";
//            cout<<content->d_name;

            if(S_ISDIR(content_info.st_mode)&&QString(content->d_name).compare(".")!=0&&QString(content->d_name).compare(".."))
            {
//                cout<<":"<<endl;
                procdir(lvl+1,directory_name+"/"+content->d_name,strong_files,strong_links);
            }
            else
            {
                if(content_info.st_nlink>1&&QString(content->d_name).compare(".")!=0&&QString(content->d_name).compare(".."))
                {
                    if(strong_files->contains(content_info.st_ino))
                        (*strong_links)[strong_files->indexOf(content_info.st_ino)].push_back(directory_name+"/"+content->d_name);
                    else
                    {
                        strong_files->push_back(content_info.st_ino);
                        strong_links->push_back(QStringList());
                        strong_links->last().push_back(directory_name+"/"+content->d_name);
                    }
                }
//                cout<<endl;
            }

        }
    }
    closedir(dir);

    return;

}

void describefiles(QStringList filestodescribe)
{
    struct stat fileinfo;
    for(int i=0;i<filestodescribe.size();i++)
    {
        if(lstat(filestodescribe[i].toStdString().c_str(),&fileinfo)==0)
        {
            if(S_ISDIR(fileinfo.st_mode))
            {
                DIR *dir=NULL;
                dirent *nestedfile=NULL;
                QStringList nestedlist;
                if(dir=opendir(filestodescribe[i].toStdString().c_str()))
                {
                    while(nestedfile=readdir(dir))
                    {
                        if(QString(nestedfile->d_name).compare(".")!=0&&QString(nestedfile->d_name).compare("..")!=0)
                        {
//                            cout<<filestodescribe[i].toStdString().c_str()<<endl;
//                            qDebug()<<nestedfile->d_name;
                            nestedlist.push_back(filestodescribe[i]+"/"+QString(nestedfile->d_name));
                        }
                    }
                    describefiles(nestedlist);
                    closedir(dir);
                }
            }
            else
            {
                cout<<"filename   : "<<filestodescribe[i].toStdString()<<endl;
                cout<<"user       : "<<getpwuid(fileinfo.st_uid)->pw_name<<endl;
                char buff[20];
//                strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(fileinfo.st_atim));
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME,&ts);
                cout<<"last access: "<<ts.tv_sec-fileinfo.st_atim.tv_sec<<endl;
                if(S_ISBLK(fileinfo.st_mode))
                    cout<<"Ц специальный блочный файл (блочное устройство).";
                if(S_ISCHR(fileinfo.st_mode))
                    cout<<"Ц специальный символьный файл (символьное устройство).";
                if(S_ISDIR(fileinfo.st_mode))
                    cout<<"Ц каталог";
                if(S_ISFIFO(fileinfo.st_mode))
                    cout<<"Ц UNIX-канал (pipe) или файл типа FIFO";
                if(S_ISLNK(fileinfo.st_mode))
                    cout<<"Ц символьна€ ссылка";
                if(S_ISREG(fileinfo.st_mode))
                    cout<<"Ц обычный файл";
                cout<<endl;
                cout<<"_______________________"<<endl;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QVector<QStringList> strong_links;
    QVector<int> strong_files;
    QStringList filestodescribe;

//    QString dir_to_analize="/home/pipich3/ƒокументы";
    QString dir_to_analize="/home";
//    QString dir_to_analize="/";

    procdir(0,dir_to_analize,&strong_files,&strong_links);

    cout<<endl<<"doubled strong links"<<endl;
    for(int i=0;i<strong_files.size();i++)
    {
        cout<<strong_files[i]<<":"<<endl;
        for(int j=0;j<strong_links[i].size();j++)
        {
            cout<<"  -"<<strong_links[i][j].toStdString()<<endl;
        }
    }

    cout<<"_______________________"<<endl;
    for(int i=1;i<argc;i++)
//        cout<<argv[i]<<endl;
        filestodescribe.push_back(argv[i]);
//    filestodescribe.push_back("/home/pipich3/«агрузки");
    describefiles(filestodescribe);


    return 0;
    return a.exec();
}
