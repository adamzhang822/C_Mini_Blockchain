/* Adam (Yan) Zhang 
 * CS 152 Win 18
 * Project
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dbl_spends.h"

dbl_spends_t* dbl_spends_new(const char* trans_id, const char* addr){
	dbl_spends_t* ds = malloc(sizeof(dbl_spends_t));
	if (ds == NULL) return NULL;
	ds->addr = strdup(addr);
	ds->id = strdup(trans_id);
	ds->next = NULL;
	return ds;
}

void dbl_spends_free(dbl_spends_t* ds)
{
	dbl_spends_t* tmp = ds;
    while(tmp != NULL){
    	free(ds->addr);
    	free(ds->id);
    	ds = ds->next;
    	free(tmp);
    	tmp = ds;
    }
}

// Changed the function arguement type to container pointer
// To follow the original function, one can simply traverse through all 
// the dbl_spends->next till hit null, then add the new dbl_spend
// however, as container structure is more efficient, I have made this change.
void add_dbl_spend(const char* trans_id, const char* addr, container* container)
{	
	dbl_spends_t* new = dbl_spends_new(trans_id, addr);
	container->last->next = new;
	container->last = new;
}

void dbl_spends_show(dbl_spends_t* ds, FILE* f)
{	
	while(ds!=NULL){
		fprintf(f, "<%s>, <%s>\n",ds->id, ds->addr);
		ds=ds->next;
	}
}

container* container_new(){
	container* new = malloc(sizeof(container));
	if (new == NULL) return NULL;
	new->first = NULL;
	new->last = NULL;
	return new;
}