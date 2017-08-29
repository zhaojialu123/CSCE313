/* 
    File: my_allocator.c

    Author: <Jialu Zhao>
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/
  Node ** head; // an array of different free lists
  //int M; // the total memory we have 
  int b; // basic block size
  int total_size; // the size of the array
    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

/* Don't forget to implement "init_allocator" and "release_allocator"! */

unsigned int init_allocator(unsigned int _basic_block_size, 
          unsigned int _length)
{
  b = _basic_block_size;
  //M = _length;
  int size = log2(_length/_basic_block_size) + 1;
  head = (Node**) malloc(size*sizeof(Node*));
  Node* free_list = (Node*) malloc(_length);
  int times = 0;
  int index;
  while(_length != 0)
  {
    int bit = _length % 2;
    index = times - log2(_basic_block_size);
    if(bit != 0 && pow(2,times) >= _basic_block_size)
      {
        head[index] = free_list + (int)pow(2,times+1);
        head[index] -> size = pow(2,times+1);
        head[index] -> next = NULL;
        head[index] -> prev = NULL;
        head[index] -> freed = true;
      }

    times++;
    _length = _length / 2;
  }
  total_size = index + 1;
} 

int release_allocator()
{
  // free every elements in the array
  int i = 0;
  for( ; i < total_size; i++)
  {
    free(head[i]);
  }
} 

/*char * toArray(int number)
    {
        int n = log10(number) + 1;
        int i;
      char *numberArray = calloc(n, sizeof(char));
        for ( i = 0; i < n; ++i, number /= 10 )
        {
            numberArray[i] = number % 10;
        }
        return numberArray;
    }*/

Addr my_malloc(unsigned int _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
  // get the next rounded power of 2 number
  int next = pow(2, ceil(log(_length + sizeof(Node))/log(2)));
  int j = log2(next/b);
  Node * itr;
  //printf("Here");

  for( ; j < total_size; j++)
  {
    bool founded = false;
    itr = head[j-1];
    while(itr -> next != NULL && founded == false)
    {
      if(itr -> freed == true)
        {
          founded = true;
          break;
        }
      itr = itr -> next;
    }
    // judge the last element
    if(itr -> freed == true)
        {
          founded = true;
        }
    if(founded)
      break;
  }

//------------------split-----------------------------
  while(j > log2(next/b))
  {
    // take out from current list
    if(itr == head[j-1])
    {
      head[j-1] = head[j-1]-> next;
      if(head[j-1] -> next != NULL)
      head[j-1] -> next -> prev = NULL;
    }
    else
    {
      itr -> prev -> next = itr -> next;
      itr -> next -> prev = itr -> prev;
    }

    // insert into new list
    // get the last element in the last list
    Node * itr2 = head[j-2];
    while(itr2 -> next != NULL)
      itr2 = itr2 -> next;
    // insert to the right position
      itr2 -> next = itr;
      itr -> prev = itr2;
      itr -> size = pow(2,j-1);
      itr -> freed = true;

      // flip s-1 
      // get the number in s-1 
      /*int s_1_address =*((int*) itr);
      int i1 = 0;
      for( ; i1 < j; i1++)
      {
        s_1_address = s_1_address / 2;
      }
      int s_1_index = s_1_address % 2;
      int new_address;
      if(s_1_index == 0)
        new_address = new_address + pow(2,j-1);
      else
        new_address = new_address - pow(2,j-1);*/

      // buddy node
      //Node* new_node = (Node*) new_address;
      Node* new_node = (Node*) ((unsigned long int) itr ^ (unsigned long int) pow(2,j-1));
      new_node -> size = pow(2,j-1);
      new_node -> prev = itr;
      new_node -> next = NULL;
      new_node -> freed = true;
      itr -> next = new_node;
      j = j - 1;
  }
  itr -> freed = false;
  return (Addr)(itr + sizeof(Node));

}

int my_free(Addr _a) {
  /* Same here! */
  Node * temp = (Node*) (_a + sizeof(Node));
  int j = log2(temp -> size);
  
  while(j <= total_size )
  {
    // flip s-1, find buddy address
     /* int s_1_address = *((int*) temp);
      int i1 = 0;
      for( ; i1 < j; i1++)
      {
        s_1_address = s_1_address / 2;
      }
      int s_1_index = s_1_address % 2;
      int new_address;
      if(s_1_index == 0)
        new_address = new_address + pow(2,j-1);
      else
        new_address = new_address - pow(2,j-1);*/

    // check if its buddy block is free or not
        //Node* buddy = (Node*) new_address;
    Node* buddy = (Node*) ((unsigned long int) temp ^ (unsigned long int) pow(2,j-1));   // CMY Changed the code, using XOR
        if(buddy -> freed == true)
        {
          // find the smaller address
          int smaller_address;
          if(*((int*) temp) < *((int*) buddy))
            smaller_address = *((int*) temp);
          else
            smaller_address = *((int*) buddy);
          Node * actual = (Node*) smaller_address;
          // take out from current list
          if(actual == head[j-1])
            head[j-1] = NULL;
          else
          {
            actual -> prev -> next = actual -> next -> next;
            actual -> next -> next -> prev = actual -> prev;
          }
          // insert back to last list connected to the last node
          // get the last element in the last list
          Node * itr2 = head[j-2];
         while(itr2 -> next != NULL)
          itr2 = itr2 -> next;
          // insert to the right position
          itr2 -> next = actual;
          actual -> prev = itr2;
          j++;
          temp = actual;
        }
        else 
          break;

      }

  free(_a);
  return 0;
}

