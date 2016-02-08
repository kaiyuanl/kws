unsigned int times33(const char *char_key, ssize_t *klen)
{
	unsigned int  hash = 0;
	const  unsigned  char  *key = ( const  unsigned  char  *)char_key;
	const  unsigned  char  *p;
	ssize_t i;

	if  (*klen == APR_HASH_KEY_STRING) {
		for  (p = key; *p; p++) {
			hash = hash * 33 + *p;
		}
		*klen = p - key;
	} else {
		for  (p = key, i = *klen; i; i--, p++) {
			hash = hash * 33 + *p;
		}
	}
	return  hash;
}
