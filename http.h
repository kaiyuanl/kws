#ifndef _HTTP_H_
#define _HTTP_H_
/* HTTP methods */
#define GET			0x01
#define POST			0x02
#define PUT			0x03
#define DELETE			0x04

#define HTTPVER11		11
#define HTTPVER10		10
#define HTTPVER09		9

/* HTTP fields */
#define ACCEPT			"Accept"
#define ACCEPTCHARSET		"Accept-Charset"
#define ACCEPTENCODING		"Accept-Encoding"
#define ACCEPTLANGUAGE		"Accept-Language"
#define AUTHORIZATION		"Authorization"
#define EXPECT			"Expect"
#define FROM			"From"
#define HOST			"Host"
#define IFMATCH			"If-Match"
#define IFMODIFIEDSINCE		"If-Modified-Since"
#define IFNONEMATCH		"If-None-Match"
#define IFRANGE			"If-Range"
#define IFUNMODIFIEDSINCE	"If-Unmodified-Since"
#define MAXFORWARDS		"Max-Forwards"
#define PROXYAUTHORIZATION	"Proxy-Authorization"
#define RANGE			"Range"
#define	REFERER			"Referer"
#define TE			"TE"
#define	USERAGENT		"User-Agent"

/* Connection status */
#define KEEPALIVE		0x01
#define CLOSE			0x02

#endif
