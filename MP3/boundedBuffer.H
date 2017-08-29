/* 
    File: boundedBuffer.H

    Author: Jialu Zhao
            Department of Computer Science
            Texas A&M University
    Date  : 06/12/17

*/

#ifndef _boundedBuffer_H_                   // include file only once
#define _boundedBuffer_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include "semaphore.H"
#include <queue>
#include <string>
using namespace std;
// boundedBuffer is used as a specific queue 

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   S e m a p h o r e  */
/*--------------------------------------------------------------------------*/

class boundedBuffer {
private:
  /* -- INTERNAL DATA STRUCTURES
     You may need to change them to fit your implementation. */
  // directly using Seamphore rather than the pointer can't be compiled 
  Semaphore *full;
  Semaphore *empty;
  Semaphore *mutex;
  queue<string> requests; // implement the boundedBuffer as a queue
public:

  /* -- CONSTRUCTOR/DESTRUCTOR */

  boundedBuffer(int N);

  ~boundedBuffer();

  /* -- boundedBuffer OPERATIONS */

  void insert(string request);
  string retrieve();
};


#endif


