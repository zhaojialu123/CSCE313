/* 
    File: semaphore.C

    Author: Jialu Zhao
            Department of Computer Science
            Texas A&M University
    Date  : 06/12/17

*/
#include "semaphore.H"
#include <iostream>
using namespace std;

// constructor
Semaphore::Semaphore(int _val)
{
	value = _val;
	// initalize
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&c,NULL);
}

//destructor
Semaphore::~Semaphore()
{
	pthread_mutex_destroy(&m);
    pthread_cond_destroy(&c);

}

// SEMAPHORE OPERATIONS
void Semaphore::P()
{
	//cout << "here hahaha" << endl;
	pthread_mutex_lock(&m); 
	value--; 
	if (value<0) { 
		pthread_cond_wait(&c, &m);
	} 
	pthread_mutex_unlock(&m);

}

void Semaphore::V()
{
	pthread_mutex_lock(&m);
 	value++; 
	if (value<=0) {// someone is waiting 
		pthread_cond_signal(&c);
	} 
	pthread_mutex_unlock(&m);

}
