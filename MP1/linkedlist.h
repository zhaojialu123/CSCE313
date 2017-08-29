// Jialu Zhao
// CSCE 313-100
// 05/31/2017
// MP1
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

typedef struct node node;

struct node 
{
	// Header
	node* next_ptr;
	// Payload
    int key;
    int value_len;
    //int value_rest; we don't need value rest as long as we can guaranttee value length is less than value rest
};

void Init(int M, int C); // initalize the linkedlist
void Destroy(); // destroy the linkedlist
int Insert(int x, char *value_ptr, int value_len); // insert a key-value pair to linkedlist
void Delete(int x); // delete the first item from the key x
char* Lookup(int x); // look up the node in linkedlist which key is x
void PrintList(); // Print out the whole linkedlist

#endif