// Jialu Zhao
// CSCE 313-100
// 05/31/2017
// MP1
#include "linkedlist.h"

// global variables used in functions
int memory_pool; // the total memory which is M
int block; // the memory of one block take which is b
node* Head; // the first element in the linkedlist 
node* free_pointer; // point to where is free
int existed_nodes; // how many nodes existed now in the linkedlist 

//-----------------------------------------------------------------
// funcitons
// initalize the linkedlist
void Init(int M, int C)
{
	memory_pool = M;
	block = C;
	Head = malloc(M);
	free_pointer = Head;
	existed_nodes = 0;
} 

// destroy the linkedlist
void Destroy()
{
	free(Head);
}

// insert a key-value pair to linkedlist
int Insert(int x, char *value_ptr, int value_len)
{
	// check the remainning memory is enough or not
	if(memory_pool/block - existed_nodes < 1)
	{
		printf("There is not enough memory to insert a node!!\n");
		return -1; // fail
	}
	// check if this value length can git in to the specific block
	if(block - 16 < value_len)
	{
		printf("This value length is too big to fit into this block!!\n");
		return -1; // fail
	}

	node *new_node;
	new_node = free_pointer;
	if(free_pointer != Head)
	{
		node *pre_node = free_pointer - block;
		pre_node -> next_ptr = new_node;
	}
	new_node -> next_ptr = NULL;
	new_node -> key = x;
	new_node -> value_len = value_len;
	memcpy(((char*)new_node) + 16, value_ptr, value_len);
	free_pointer = free_pointer + block;
	existed_nodes = existed_nodes + 1;
	return x; // success
}

// delete the first item from the key x 
void Delete(int x)
{
	node* delete_node = Head;
	// traverse all the existed nodes to find x
	while(delete_node -> key != x )
	{
		if(delete_node -> next_ptr == NULL)
		{
			printf("The node with the key %d doesn't exist!!\n", x);
			return;
		}
		delete_node = delete_node -> next_ptr;
	}

	node* pre_node = delete_node - block;
	node* cur_node = delete_node;
	while(cur_node -> next_ptr != NULL)
		{
			pre_node -> next_ptr = cur_node -> next_ptr;
			pre_node = cur_node;
			cur_node = cur_node -> next_ptr;
		}
		printf("The node with the key %d has been deleted!!\n", x); 
} 

// look up the node in linkedlist which key is x
char* Lookup(int x)
{
	// traverse all the nodes in the linkedlist
	node* temp = Head;
	while(temp -> key != x )
	{
		if(temp -> next_ptr == NULL)
		{
			printf("The node with the key %d doesn't exist!!\n", x); 
			return NULL;
		}
		temp = temp -> next_ptr;
	}
	char* founded = (char*)temp + 8;
	printf("The node with the key %d has been founded!!\n", x); 
	return founded;

} 

// Print out the whole linkedlist
void PrintList()
{
	node* temp = Head;
	while(temp -> next_ptr != NULL )
	{
		printf("key is %d, value length is %d\n", temp -> key, temp -> value_len );
		temp = temp -> next_ptr;

	}
	printf("key is %d, value length is %d\n", temp -> key, temp -> value_len );

} 