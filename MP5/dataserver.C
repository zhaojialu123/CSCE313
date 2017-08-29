/* 
    File: dataserver.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/16

    Dataserver main program for MP3 in CSCE 313
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
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

#include "NetworkRequestChannel.H"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

//void handle_process_loop(RequestChannel & _channel);

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

// same use with the function in networkrequestchannel.c but just work for a file descriptor 
string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}
  
  string cread(int fd)
  {
    char buffer[255];

    if (recv(fd, buffer, 256, 0) < 0) {
      perror("Error receiving messgae from fd!!");
    }
    
    string s = buffer;

    //  cout << "Request Channel (" << my_name << ") reads [" << buf << "]\n";

    return s;

  }
 
  int cwrite(int fd,string _msg)
  {
    if (_msg.length() >= 255) {
    cerr << "Message too long for Channel!\n";
    return -1;
    }

    //  cout << "Request Channel (" << my_name << ") writing [" << _msg << "]";

    const char * s = _msg.c_str();

    if (send(fd, s, strlen(s)+1, 0) < 0) {
      perror("Error sending message from fd!!");
    }
    return 0;

  }
  string send_request(int fd, string _request)
  {
    cwrite(fd, _request);
    string s = cread(fd);
    return s;
  }

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/

/*void * handle_data_requests(void * args) {

  RequestChannel * data_channel =  (RequestChannel*)args;

  // -- Handle client requests on this channel. 
  
  handle_process_loop(*data_channel);

  // -- Client has quit. We remove channel.
 
  delete data_channel;
}*/



/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/


// wroks for a file descriptor using same with the channels 
void process_hello(int fd, const string & _request) {
  cwrite(fd, "hello to you too");
}

void process_data(int fd, const string &  _request) {
  usleep(1000 + (rand() % 5000));
  //_channel.cwrite("here comes data about " + _request.substr(4) + ": " + int2string(random() % 100));
  cwrite(fd, int2string(rand() % 100));
}

/*void process_newthread(RequestChannel & _channel, const string & _request) {
  int error;
  nthreads ++;

  // -- Name new data channel

  string new_channel_name = "data" + int2string(nthreads) + "_";
  //  cout << "new channel name = " << new_channel_name << endl;

  // -- Pass new channel name back to client

  _channel.cwrite(new_channel_name);

  // -- Construct new data channel (pointer to be passed to thread function)
  
  RequestChannel * data_channel = new RequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);

  // -- Create new thread to handle request channel

  pthread_t thread_id;
  //  cout << "starting new thread " << nthreads << endl;
  if (error = pthread_create(& thread_id, NULL, handle_data_requests, data_channel)) {
    fprintf(stderr, "p_create failed: %s\n", strerror(error));
  }  

}*/

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/



void process_request(int fd, const string & _request) {

  if (_request.compare(0, 5, "hello") == 0) {
    process_hello(fd, _request);
  }
  else if (_request.compare(0, 4, "data") == 0) {
    process_data(fd, _request);
  }
  /*else if (_request.compare(0, 9, "newthread") == 0) {
    process_newthread(_channel, _request);
  }*/

  else {
    cwrite(fd, "unknown request");
  }

}

void * connection_handler(void * arg_confd) {
  // convert a void * to an int can be used as a file descriptor 
  int confd = *((int *)(arg_confd));
  while(1)
  {

    //cout << "Reading next request from channel (" << channel.name() << ") ..." << flush;
    // using a fd to read 
    string request = cread(confd);
   
    if (request.compare("quit") == 0) {
      cwrite(confd, "bye");
      usleep(10000);          // give the other end a bit of time.
      break;                  // break out of the loop;
    }

    process_request(confd, request);
  
  }
  // close the new client 
  close(confd);
  
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
  int opt;
  // read data from command line
  char* port_number_server = (char *)"50000";
  int backlog = 10;

  while((opt = getopt(argc, argv, "p:b:")) != -1)
  {
    switch(opt)
    {
      case 'p':
        port_number_server = optarg;
        break;
      case 'b':
        backlog = atoi(optarg);
        break;
      default:
        port_number_server = (char *)"50000";
        backlog = 10;
    }
  }

  //  cout << "Establishing control channel... " << flush;
  //RequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
  //  cout << "done.\n" << flush;
 
  cout << "Establishing serever port:" << port_number_server << endl;
  NetworkRequestChannel server = NetworkRequestChannel(port_number_server, connection_handler, backlog);
  // desctruct the channel and can be used again
  cout << "here" << endl; 
  server.~NetworkRequestChannel();
  cout << "closed" << endl;

}

