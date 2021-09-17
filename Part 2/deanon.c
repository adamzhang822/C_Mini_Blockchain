#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "deanon.h"
#include "cs152coin.h"
#include "hmap.h"

deanon_t* deanon_new()
{
    deanon_t* d = malloc(sizeof(deanon_t));
    if(d == NULL){
        fprintf(stderr,"malloc failed for deanon new\n");
        exit(1);
    }
    d->association_map = hmap_new();
    d->cluster_map=hmap_new();
    d->cluster_list = NULL;
    return d;
}

void deanon_free(deanon_t* d)
{
    hmap_t* association_map = d->association_map;
    for(size_t i = 0; i<association_map->n_buckets;i++){
        bucket_t* current = association_map->buckets[i];
        while(current!=NULL){
            if(current->value!=NULL) cluster_free(current->value);
            current = current->next;
        }
    }
    list_t* tmp = d->cluster_list;
    while(tmp!=NULL){
        cluster_free(tmp->pointer);
        tmp = tmp->next;
    }
    hmap_free(d->association_map);
    hmap_free(d->cluster_map);
    list_free(d->cluster_list);
    free(d);
}

void add_address(const char* addr, deanon_t* d)
{
    cluster_t* cluster = cluster_new(addr);
    hmap_put(addr, cluster, d->association_map);
}

void add_associations(dep_wd_t* trans_inputs[], unsigned int num_inputs, deanon_t* d)
{
    hmap_t* association_map = d->association_map;
    for(int i=0; i<num_inputs;i++){
        cluster_t* current = hmap_get(trans_inputs[i]->addr,association_map);
        if(current == NULL){
            add_address(trans_inputs[i]->addr, d);
            current = hmap_get(trans_inputs[i]->addr,association_map);
        }
        cluster_t* tmp = current;

        // Move to the end of cluster:
        while(tmp->next!=NULL){
            tmp = tmp->next;
        }

        for(int j=0;j<num_inputs;j++){
            if(i!=j){
                if(!cluster_contains(trans_inputs[j]->addr,current)){
                    tmp->next = cluster_new(trans_inputs[j]->addr);
                    tmp = tmp->next;
            }
            }
        }
    }
}

cluster_t* get_cluster(const char* addr, deanon_t* d)
{
    hmap_t* association_map = d->association_map;
    hmap_t* cluster_map = d->cluster_map;
    cluster_t* cluster_out = hmap_get(addr, cluster_map);

    /* Return cluster in O(1) if cluster already discovered */
    if (cluster_out != NULL){
        printf("have already visited this cluster\n");
        return cluster_out;
    }

    /* Discovering new cluster in O(a+b) time */
    cluster_out = cluster_new(addr);
    cluster_t* output = cluster_out;
    queue* q = queue_new();
    cluster_t* current_association = hmap_get(addr, association_map);
    cluster_t* tmp = current_association;
    while(tmp!=NULL){
        enqueue(q, tmp->addr);
        printf("enqueued address: %s\n",tmp->addr);
        tmp = tmp->next;
    }
    cluster_free(current_association);
    bucket_t* b = hmap_get_bucket(addr, association_map);
    b->value = NULL;

    while(q->n > 0){
        char* popped = dequeue(q);
        cluster_t* associated = hmap_get(popped, association_map);
        if(associated!=NULL){
            cluster_out->next = cluster_new(popped);
            cluster_out = cluster_out->next;
            tmp = associated;
            while(tmp!=NULL){
                enqueue(q, tmp->addr);
                tmp = tmp->next;
            }
            cluster_free(associated);
            bucket_t* b = hmap_get_bucket(popped,association_map);
            b->value = NULL;
        }
        free(popped);
    }
    q_free(q);

    /* Store the cluster*/
    cluster_t* tmp2 = output;
    while(tmp2!=NULL){
        hmap_put(tmp2->addr,output,cluster_map);
        tmp2 = tmp2->next;
    }
    d->cluster_list = list_cons(output, d->cluster_list);

    return output;
}

cluster_t* cluster_new(const char* addr)
{
    cluster_t* out = malloc(sizeof(cluster_t));
    if(out == NULL){
        fprintf(stderr, "malloc failed for cluster_new");
        exit(1);
    }
    out->addr = strdup(addr);
    out->next = NULL;
    return out;
}

void cluster_free(cluster_t* c)
{
    while(c!=NULL){
        cluster_t* old = c;
        c = c->next;
        free(old->addr);
        free(old);
    }
}


// ===================================================================
// Add any other functions you need in order to implement deanon below
// ===================================================================

queue *queue_new()
{
    queue *q = malloc(sizeof(queue));
    if(q == NULL){
        fprintf(stderr, "queue malloc failed\n");
        exit(1);
    }
    q->n = 0;
    q->front = q->back = NULL;
    return q;
}

void enqueue(queue *q, char* addr)
{
  cluster_t* cell = cluster_new(addr);
  if(q->n == 0){
    q->front = cell;
    q->back = cell;
  }else{
    q->back->next = cell;
    q->back = cell;
  }
  q->n = q->n + 1;
}

char* dequeue(queue *q)
{
  if(q->n == 0){
    fprintf(stderr, "queue empty");
    exit(1);
  }
  char* address = strdup(q->front->addr);
  cluster_t* tmp = q->front;
  q->front = q->front->next;
  free(tmp->addr);
  free(tmp);
  q->n = q->n - 1;
  return address;
}

void q_free(queue *q)
{
  if (q!=NULL) {
    cluster_free(q->front);
    free(q);
  }
}

list_t* list_cons(cluster_t* cluster, list_t* prev_head)
{
    list_t* new = malloc(sizeof(list_t));
    if (!new) return NULL;
    new->pointer = cluster;
    new->next = prev_head;
    return new;
}

void list_free(list_t* list)
{
    while(list!=NULL){
        list_t* old = list;
        list = list->next;
        free(old);
    }
}
int cluster_contains(char* addr, cluster_t* cluster)
{
    while(cluster!=NULL){
        if(!strcmp(addr,cluster->addr)){
            return 1;
        }
        cluster = cluster->next;
    }
    return 0;
}

