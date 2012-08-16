#include "mgUtil/Include/mgUtil.h"

#ifdef __unix__
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef int SOCKET;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

typedef struct sockaddr SOCKADDR;

#define closesocket close

#define _fstat fstat
#define _open open
#define _read read
#define _close close
#define _lseek lseek
#define _O_RDONLY O_RDONLY
#define _O_BINARY 0
#define _stat stat
#define _mkgmtime timegm

#endif

