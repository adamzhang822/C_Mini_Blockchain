/* Adam (Yan) Zhang 
 * CS 152 Win 18
 * Project
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "cs152coin.h"
#include "addr_bal.h"
#include "dbl_spends.h"


// This function is already implemented as an example of how to use the parser
dep_wd_t* parse_dep_wd()
{
    char* s = NULL;
    double d = 0;
    
    if (!read_string(&s)) return NULL;
    dep_wd_type type;
    if (!strcmp(s, "DEPOSIT")) {
        type = DEPOSIT;
    } else if (!strcmp(s, "WITHDRAWAL")) {
        type = WITHDRAWAL;
    } else {
        return NULL;
    }
    next_token();
    
    dep_wd_t* dw = calloc(1, sizeof(dep_wd_t));
    if (dw == NULL) {
        return NULL;
    }
    dw->type = type;
    
    if (!read_string(&s)) {
        dep_wd_free(dw);
        return NULL;
    }
    dw->addr = strdup(s);
    next_token();
    
    if (!read_double(&d)) {
        dep_wd_free(dw);
        return NULL;
    }
    dw->amount = d;
    next_token();

    return dw;
}

trans_t* parse_trans()
{
    char* s = NULL;
    if (!read_string(&s)) return NULL;
    if (strcmp(s, "BEGIN_TRANSACTION")) return NULL;
    trans_t *trans = calloc(1, sizeof(trans_t));
    if (trans == NULL){
        return NULL;
    }
    next_token();
    if (!read_string(&s)){
        trans_free(trans);
        return NULL;
    }
    trans->id = strdup(s);

    next_token();
    if (!read_string(&s) || (strcmp(s, "BEGIN_INPUTS"))){
        trans_free(trans);
        return NULL;
    }
    next_token();
    if(!read_string(&s) || (strcmp(s,"WITHDRAWAL"))){
        trans_free(trans);
        return NULL;
    }
    unsigned int num_inputs = 0;
    for (int i = 0; i < 5; i++){
        trans->inputs[i] = parse_dep_wd();
        if(!read_string(&s)){
            trans_free(trans);
            return NULL;
        }
        if (!strcmp(s,"END_INPUTS") && trans->inputs[i]!=NULL){
            num_inputs++;
            break;
        }
        num_inputs++;
    }
    trans->num_inputs = num_inputs;
    next_token(); 

    if(!read_string(&s) || (strcmp(s, "DEPOSIT"))){
        trans_free(trans);
        return NULL;
    }
    trans->output = parse_dep_wd();

    if(!read_string(&s) || strcmp(s, "END_TRANSACTION")){
        trans_free(trans);
        return NULL;
    }
    next_token();
    return trans;
} 

block_t* parse_block()
{
    char *s = NULL;
    if(!read_string(&s) || strcmp(s, "BEGIN_BLOCK")) return NULL;
    block_t* block = calloc(1, sizeof(block_t));
    if (block == NULL) return NULL;
    next_token();

    if(!read_string(&s) || strcmp(s,"DEPOSIT")){
        block_free(block);
        return NULL;
    }
    block->miner = parse_dep_wd();

    if(!read_string(&s) || strcmp(s, "BEGIN_TRANSACTION")){
        block_free(block);
        return NULL;
    }
    block->trans = parse_trans();
    if(block->trans == NULL || !read_string(&s)){
        block_free(block);
        return NULL;
    }
    trans_t *tmp = block->trans;

    while(read_string(&s) && strcmp(s,"END_BLOCK") && tmp != NULL){
        tmp->next = parse_trans();
        tmp = tmp->next;
    }
    if(!read_string(&s) || tmp == NULL){
        block_free(block);
        return NULL;
    }
    tmp->next = NULL;

    next_token();
    return block;
}

blockchain_t* parse_blockchain()
{
    char* s = NULL;
    if(!read_string(&s) || strcmp(s, "BEGIN_BLOCK")) return NULL;
    blockchain_t *blockchain = calloc(1, sizeof(blockchain_t));
    if (blockchain == NULL){
        fprintf(stderr,"error in parsing");
        exit(1);
    }

    blockchain->blocks = parse_block();
    if(!read_string(&s) || blockchain->blocks == NULL){
        blockchain_free(blockchain);
        fprintf(stderr, "error in parsing");
        exit(1);
    }
    block_t *tmp = blockchain->blocks;

    while(read_string(&s)){
        tmp->next = parse_block();
        tmp = tmp->next;
        if (tmp == NULL){
            fprintf(stderr, "error in parsing");
            exit(1);
        }
    }
    tmp->next = NULL;

    return blockchain;
}

void dep_wd_free(dep_wd_t* dw)
{
    free(dw->addr);
    free(dw);
}

void trans_free(trans_t* trans)
{
    free(trans->id);
    dep_wd_free(trans->output);
    int num_t = trans->num_inputs;
    for (int i = 0; i<num_t; i++){
        dep_wd_free(trans->inputs[i]);
    }
    free(trans);
}

void block_free(block_t* block)
{
    dep_wd_free(block->miner);
    trans_t *tmp;
    trans_t *current = block->trans;
    while(current!=NULL){
        tmp = current;
        current = current->next;
        trans_free(tmp);
    }
    free(block);
}

void blockchain_free(blockchain_t* bc)
{
    block_t *tmp;
    block_t *current = bc->blocks;
    while(current!=NULL){
        tmp = current;
        current = current->next;
        block_free(tmp);
    }
    addr_bal_free(bc->addr_bal);
    dbl_spends_free(bc->dbl_spends);
    free(bc);
}

void set_balance_helper(dep_wd_t* dw, addr_bal_t* ab){
    if(!ab_contains(dw->addr, ab))
        ab_insert(dw->addr,ab);
    if (dw->type == DEPOSIT){
        set_balance(dw->addr, dw->amount, ab);
    }else{
        set_balance(dw->addr, -(dw->amount),ab);
    }
}

int detect_ds(dep_wd_t* dw, addr_bal_t *ab){
    double amount = (dw->amount);
    double* balance = malloc(sizeof(double));
    get_balance(dw->addr, balance, ab);
    if (*balance - amount < 0){
        free(balance);
        return 1;
    }
    free(balance);
    return 0;
}

int dbl_spends_helper(blockchain_t* bc, container* cont, trans_t* trans, dep_wd_t* dw, addr_bal_t* ab){
    if (!detect_ds(dw,ab)) return 0;
    if (cont->last != NULL){
        add_dbl_spend(trans->id, dw->addr,cont);
    }else{
        dbl_spends_t* first = dbl_spends_new(trans->id, dw->addr);
        bc->dbl_spends = first;
        cont->first = first;
        cont->last = first;
    }
    return 1;
}

void compute_stats(blockchain_t* bc)
{
    int count_b = 0;
    int count_t = 0;
    double trans_val = 0.0;
    addr_bal_t* ab = addr_bal_new(10);
    bc->addr_bal = ab;
    bc->dbl_spends = NULL;
    container* cont = container_new();
    cont->first = bc->dbl_spends;
    cont->last = bc->dbl_spends;

    block_t *current_b = bc->blocks;
    while(current_b != NULL){
        count_b++;
        set_balance_helper(current_b->miner, ab);
        // Disabling dynamic hash table for efficiency issue
        if(detect_overload(ab)){
            addr_bal_t* reloaded = reload((ab));
            bc->addr_bal = reloaded;
            ab = reloaded;
       } 
        trans_t *current_t = current_b->trans;
        while(current_t != NULL){
            count_t++;
            trans_val = trans_val + current_t->output->amount;
            for (int i = 0; i < current_t->num_inputs; i++){
                dbl_spends_helper(bc, cont, current_t, current_t->inputs[i], ab);
                set_balance_helper(current_t->inputs[i],ab);
                if(detect_overload(ab)){
                    addr_bal_t* reloaded = reload((ab));
                    bc->addr_bal = reloaded;
                    ab = reloaded;
                }
            }
            set_balance_helper(current_t->output, ab);
            if(detect_overload(ab)){
                addr_bal_t* reloaded = reload((ab));
                bc->addr_bal = reloaded;
                ab = reloaded;
            }
            current_t = current_t->next;
        }
        current_b = current_b->next;
    }
    bc->num_blocks = count_b;
    bc->num_trans = count_t;
    bc->avg_trans_value = trans_val / count_t;
    free(cont);
}

static void print_stats(blockchain_t* bc)
{
    if (bc == NULL) return;
    
    printf("Number of blocks: %lu\n", bc->num_blocks);
    printf("Number of transactions: %lu\n", bc->num_trans);
    printf("Average transaction value: %lf\n", bc->avg_trans_value);
    printf("\n");
    
    printf("Double spends:\n");
    dbl_spends_show(bc->dbl_spends, stdout); 
    printf("\n");
}

// Auxilliary functions for debugging 
void show_dep_wd_t(dep_wd_t* dep){
    if (dep->type == DEPOSIT){
        printf("the type is deposit\n");
    }else if(dep->type == WITHDRAWAL){
        printf("the type is withdrawal\n");
    }
    printf("the address is %s\n", dep->addr);
    printf("the amount is %f\n", dep->amount);
}
void show_trans_t(trans_t* trans){
    printf("******the id for this trans is%s ******\n", trans->id);
    printf("num inputs is %d\n", trans->num_inputs);
    for (int i = 0; i < trans->num_inputs; i++){
        printf("this is input [%d]\n", i+1);
        show_dep_wd_t(trans->inputs[i]);
    }
    printf("this is output\n");
    show_dep_wd_t(trans->output);
    printf("******this is the end of this transaction****\n");
}
void show_block_t(block_t* block){
    printf("&&&&&& this is the start of a block &&&&&\n");
    printf("this is the miner\n");
    show_dep_wd_t(block->miner);
    trans_t* tmp = block->trans;
    while(tmp!=NULL){
        show_trans_t(tmp);
        tmp = tmp->next;
    }
    printf("&&&&&&&this is the end of this block&&&&&&&&&\n");
}
void show_bc(blockchain_t* blockchain){
    printf("@@@@@@@@@ this is the start of a chain @@@@@@@@\n");
    block_t* tmp = blockchain->blocks;
    while(tmp!=NULL){
        show_block_t(tmp);
        tmp = tmp->next;
    }
    printf("@@@@@@@ this is the end of this chain @@@@@@@\n");
}

int main(int argc, char* argv[])
{
    init_parser();
    blockchain_t* bc = parse_blockchain();
    compute_stats(bc);
    print_stats(bc);

    for (int i = 1; i < argc; i++) {
        double bal = 0;
        if (get_balance(argv[i], &bal, bc->addr_bal)) {
            printf("Balance for address %s: %lf\n", argv[i], bal);
        } else {
            printf("Address %s: not found\n", argv[i]);
        }
    }

    blockchain_free(bc);
}
