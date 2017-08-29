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
// finished bonus
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <iomanip>   
#include "reqchannel.H"
#include "boundedBuffer.H"
#include <time.h>
#include <signal.h>



using namespace std;
#define CLOCKID CLOCK_REALTIME
#define SIG SIGUSR1

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
/*string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}*/

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

/*void signal_handler(int a, siginfo_t * b, void * c)
{
  system("clear");
  print_histogram();
}
*/
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
void *event_handler_func(void * chan)
{
    RequestChannel *channel = (RequestChannel*)chan;
  // create w worker channels
    cout << "Create w worker channels:" << endl;
    //declare vector to hold worker channels/worker thread arguments
    // using vector rather than array which size is dynamic
    vector<string> request_vector = vector<string>(number_worker_threads,"");
    vector<RequestChannel*> worker_channel = vector<RequestChannel*>(number_worker_threads,nullptr);
    for(int i = 0; i < number_worker_threads; i++)
    {
      //initialize worker channels/worker thread arguments
      string reply = channel -> send_request("newthread");
      // using pointer works fine 
      worker_channel[i] = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);
      worker_channel[i] -> cwrite("hello");
      request_vector[i] = "hello";
    } 
    cout << "done." << endl;

  /*string reply = channel -> send_request("newthread"); //Can't do parallel newthread requests
  RequestChannel *worker_channel = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);*/
  
  int counter = 0; // count for how many quit requests you got
  while(true)
   {
    // remove don't have sync
      string reply;
      string request;
      fd_set rfds;
      struct timeval tv;
      int retval;
      int nfds = 0;

      tv.tv_sec = 10;     
      tv.tv_usec = 0;
      // initalize the fd_set
      FD_ZERO(&rfds);
      //FD_ZERO(&wfds);
      for(int i =0; i < number_worker_threads; i++)
      {
        if(worker_channel[i] != NULL)
        {
          int fd1 = worker_channel[i] -> read_fd();
          //int fd2 = worker_channel[i] -> write_fd();
          FD_SET(fd1, &rfds);   /* Watch a file (e.g. fd1) to see when it has input to read. */
          //FD_SET(fd2, &wfds); /* Watch a file (e.g. fd2) to see when it can be written. */
          nfds = max(fd1+1,nfds);
        }
      }

     retval = select(nfds, &rfds, NULL, NULL, &tv);
     //cout<<retval<<endl;
     if (retval == -1) 
            perror("select()"); 
      else if (retval) 
        {
            for (int j = 0; j <number_worker_threads; j++)
            {
                 if (worker_channel[j] != NULL && FD_ISSET(worker_channel[j]->read_fd(), &rfds))    //if i is ready to read
                 {  
                    reply = worker_channel[j] -> cread();    //read from the channel
                    
                    if(request_vector[j] == "data Joe Smith")
                    {
                      reply_buffer_Joe.insert(reply);
                      request = Buffer.remove();
                      worker_channel[j] -> cwrite(request);
                      request_vector[j] = request;
                    }
                      //take next request from buffer
                      //send it
                    //update request vector
                    else if(request_vector[j] == "data Jane Smith")
                    {
                      reply_buffer_Jane.insert(reply);
                      request = Buffer.remove();
                      worker_channel[j] -> cwrite(request);
                      request_vector[j] = request;
                    }
                    //take next request from buffer
                      //send it
                    //update request vector
                    else if(request_vector[j] == "data John Doe")
                    {
                      reply_buffer_John.insert(reply);
                      request = Buffer.remove();
                      worker_channel[j] -> cwrite(request);
                      request_vector[j] = request;
                    }
                    //take next request from buffer
                      //send it
                    //update request vector
                    else if(request_vector[j] == "hello") {
                      request = Buffer.remove();
                      worker_channel[j] -> cwrite(request);
                      request_vector[j] = request; 
                      //take next request from buffer
                      //send it 
                      //update request vector
                    }
                    // get out of the while loop while get the quit request
                    else if(request_vector[j] == "quit") 
                    {
                      
                      delete worker_channel[j];
                      worker_channel[j] = NULL;
                      //remove worker_channel[j]'s file descriptors from FD set
                      //FD_CLR(worker_channel[j]->read_fd(), &rfds);
                      //update counter for deleted channelsd
                      counter++;

                      //if counter == w then break
                      }
                    
                 }
            } 
            // exit from the while loop
            if(counter == number_worker_threads)
              break;
        }

        else 
            printf("No data within tv.\n");
            
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
  size_bounded_buffer = 50;
  number_worker_threads = 10;

  while((opt = getopt(argc, argv, "n:b:w:")) != -1)
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
      default:
        data_request_per_person = 100;
        size_bounded_buffer = 50;
        number_worker_threads = 10;
    }
  }
  //cout << data_request_per_person << endl;
  // produce the child process
  pid_t pid = fork();
  struct timespec start, end;
  double total;
  if(pid != 0)
  {
    clock_gettime(CLOCK_REALTIME,&start);
    // create a control channel
    cout << "CLIENT STARTED:" << endl;

    cout << "Establishing control channel... " << flush;
    RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
    cout << "done." << endl;

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

    /*// create w worker channels
    cout << "Create w worker channels:" << endl;
    //declare vector to hold worker channels/worker thread arguments
    // using vector rather than array which size is dynamic
    vector<RequestChannel*> worker_channel = vector<RequestChannel*>(number_worker_threads,nullptr);
    for(int i = 0; i < number_worker_threads; i++)
    {
      //initialize worker channels/worker thread arguments
      string reply = chan.send_request("newthread");
      // using pointer works fine 
      worker_channel[i] = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);
    } 
    cout << "done." << endl;*/

    // create one single event handler 
    cout << "Create one single event handler:"<< endl;
    pthread_t event_handler;
    int error;
      if (error = pthread_create(& event_handler, NULL, event_handler_func,(void*) chan )) 
      {
        fprintf(stderr, "p_create failed: %s\n", strerror(error));
        exit(1);
      } 

    cout << "request_thread joined:" << endl;
    pthread_join(requester_threads[0],NULL);
    pthread_join(requester_threads[1],NULL);
    pthread_join(requester_threads[2],NULL);
    cout << "done." << endl;

    // no worker threads any more
    // only when using channels need to send quit to let the channels end 
    cout << "insert quit requests:" << endl;
    for(int i = 0; i < number_worker_threads; i++)
    {
      Buffer.insert("quit");
    }
    cout << "done" << endl;
  
    cout << " event handler joined:" << endl; 
    pthread_join(event_handler,NULL);
    cout << "done." << endl;

    cout << " stat_threads joined:" << endl;
    pthread_join(stat_threads[0], NULL); // wait for stat threads to exit
    pthread_join(stat_threads[1], NULL);
    pthread_join(stat_threads[2], NULL);
    cout << "done." << endl;

    

    // tell the control channel to quit 
    string reply4 = chan -> send_request("quit");
    clock_gettime(CLOCK_REALTIME,&end);
    print_histogram(1);
    total = (end.tv_sec -start.tv_sec)+(end.tv_nsec -start.tv_nsec)/1E9;
    cout << "The total time is:" << total << "s" << endl;
    delete chan;
    usleep(1000000);
  }
  else 
    execl("./dataserver", (char*)NULL);

  return 0; // successfully exit 
}
