/* Adam (Yan) Zhang 
 * CS 152 Win 18
 * Project
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "addr_bal.h"

/* === hashing === */
unsigned long int hash(const char* s)
{
  unsigned long int res = 17;
  int slen = strlen(s);
  for (int i=0; i < slen || i < 9 ; i++){
    res = 37 * res + s[i];
  }
  return res;
}

/* === account operations  === */
account* account_cons(const char* address, unsigned long int hash, account* prev_head){
	account* output = malloc(sizeof(account));
	if (output == NULL) return NULL;
	output -> addr = strdup(address);
	output -> hash = hash;
	output -> next = prev_head;
	output -> balance = 0.0;
	return output;
}

size_t account_size(account* a){
	size_t size = 0;
	while(a != NULL){
		size++;
		a = a->next;
	}
	return size;
}

int account_contains(const char* address, unsigned long int hash, account* a){
	size_t size = account_size(a);
	for (int i = 0; i<size; i++){
		if(a->hash == hash){
			if(strcmp(address, a->addr)==0)
				return 1;
		}
	a = a->next;
	}
	return 0;
}

account* account_get(const char* address, unsigned long int hash, account* a){
	size_t size = account_size(a);
	for (int i = 0; i<size; i++){
		if(a->hash == hash){
			if(strcmp(address, a->addr)==0)
				return a;
		}
	a = a->next;
	}
	return NULL;
}

void account_show(account* a){
	printf("the account address is: %s\n", a->addr);
	printf("the account balance is: %f\n", a->balance);
}

void account_free(account* a){
	size_t size = account_size(a);
	for (int i = 0; i <size; i++){
		account* old = a;
		a = a->next;
		free(old->addr);
		free(old);
	}
}

/* === addr_bal_t operations  === */

addr_bal_t* addr_bal_new(size_t sz){
    addr_bal_t* hset = malloc(sizeof(addr_bal_t));
    if (hset==NULL) return NULL;
    account **accounts = calloc(sz, sizeof(account*));
    if(accounts == NULL) return NULL;

    hset->hash_func = &hash;
    hset->n_accounts = sz;
    hset->accounts = accounts;
    for(int i = 0; i<sz; i++){
    	hset -> accounts[i] = NULL;
    }
    return hset;
}

void addr_bal_free(addr_bal_t* ab)
{
	size_t sz = ab -> n_accounts;
	for (int i = 0; i < sz; i++)
		account_free(ab->accounts[i]);
	free(ab->accounts);
	free(ab);
}


size_t ab_size(addr_bal_t *ab){
	size_t size = 0;
	int sz = ab->n_accounts;
	for (int i = 0; i<sz; i++){
		size += account_size(ab->accounts[i]);
	}
	return size;
}

double ab_load_factor(addr_bal_t* ab){
	return 1.0 * ((ab_size(ab) * 1.0)/ (ab->n_accounts));
}

int ab_contains(const char* address, addr_bal_t* ab)
{
	unsigned long int(*f)(const char*);
	f = ab->hash_func;
	int sz = ab->n_accounts;
	unsigned long int hash = (*f)(address);
	int index = hash%sz;
	return account_contains(address, hash, ab->accounts[index]);
}

size_t ab_insert(const char* address, addr_bal_t* ab)
{
	unsigned long int(*f)(const char*);
	f = ab -> hash_func;
	unsigned long int hash = (*f)(address);
	int index = hash%(ab->n_accounts);
	account* new_account = account_cons(address, hash, ab->accounts[index]);
	ab -> accounts[index] = new_account;
	return account_size(new_account);
}

void ab_show(addr_bal_t* ab)
{
	size_t sz = ab->n_accounts;
	for (int i = 0; i < sz; i++){
		printf("this is bucket %d\n", i+1);
		account* a = ab->accounts[i];
		size_t sza = account_size(a);
		for (int j = 0; j<sza;j++){
			account_show(a);
			a = a->next;
		}
		printf("\n");
	}
}


void set_balance(const char* addr, double bal, addr_bal_t* ab)
{
    unsigned long int(*f)(const char*);
	f = ab->hash_func;
	int sz = ab->n_accounts;
	unsigned long int hash = (*f)(addr);
	int index = hash%sz;
    account* target_account = account_get(addr, hash, ab->accounts[index]);
    target_account->balance += bal;
}

int get_balance(const char* addr, double* out_bal, addr_bal_t* ab)
{
	if (!ab_contains(addr, ab)) return 0;
	unsigned long int(*f)(const char*);
	f = ab->hash_func;
	int sz = ab->n_accounts;
	unsigned long int hash = (*f)(addr);
	int index = hash%sz;
	account* target_account = account_get(addr, hash, ab->accounts[index]);
	*out_bal = target_account->balance;
	return 1;
}

/* === reloading operations  === */
int detect_overload(addr_bal_t* ab){
	if (ab_load_factor(ab) > 1.20) return 1;
	return 0;
}

addr_bal_t* reload(addr_bal_t* ab){
	addr_bal_t* new = addr_bal_new((ab->n_accounts)*2);
	size_t old_size = ab->n_accounts;
	for (int i = 0; i < old_size; i++){
		account* a = ab->accounts[i];
		size_t asize = account_size(a);
		for (int j =0; j<asize; j++){
			ab_insert(a->addr, new);
			set_balance(a->addr, a->balance, new);
			a = a->next;
		}
	}
	addr_bal_free(ab);
	return new;
}

