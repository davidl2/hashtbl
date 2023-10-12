#include <string.h>
#include <stdlib.h>
#include "hashtbl.h"

#ifdef DEBUG
#include <stdio.h>
#endif

const int Hashtable::rehash_pct = 78ul;

Hashtable::Hashtable() {
  index = new idx_node[13];
  nbuckets = 13;
  total_count = 0;
  max_count = 0;
}

Hashtable::Hashtable(const Hashtable & other) {
  index = new idx_node[other.nbuckets];
  total_count = other.total_count;
  max_count = other.max_count;

  for (int i = 0; i < nbuckets; i++) {
    if (other.index[i].bucket) {
      index[i].bucket = node = new data_node();
      index[i].count  = other.index[i].count;
      data_node * first=node;
      data_node * ref=other.index[i].bucket;
      for (int j=0; j < index[i].count; j++) {
          node->key = strdup(ref->key);
          node->value = strdup(ref->value);
          data_node * next = new data_node();
          next->prev = node;
          node->next = next;
          node = node->next;
          ref = ref->next;
      }
      node = node->prev;
      delete node->next;
      node->next = first;
      first->prev = node;
    }
  }
}

Hashtable::~Hashtable() {
  for (int i=0; i<nbuckets; i++) {
    if (index[i].bucket) {
      data_node * first = node = index[i].bucket;
      data_node * prev;
      do {
          prev = node;
          node=node->next;
          free(prev->key);
          free(prev->value);
          delete prev;
      } while (node != first);
    }
  }
#ifndef TC1
  delete [] index;
#else
  delete index;
#endif
#ifdef DEBUG
  nbuckets = 0xDDBFu;
  total_count = 0xDDBEu;
  max_count = 0xDDBCu;
#endif
}

unsigned int Hashtable::hash(const char * s) {
  unsigned int h = 0;
  for ( ; *s; s++) {
    h += h*31 ^ *s;
  }
  return h;
}

void Hashtable::rehash() {
#ifdef DEBUG
    fprintf(stderr,"Rehashing %p (%d/%d)...\n",this,total_count,nbuckets);
#endif
  idx_node * old_index = index;
  int nnbuckets=nbuckets*2;
  index = new idx_node[nnbuckets];
  max_count = 0;
  for (int i=0; i<nbuckets; i++) {
    if (old_index[i].bucket) {
      data_node * ofirst=old_index[i].bucket,*oprev,*onode=ofirst;
      do {
        oprev=onode;
        onode=onode->next;
        unsigned int HH = hash(oprev->key)%nnbuckets; // new hash value
        if (index[HH].bucket==NULL) {
          index[HH].count = 1;
          node = new data_node;
	  index[HH].bucket = node;
          node->key = oprev->key;
          node->value = oprev->value;
        } else {
          index[HH].count ++;
          data_node * first = index[HH].bucket;
          data_node * last = first->prev;
          node = new data_node;
          node->key = oprev->key;
          node->value = oprev->value;
          node->next = first;
          first->prev = node;
          node->prev = last;
          last->next = node;
          index[HH].bucket = node;
        }
        delete oprev;
        if (max_count < index[HH].count)
          max_count = index[HH].count;
      } while (onode != ofirst);
    }
  }
#ifndef TC1
  delete [] old_index;
#else
  delete old_index;
#endif
  nbuckets = nnbuckets;
}

bool Hashtable::find(const char *key) {
  bucketnr = hash(key) % nbuckets;
  if (index[bucketnr].bucket==NULL) {
    node = NULL;
    return false;
  }
  node = index[bucketnr].bucket;
  data_node * first = node,*next = first->next;
  if (strcmp(key,node->key)==0) {
    index[bucketnr].bucket = node;
    return true;
  }
  do {
    node=next;
    next=node->next;
    if (strcmp(key,node->key)==0) {
      index[bucketnr].bucket = node;
      return true;
    }
  } while (node != first);
  return false;
}

bool Hashtable::insert(const char*key,const char*value) {
  if (key==NULL) {
    if (nullvalue) {
      free(nullvalue);
      nullvalue = strdup(value);
      return true;
    } else {
      nullvalue = strdup(value);
      return false;
    }
  } else if (value==NULL) {
    return erase(key);
  } else if (find(key)) {
    free(node->value);
    node->value = strdup(value);
    return true;
  }
  if (node!=NULL /* see below */
      && total_count*100l/nbuckets > rehash_pct) {
    rehash();
    find(key);
  }
  if (node==NULL /* not found, bucket does not exist */) {
    index[bucketnr].count = 1;
    if (max_count < 1)
      max_count = 1;
    total_count ++;
    node = new data_node();
    index[bucketnr].bucket = node;
    node->key = strdup(key);
    node->value = strdup(value);
    return false;
  } else {
    index[bucketnr].count ++;
    if (max_count < index[bucketnr].count)
      max_count = index[bucketnr].count;
    data_node * prev=node,*next=node->next;
    node = new data_node;
    node->prev = prev;
    node->prev->next=node;
    node->next = next;
    node->next->prev=node;
    node->key = strdup(key);
    node->value = strdup(value);
    return false;
  }
}
    
bool Hashtable::erase(const char *key) {
  if (key==NULL) {
    if (nullvalue==NULL) {
      free(nullvalue);
      return true;
    } else {
      return false;
    }
  } else if (find(key)) {
    if (index[bucketnr].count > 1) {
      index[bucketnr].bucket = node->next;
      index[bucketnr].count --;
      node->next->prev = node->prev;
      node->prev->next = node->next;
      free(node->key);
      free(node->value);
      delete node;
    } else {
      free(node->key);
      free(node->value);
      delete node;
      index[bucketnr].bucket = NULL;
    }
    total_count --;
    return true;
  } else {
    return false;
  }
}

const char * Hashtable::query(const char *key) {
  if (find(key)) {
    return node->value;
  } else {
    return NULL;
  }
}

#ifdef DEBUG
void Hashtable::dump() {
   FILE * f = fopen("dump.txt","w");
  fprintf(f,"Hashtable %p\n", this);
  fprintf(f,"buckets = %d  max = %d  total = %d  node = %p  index = %p\n",
	  nbuckets, max_count, total_count,node,index);
  for (int i = 0; i<nbuckets; i++) {
    int j = 0;
    fprintf(f,"[%02d] (%d) ",i,index[i].bucket?index[i].count:0);
    node = index[i].bucket;
    data_node * first = node;
    if (node) {
      fprintf(f,"<%p> %s/%s  ",node,node->key,node->value);
      node=node->next;
      j++;
      while (node != first) {
	fprintf(f,"<%p> %s/%s  ",node,node->key,node->value);
	node=node->next;
	j++;
	if (j > index[i].count) {
	  fprintf(f," !open loop at <%p>!",node);
	  break;
	}
      };
    }
    fprintf(f,"\n");
  }
   fclose(f);
}
#endif













