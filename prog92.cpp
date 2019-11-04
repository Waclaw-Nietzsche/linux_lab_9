#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <string.h>

using namespace std;

#define SIZE 512
#define LIMIT 10

/* Параметры: 1 - номер программы */
int main(int argc, char const *argv[])
{
    int shmid1, shmid2, shmid3;
    key_t key = ftok("prog92", 1);
    key_t key_1 = ftok("prog92", 2);
    key_t key_2 = ftok("prog92", 3);

    switch (atoi(argv[1]))
    {
        case 1:
        {
            cout << "Programm 1 (" << getpid() << ") started." << endl;
            if ((shmid1 = shmget(key, SIZE, 0666 | IPC_CREAT)) == -1)
            {
                cerr << getpid() << ": Can't allocate shared memory! Exiting..." << endl;
                return -1;
            }
            cout << getpid() << ": Created new shared memory #1: " << shmid1 << endl;
            
            if ((shmid2 = shmget(key_1, SIZE, 0666 | IPC_CREAT)) == -1)
            {
                cerr << getpid() << ": Can't allocate shared memory! Exiting..." << endl;
                return -1;
            }
            cout << getpid() << ": Created new shared memory #2: " << shmid2 << endl;

            if ((shmid3 = shmget(key_2, SIZE, 0666 | IPC_CREAT)) == -1)
            {
                cerr << getpid() << ": Can't allocate shared memory! Exiting..." << endl;
                return -1;
            }
            cout << getpid() << ": Created new shared memory #3: " << shmid3 << endl;

            int mutex = 0;
            int *array, *newarray, *mutexvar;
            array = (int *)shmat(shmid1, NULL, 0);
            newarray = (int *)shmat(shmid2, NULL, SHM_RDONLY);
            mutexvar = (int*) shmat(shmid3, NULL, 0);
            mutexvar[0] = mutex;
            mutexvar[1] = mutex;
            cout << getpid() << ": Initial array is [";
            for(int i = 0; i < LIMIT; i++ )
            {
                array[i] = i;
                cout << array[i];
                if (i != (LIMIT - 1))
                {
                    cout << ", ";
                }
            }
            cout << "]." << endl;

            mutex = 1;
            mutexvar[0] = mutex;

            while(mutexvar[1] != 1)
            {
                //waiting
            }
            
            cout << getpid() << ": Modified array is [";
            for(int i = 0; i < LIMIT; i++ )
            {
                cout << newarray[i];
                if (i != (LIMIT - 1))
                {
                    cout << ", ";
                }
            }
            cout << "]." << endl;


            shmctl(shmid1, IPC_RMID, NULL);
            cout << getpid() << ": Memory #1 marked for delete..." << endl;
            shmctl(shmid3, IPC_RMID, NULL);
            cout << getpid() << ": Memory #3 marked for delete..." << endl;
        } 
        break;
        case 2:
        {
            cout << "Programm 2 (" << getpid() << ") started." << endl;
            if ((shmid1 = shmget(key, SIZE, 0666 | IPC_CREAT)) == -1)
            {
                cerr << getpid() << ": Can't allocate shared memory! Exiting..." << endl;
                return -1;
            }
            cout << getpid() << ": Connected to existed shared memory #1: " << shmid1 << endl;
            if ((shmid2 = shmget(key_1, SIZE, 0666 | IPC_CREAT)) == -1)
            {
                cerr << getpid() << ": Can't allocate shared memory! Exiting..." << endl;
                return -1;
            }
            cout << getpid() << ": Connected to existed shared memory #2: " << shmid2 << endl;

            if ((shmid3 = shmget(key_2, SIZE, 0666 | IPC_CREAT)) == -1)
            {
                cerr << getpid() << ": Can't allocate shared memory! Exiting..." << endl;
                return -1;
            }
            cout << getpid() << ": Connected to existed shared memory #3: " << shmid3 << endl;

            int *array, *newarray, *mutexvar;
            array = (int *)shmat(shmid1, NULL, SHM_RDONLY);
            newarray = (int *)shmat(shmid2, NULL, 0);
            mutexvar = (int *) shmat(shmid3, NULL, 0);

            while(mutexvar[0] != 1)
            {
                //waiting
            }
            int numberarray[LIMIT];
            cout << getpid() << ": Got array, which is [";
            for(int i = 0; i < LIMIT; i++ )
            {
                numberarray[i] = array[i];
                cout << numberarray[i];
                newarray[i] = numberarray[i] - 1;  
                if (i != (LIMIT - 1))
                {
                    cout << ", ";
                }
            }
            cout << "]." << endl;

            int mutex = 1;
            mutexvar[1] = mutex;
            shmctl(shmid2, IPC_RMID, NULL);
            cout << getpid() << ": Memory #2 marked for delete..." << endl;
        }
        break;
        default:
        {
            cerr << "Incorrect input!" << endl;
        }
        break;
    }
    return 0;
}
