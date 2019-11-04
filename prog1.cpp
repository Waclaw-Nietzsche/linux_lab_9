#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <string.h>

using namespace std;

#define SIZE 1024

/* Параметры: 1 - частота записи (сек), 2 - количество строк на запись */
int main(int argc, char const *argv[])
{
    ofstream file("out.txt", ios_base::app);
    pid_t pid = getpid();
    int shmid, check;
    int wait = atoi(argv[1]);
    int iterations = atoi(argv[2]);
    key_t key = ftok("prog", 5);

    // Вернёт -1 если уже существует сегмент
    if ((check = shmget(key, SIZE, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
    {
        if ((shmid = shmget(key, SIZE, 0666 | IPC_CREAT)) == -1)
        {
            cerr << getpid() << ": Can't allocate shared memory! Exiting..." << endl;
            return -1;
        }
        cerr << getpid() << ": Connected to!" << endl;
    }
    else
    {
        cerr << getpid() << ": Created new!" << endl;
        shmid = check;
    }
    
    char *mutexvar = (char*) shmat(shmid, (void*)0, 0);

    if (check == -1)
    {
        int mutex = 0;
        while (iterations != 0)
        {
            while(atoi(mutexvar) != 0)
            {
                //waiting
            }
            mutex = 1;
            sprintf(mutexvar, "%d", mutex);
            file << getpid() << endl;
            mutex = 0;
            sprintf(mutexvar, "%d", mutex);
            iterations = iterations - 1;
            cout << getpid() << ": Now waiting" << endl;
            sleep(wait);
        }
    }
    else
    {
        int mutex = 0;
        sprintf(mutexvar, "%d", mutex);
        while (iterations != 0)
        {
            while(atoi(mutexvar) != 0)
            {
                //waiting
            }
            mutex = 1;
            sprintf(mutexvar, "%d", mutex);
            file << getpid() << endl;
            mutex = 0;
            sprintf(mutexvar, "%d", mutex);
            iterations = iterations - 1;
            cout << getpid() << ": Now waiting" << endl;
            sleep(wait);
        }
    }
    if (shmdt(mutexvar) != 0)
    {
        cerr << getpid() << ": Can't delete shared variable! Exiting..." << endl;
        return -1;
    }  
    file.close();
    cout << getpid() << ": Finished..." << endl;

    if (check != -1)
    {
        shmctl(shmid, IPC_RMID, NULL);
        cout << getpid() << ": Memory marked for delete..." << endl;
    }
    return 0;
}
