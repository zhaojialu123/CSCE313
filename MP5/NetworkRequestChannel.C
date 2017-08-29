/* NetworkRequestChannel.C
   Jialu Zhao
   06/27/2017
  */
#include "NetworkRequestChannel.H"
#include <string> 
using namespace std;

 struct sockaddr_in serv_addr; // data structure , using as public

  // constructor for client 
  NetworkRequestChannel::NetworkRequestChannel(const char* _server_host_name, const char* _port_no)
  {
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    // map the port number with host name
    //if(struct servent * pse = getservbyport(_port_no, "tcp"))//make port
    //serv_addr.sin_port = pse->s_port;

    //else  
    if ((serv_addr.sin_port = htons((unsigned short)atoi(_port_no))) == 0)
      cerr << "cant connect port\n";

    if(struct hostent * hn = gethostbyname(_server_host_name))
      memcpy(&serv_addr.sin_addr, hn->h_addr, hn->h_length);

    else if((serv_addr.sin_addr.s_addr = inet_addr(_server_host_name)) == INADDR_NONE)
      cerr << "cant determine host <" << _server_host_name << ">\n";

    // socket
     if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("\n Error : Could not create socket \n");
    } 

   // int address = getaddrinfo(_server_host_name.c_str(), port_number.c_str(), &hints, &res);

    //connect
    if( connect(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       perror("\n Error : Connect Failed \n");
    }

    //cout << "client" << endl;
    
  }
 
 // constructor for server
  NetworkRequestChannel::NetworkRequestChannel(const char* _port_no,
                        void * (*connection_handler)(void*), int backlog)
  {
    // initalize the serv_addr to be empty
      memset(&serv_addr, '0', sizeof(serv_addr));
    // allocate the members 
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // pass the port number to its memeber 
      if((serv_addr.sin_port = htons((unsigned short)atoi(_port_no))) == 0)
        cerr << "can't get port\n";

      // create a socket 
      listenfd = socket(AF_INET, SOCK_STREAM, 0);
      if(listenfd < 0)
        perror("can not create socket!!");
      //bind the socket with the address 
      if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        perror("can not bind!!"); 
      // start to listen until someone can connect 
      if(listen(listenfd, backlog) < 0)
        perror("can not listen!!"); 

      printf("Ready for client connect().\n");

      // using for accept a client 
      struct sockaddr_in client_addr;
      int len = sizeof(client_addr);

      // keep accepting new clients 
      int count = 0;
      while(1)
      {
        connfd = accept(listenfd, (struct sockaddr*)&client_addr, (socklen_t*)&len);
        // everytime create a new thread to deal with new request 
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, connection_handler, &connfd);
          //close(connfd);

        // the reson for sleep is before the first thread end avoid the second thread both connect with the server
        sleep(1);
       }

   cout << "connect completed!!" << endl;
  }
  
  NetworkRequestChannel::~NetworkRequestChannel()
  {
    close(listenfd);
  }


// the next three functions only work for a channel
  string NetworkRequestChannel::send_request(string _request)
  {
    cwrite(_request);
    string s = cread();
    return s;
  }
  
  string NetworkRequestChannel::cread()
  {
    char buffer[255];

    if (recv(listenfd, buffer, 256, 0) < 0) {
      perror("Error receiving messgae from fd!!");
    }
    
    string s = buffer;

    //  cout << "Request Channel (" << my_name << ") reads [" << buf << "]\n";

    return s;

  }
 
  int NetworkRequestChannel::cwrite(string _msg)
  {
    if (_msg.length() >= 255) {
    cerr << "Message too long for Channel!\n";
    return -1;
    }

    //  cout << "Request Channel (" << my_name << ") writing [" << _msg << "]";

    const char * s = _msg.c_str();

    //cout << _msg << endl;

    if (send(listenfd, s, strlen(s)+1, 0) < 0) {
      perror("Error sending message from fd!!");
    }
    return 0;

  }
  