#ifndef HASHTBL_H
#define HASHTBL_H

#ifdef TC1
typedef int bool;
enum { false, true };
#endif

class Hashtable {
#ifdef DEBUG
  public:
#endif
    // using an array of pointers to doubly-linked lists
    struct data_node { char * key; char * value; data_node *next,*prev;
                       data_node() {next=prev=this;} };
    struct idx_node {  data_node *bucket; int count;
                       idx_node() {bucket=NULL;} };
    idx_node * index;

    char * nullvalue;
    int nbuckets;
    int total_count;  // except the nullvalue
    int max_count;

    static unsigned int hash(const char *);
    void rehash();
    static const int rehash_pct;

    Hashtable& operator=(const Hashtable&) {return *this;}

    bool find(const char * key);
    data_node * node;
    int bucketnr;
public:
    bool insert(const char *key,const char *value);
    bool erase(const char *key);
    const char* query(const char *key);

    Hashtable();
    Hashtable(const Hashtable &);
    ~Hashtable();

#ifdef DEBUG
  void dump();
#endif
};


#endif
