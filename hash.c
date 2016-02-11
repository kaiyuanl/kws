#include "proto.h"

unsigned int kws_hash(struct kws_string str);
void kws_hash_add(struct hlist_head fields[], int bkt, struct kws_string field);
struct kws_string kws_hash_get(struct hlist_head fields[], int bkt, char *field);
