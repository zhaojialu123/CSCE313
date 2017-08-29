/* 
    File: simpleclient.C

    Author: Jialu Zhao
            Department of Computer Science
            Texas A&M University
    Date  : 2017/06/12

    Simple client main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <iomanip>  
#include <time.h>
#include <signal.h>
#include <sys/time.h> 
#include <unistd.h>
#include "NetworkRequestChannel.H"
#include "boundedBuffer.H"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/
// global variables 
  int data_request_per_person = 10000;
  int size_bounded_buffer = 100;
  int number_worker_threads = 10;
  char* name_server_host = (char *)"127.0.0.1";
  char* port_number_server_host = (char *)"50005";

  boundedBuffer Buffer(size_bounded_buffer);
  boundedBuffer reply_buffer_Joe(size_bounded_buffer);// = Buffer
  boundedBuffer reply_buffer_Jane(size_bounded_buffer);// = Buffer
  boundedBuffer reply_buffer_John(size_bounded_buffer);// = Buffer
  
  // initalize all histogram to be 0
  int histogram_Joe[10] = {}; // histogram for Joe
  int histogram_Jane[10] = {}; // histogram for Jane
  int histogram_John[10] = {}; // histogram for John
    /* -- (none) -- */
  

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

// requester threads function 
void print_histogram(int signum)
{
   system("clear");
  // print the histogram 
    cout << setw(50) << "Histogram" << endl;
    
    cout<<"Name" << setw(12)<<"0-9"<<setw(7)<<"10-19"<<setw(7)<<"20-29"<<setw(7)<<"30-39"<<setw(7)<<"40-49"<<setw(7)<<"50-59"
        <<setw(7)<<"60-69"<<setw(7)<<"70-79"<<setw(7)<<"80-89"<<setw(7)<<"90-99"<<setw(7)<<"Total"<<endl;
    cout << "Joe Doe" << setw(8);

    int sum1 = 0;
    int sum2 = 0;
    int sum3 = 0;

    for(int i = 0; i < 10; i++)
    {
      sum1 = sum1 + histogram_Joe[i]; 
      cout << histogram_Joe[i] << setw(7);
    }
    cout <<sum1;
    cout << endl;

    cout << "Jane Smith" << setw(5);
    for(int i = 0; i < 10; i++)
    {
      sum2 = sum2 + histogram_Jane[i]; 
      cout << histogram_Jane[i] << setw(7);
    }
    cout << sum2;
    cout << endl;

    cout << "John Smith" << setw(5);
    for(int i = 0; i < 10; i++)
     {
      sum3 = sum3 + histogram_Joe[i]; 
      cout << histogram_John[i] << setw(7);

    }
    cout << sum3;
    cout << endl;
}


void *requester_thread(void * name)
{
  int parent = *((int*)name);
  //cout << parent << endl;
  for(int i = 0; i < data_request_per_person; i++)
  {
    string request;
    switch (parent)
    {
      case 0:
      request = "data Joe Smith";
      break;
      case 1:
      request = "data Jane Smith";
      break;
      case 2:
      request = "data John Doe";
      break;
    }
    Buffer.insert(request);
  }
  /*cout << "quit" + (char)parent << endl;
  Buffer.insert("quit" + (char)parent);*/

  // after sending n requests, exit from requester threads 
  pthread_exit(NULL);
}

// worker threads function 
void *worker_thread(void * chan)
{
  NetworkRequestChannel *channel = (NetworkRequestChannel*)chan;
  /*string reply = channel -> send_request("newthread"); //Can't do parallel newthread requests
  RequestChannel *worker_channel = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);*/
  
  while(true)
   {
      string request = Buffer.retrieve();
      //cout << request << endl;

      // send request to channel, get reply 
      string reply;

      
      //cout << "worker_thread" << pthread_self() << endl;
      reply = channel -> send_request(request);
      // send the reply to seperate reply buffer 
      //cout << reply << ":" << pthread_self()<< endl;
      if(request == "data Joe Smith")
        reply_buffer_Joe.insert(reply);
      else if(request == "data Jane Smith")
        reply_buffer_Jane.insert(reply);
      else if(request == "data John Doe")
        reply_buffer_John.insert(reply);
      // get out of the while loop while get the quit request
      else if(request == "quit") 
      {
        delete channel;
        break;
      }
  }
    // exit from worker threads 
   pthread_exit(NULL);

}

// stat threads function 
void *stat_thread(void * name)
{
  int count = 0;
  int parent = *((int*)name);
  while(true)
  {
    string reply;
    switch(parent)
    {
      case 0:
        reply = reply_buffer_Joe.retrieve();
        break;
      case 1:
        reply = reply_buffer_Jane.retrieve();
        break;
      case 2:
        reply = reply_buffer_John.retrieve();
        break;
    }
    int index = stoi(reply.c_str())/10;
    switch(parent)
    {
      case 0:
        histogram_Joe[index]++;
        break;
      case 1:
        histogram_Jane[index]++;
        break;
      case 2:
        histogram_John[index]++;
        break;

    }
    count++;
    if(count == data_request_per_person)
      break;
  
  }
  // exit from stat threads 
  pthread_exit(NULL);

}
    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
  // update the histogram
   struct sigaction sa;
  struct itimerval timer;

  /* Install timer_handler as the signal handler for SIGVTALRM. */
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = &print_histogram;
  sigaction (SIGVTALRM, &sa, NULL);

  /* Configure the timer to expire after 250 msec... */
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 10000;
  /* ... and every 250 msec after that. */
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 10000;
  /* Start a virtual timer. It counts down whenever this process is
  executing. */
  setitimer (ITIMER_VIRTUAL, &timer, NULL);

  // read from command line
  int opt;
  // local variables exitsed in main function 
  data_request_per_person = 10000;
  size_bounded_buffer = 100;
  number_worker_threads = 10;
  name_server_host = (char *)"127.0.0.1";
  port_number_server_host = (char *)"50000";

  while((opt = getopt(argc, argv, "n:b:w:h:p:")) != -1)
  {
    switch(opt)
    {
      case 'n':
        data_request_per_person = atoi(optarg);
        break;
      case 'b':
        size_bounded_buffer = atoi(optarg);
        break;
      case 'w':
        number_worker_threads = atoi(optarg);
        break;
      case 'h':
        name_server_host = optarg;
        break;
      case 'p':
        port_number_server_host = optarg;
        break;
      default:
        data_request_per_person = 10000;
        size_bounded_buffer = 100;
        number_worker_threads = 10;
        name_server_host = (char *)"127.0.0.1";
        port_number_server_host = (char *)"50000";
    }
  }
  //cout << data_request_per_person << endl;
  // produce the child process
  //pid_t pid = fork();
  struct timespec start, end;
  double total;
  //if(pid != 0)
  //{
    clock_gettime(CLOCK_REALTIME,&start);
    // create a control channel
    cout << "CLIENT STARTED:" << endl;

    /*cout << "Establishing control channel... " << flush;
    RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
    cout << "done." << endl;
    */

    // create 3 requester threads 
    cout << "Create 3 requester threads:" << endl;
    pthread_t requester_threads[3];

    // directly using i can't be compiled, after convert it to int*, everything is fine
    int *count[3];
    for(int i = 0; i < 3; i++){
        count[i] = new int(i);
    }

    for(int i = 0; i < 3; i++)
    {
      int error;
      if (error = pthread_create(&requester_threads[i], NULL, requester_thread, (void *) count[i])) 
      {
        fprintf(stderr, "p_create failed: %s\n", strerror(error));
        exit(1);
      } 
    } 
    cout << "done." << endl;

    // move worker channel inside worker threads
    // create w data channel
    /*cout << "Create w data channel:" << endl;
    RequestChannel *worker_channel[number_worker_threads];
    for(int i = 0; i < number_worker_threads; i++)
    {
      string reply = chan.send_request("newthread");
      worker_channel[i] = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);
    }
    cout << "done." << endl;*/

    // create 3 statistic threads, before worker threads because we need to send request to stat threads
    cout << "Create 3 statistic threads:" << endl;
    pthread_t stat_threads[3];
    for(int i = 0; i < 3; i++)
    {
      int error;
      if (error = pthread_create(& stat_threads[i], NULL, stat_thread, (void *) count[i])) 
      {
        fprintf(stderr, "p_create failed: %s\n", strerror(error));
        exit(1);
      } 
    } 
    cout << "done." << endl;

    // create w worker threads 
    cout << "Create w worker threads:" << endl;
    pthread_t worker_threads[number_worker_threads];
    //declare vector to hold worker channels/worker thread arguments
    // using vector rather than array which size is dynamic
    vector<NetworkRequestChannel*> worker_channel = vector<NetworkRequestChannel*>(number_worker_threads,nullptr);
    for(int i = 0; i < number_worker_threads; i++)
    {
      //initialize worker channels/worker thread arguments
      //string reply = chan.send_request("newthread");
      // using pointer works fine 
      worker_channel[i] = new NetworkRequestChannel(name_server_host,port_number_server_host);
      int error;
      if (error = pthread_create(& worker_threads[i], NULL, worker_thread,(void*) worker_channel[i] )) 
      {
        fprintf(stderr, "p_create failed: %s\n", strerror(error));
        exit(1);
      } 
    } 
    cout << "done." << endl;

    cout << "request_thread joined:" << endl;
    pthread_join(requester_threads[0],NULL);
    pthread_join(requester_threads[1],NULL);
    pthread_join(requester_threads[2],NULL);
    cout << "done." << endl;

    // only when using channels need to send quit to let the channels end 
    cout << "insert quit requests:" << endl;
    for(int i = 0; i < number_worker_threads; i++)
    {
      Buffer.insert("quit");
    }
    cout << "done" << endl;

    cout << " worker_threads joined:" << endl;
    for(int i = 0; i < number_worker_threads; i++)
    {
      pthread_join(worker_threads[i],NULL);
    }
    cout << "done." << endl;

    cout << " stat_threads joined:" << endl;
    pthread_join(stat_threads[0], NULL); // wait for stat threads to exit
    pthread_join(stat_threads[1], NULL);
    pthread_join(stat_threads[2], NULL);
    cout << "done." << endl;

  	// tell the control channel to quit 
    //string reply4 = chan.send_request("quit");
    clock_gettime(CLOCK_REALTIME,&end);
    //sleep(1);
    print_histogram(1);
    total = (end.tv_sec -start.tv_sec)+(end.tv_nsec -start.tv_nsec)/1E9;
    cout << "The total time is:" << total << "s" << endl;

    usleep(1000000);
 // }
  //else 
   // execl("./dataserver", (char*)NULL);

  return 0; // successfully exit 
}
