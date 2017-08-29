/* 
    File: simpleclient.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

    Simple client main program for MP2 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <time.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "reqchannel.H"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/
  int nthreads2;
    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

string local_func(string request){
  if(request.compare(0, 5, "hello")== 0)
    return "hello to you too";
  else if(request.compare(0, 4, "data")== 0)
    return int2string(rand()%100);
  else if(request.compare(0, 9, "newthread") == 0)
  {
    nthreads2 ++;
    return "data" + int2string(nthreads2) + "_";
  }
  else if(request.compare("quit") == 0)
    return "bye";
  else
    return "unknown request";
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
  pid_t pid = fork();
  //clock_t start_t1, end_t1, total_t1;
  struct timespec start1, end1, start2, end2;
  double total1,total2;
  if(pid != 0)
  {

    cout << "CLIENT STARTED:" << endl;

    cout << "Establishing control channel... " << flush;
    RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
    cout << "done." << endl;

    /* -- Start sending a sequence of requests */
    
    string reply1 = chan.send_request("hello");
    cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;
    

    string reply2 = chan.send_request("data Joe Smith");
    cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

    string reply3 = chan.send_request("data Jane Smith");
    cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;
    clock_gettime(CLOCK_REALTIME,&start1);
    for(int i = 0; i < 100; i++) {
      string request_string("data TestPerson" + int2string(i));
      string reply_string = chan.send_request(request_string);
  	cout << "reply to request " << i << ":" << reply_string << endl;;
    }
    clock_gettime(CLOCK_REALTIME,&end1);
    total1 = (end1.tv_sec -start1.tv_sec)+(end1.tv_nsec -start1.tv_nsec)/1E9;
    printf("Total time taken by CPU: %f\n", total1  );
    printf("Time taken by one request:%f\n", total1/100);
    string reply4 = chan.send_request("quit");
    cout << "Reply to request 'quit' is '" << reply4 << endl;

    //---------------do the same process using local function-----------------
    printf("%s\n","do the same process using local function:" );
    string reply21 = local_func("hello");
    cout << "Reply to request 'hello' is '" << reply21 << "'" << endl;
    

    string reply22 = local_func("data Joe Smith");
    cout << "Reply to request 'data Joe Smith' is '" << reply22 << "'" << endl;

    string reply23 = local_func("data Jane Smith");
    cout << "Reply to request 'data Jane Smith' is '" << reply23 << "'" << endl;

    clock_gettime(CLOCK_REALTIME,&start2);
    for(int i = 0; i < 100; i++) {
      string request_string("data TestPerson" + int2string(i));
      string reply_string = local_func(request_string);
    cout << "reply to request " << i << ":" << reply_string << endl;;
    }
    clock_gettime(CLOCK_REALTIME,&end2);
    total2 = (end2.tv_sec -start2.tv_sec)+(end2.tv_nsec -start2.tv_nsec)/1E9;
    printf("Total time taken by CPU(by using local function): %f\n", total2  );
    printf("Time taken by one request(by using local function):%f\n", total2/100);
    string reply24 = local_func("quit");
    cout << "Reply to request 'quit' is '" << reply24 << endl;


    usleep(1000000);

}
else 
  execl("./dataserver", (char*)NULL);



}
