/* Adam (Yan) Zhang 
 * CS 152 Win 18
 * Project
*/

#ifndef _DBL_SPENDS_H
#define _DBL_SPENDS_H

#include <stdio.h>

// ========================================================================
// Declare any other structs you need in order to implement dbl_spends here
// ========================================================================

typedef struct dbl_spends{
	char* addr;
	char* id;
	struct dbl_spends* next;
}dbl_spends_t;


typedef struct container{
	dbl_spends_t* first;
	dbl_spends_t* last;
}container;




// Returns a new heap-allocated data structure for tracking double spends.
dbl_spends_t* dbl_spends_new(const char* addr, const char* id);

// Frees a heap-allocated data structure for tracking double spends.
void dbl_spends_free(dbl_spends_t* ds);

// Add a double spend entry consisting of the id transaction of the transaction
// containing the double spend and the address whose balance went negative.
void add_dbl_spend(const char* trans_id, const char* addr, container* ds);

// Print all of the double spend entries IN THE ORDER THEY APPEAR IN THE BLOCKCHAIN
// to the file f. Output should be of the form:
// <trans_id>, <addr>
// <trans_id>, <addr>
// ...
void dbl_spends_show(dbl_spends_t* ds, FILE* f);

//Construct a new container
container* container_new();


#endif