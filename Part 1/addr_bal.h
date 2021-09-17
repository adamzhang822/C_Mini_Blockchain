/* Adam (Yan) Zhang 
 * CS 152 Win 18
 * Project
*/

#ifndef _ADDR_BAL_H
#define _ADDR_BAL_H


typedef struct account{
	char* addr;
	unsigned long int hash;
	double balance;
	struct account* next;
}account;

typedef struct addr_bal{
	unsigned long int(*hash_func)(const char*);
	account** accounts;
	size_t n_accounts;
}addr_bal_t;


/* === addr_bal operations === */
// Added a size parameter for dynamic hash table 
addr_bal_t* addr_bal_new(size_t sz);

// Frees a heap-allocated data structure for tracking the balance of each address.
void addr_bal_free(addr_bal_t* ab);

// Get the balance associated with address addr and store the result in the double
// pointed to by out_bal.
// Returns 0 if the address couldn't be found and nonzero otherwise.
int get_balance(const char* addr, double* out_bal, addr_bal_t* ab);

// Set the balance associated with address addr to bal.
void set_balance(const char* addr, double bal, addr_bal_t* ab);

// ========================================
// Extra functions
// ========================================

/* === hashing === */
// hash function imported from lab4.
unsigned long int hash(const char* s);

/* === account operations  === */
// construct a new account, balance is initialized to 0.
account* account_cons(const char* address, unsigned long int hash, account* prev_head);

// compute the total number of accounts in this bucket.
size_t account_size(account* a);

// compute whether the bucket contains a particular address.
int account_contains(const char* address, unsigned long int hash, account* a);

// returns a pointer to target account with specified address.
account* account_get(const char* address, unsigned long int hash, account* a);

void account_show(account* a);

void account_free(account* a);

/* === addr_bal operations === */
// Returns the number of total individual accounts in the addr_bal struct
size_t ab_size(addr_bal_t *ab);

// Compute the load factor
double ab_load_factor(addr_bal_t* ab);

// Insert a new account into the hash table
size_t ab_insert(const char* address, addr_bal_t* ab);

// Comutes whether an address is already in hash table
int ab_contains(const char* address, addr_bal_t* ab);

void ab_show(addr_bal_t* ab);

void addr_bal_free(addr_bal_t* ab);

/* === reloading operations  === */
// compute whether the load factor is too high
int detect_overload(addr_bal_t* ab);

// reload the hash table with expanded bucket number 
addr_bal_t* reload(addr_bal_t* ab);


#endif /* _HSET_H */
