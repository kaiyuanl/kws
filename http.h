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

#define URLMAX			512 /*2083*/

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

/* HTTP Status Codes */
#define MSG200			"OK"
#define MSG400			"Bad Request"
#define MSG401			"Unarthorized"
#define MSG403			"Forbidden"
#define MSG404			"Not Found"
#define MSG405			"Method Not Allowed"
#define MSG500			"Internal Server Error"
#define MSG501			"Not Implemented"
#define MSG505			"HTTP Version Not Supported"

#define HTTPRESP					\
"HTTP/1.1 200 OK"					\
"Date: Mon, 27 Jul 2009 12:28:53 GMT"			\
"Server: Kaiyuan's Web Server"				\
"Content-Type: text/html"				\
"Connection: Closed"


#define INDEXHTML					\
"<html>"						\
"<title>Kws HTTP Server</title>"			\
"<body>"						\
"<h1>Kws HTTP Server</h1>"				\
"Default Home Page"					\
"</body>"						\
"</html>"
#endif
