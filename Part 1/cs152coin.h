/* Adam (Yan) Zhang 
 * CS 152 Win 18
 * Project
*/

#ifndef _CS152COIN_H
#define _CS152COIN_H

#include "parser.h"
#include "addr_bal.h"
#include "dbl_spends.h"

// ======================================
// Blockchain data structure declarations
// ======================================

// A deposits or withdrawl
typedef enum dep_wd_e {DEPOSIT, WITHDRAWAL} dep_wd_type;

typedef struct dep_wd {
    char* addr;
    dep_wd_type type;
    double amount;
} dep_wd_t;

// A transaction
typedef struct trans {
    char* id; // Transaction id
    // Up to 5 input withdrawals
    dep_wd_t* inputs[5];
    unsigned int num_inputs;
    // Output deposit
    dep_wd_t* output;
    struct trans* next;
} trans_t;

// A block
typedef struct block {
    dep_wd_t* miner; // Deposit of the mining reward
    trans_t* trans; // List of transactions
    struct block* next;
} block_t;

// The entire blockchain
typedef struct blockchain {
    block_t* blocks; // List of blocks
    
    unsigned long int num_blocks; // Total number of blocks. Compute this.
    unsigned long int num_trans; // Total number of transactions. Compute this.
    double avg_trans_value; // Average value of a transaction
    
    addr_bal_t* addr_bal; // Data structure for tracking the balance associate with each address
    dbl_spends_t* dbl_spends; // Data structure for tracking double spends.
} blockchain_t;

// ========================================
// Blockchain parsing function declarations
// ========================================

// Read a deposit or withdrawal from stdin and return a pointer to a heap-allocated dep_wd_t.
// Returns NULL if a dep_wd_t couldn't be parsed (e.g., if the end of the input was reached).
dep_wd_t* parse_dep_wd();

// Read a transaction from stdin and return a pointer to a heap-allocated trans_t.
// Returns NULL if a trans_t couldn't be parsed (e.g., if the end of the input was reached).
trans_t* parse_trans();

// Read a block from stdin and return a pointer to a heap-allocated block_t.
// Returns NULL if a block_t couldn't be parsed (e.g., if the end of the input was reached).
block_t* parse_block();

// Read an entire blockchain from stdin and return a pointer to a heap-allocated blockchain_t.
// Returns NULL if a blockchain_t couldn't be parsed 
// (e.g., if the end of the input was reached unexpectedly).
blockchain_t* parse_blockchain();

// Free a deposit or withdrawal
void dep_wd_free(dep_wd_t* dw);

// Free a transaction
void trans_free(trans_t* trans);

// Free a block
void block_free(block_t* block);

// Free the entire blockchain
void blockchain_free(blockchain_t* bc);

// Once the blockchain has been parsed, this function should:
// - Compute the total number of blocks
// - Compute the total number of transactions.
// - Compute the average value of a transaction.
// - Find and store the balance associated with each account.
// - Find all double spends.
void compute_stats(blockchain_t* bc);

// ========================================
// Extra functions
// ========================================

// Auxilliary functions for debugging 
void show_dep_wd_t(dep_wd_t* dep);
void show_trans_t(trans_t* trans);
void show_block_t(block_t* block);
void show_bc(blockchain_t* blockchain);

// update balance on address given its status in hash table
void set_balance_helper(dep_wd_t* dw, addr_bal_t* ab);
// detect if a transaction has double spending 
int detect_ds(dep_wd_t* dw, addr_bal_t *ab);
// add to double spending struct 
int dbl_spends_helper(blockchain_t* bc, container* cont, trans_t* trans, dep_wd_t* dw, addr_bal_t* ab);



#endif