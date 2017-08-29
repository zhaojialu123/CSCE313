// Jialu Zhao
// boundedBuffer.C
// 06/12/2017
#include "boundedBuffer.H"
#include <iostream>
using namespace std;
/*constructor and destructor*/
 boundedBuffer::boundedBuffer(int N)
 {
 	full = new Semaphore(0);
 	empty = new Semaphore(N); // N is the total number in the buffer
  mutex = new Semaphore(1);
 }

 boundedBuffer::~boundedBuffer()
 {
  delete full;
  delete empty; 
  delete mutex;
 }


  /* -- boundedBuffer OPERATIONS */

  void boundedBuffer::insert(string request)
  {
  	empty -> P();
  	mutex -> P(); // lock the critical section
  	requests.push(request);
  	mutex -> V(); // unlock the critical section

  	full -> V();

  }

  string boundedBuffer::retrieve()
  {
  	full -> P();
  	mutex -> P(); // lock the critical section
  	string request = requests.front();
  	requests.pop();
  	mutex -> V(); // unlock the critical section

  	empty -> V();
  	return request;
  }