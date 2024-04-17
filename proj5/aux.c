/**************  DO NOT CHANGE  THIS FILE  ******************/
#include <stdio.h>  // for perror()
#include <stdlib.h> // for exit()
#include <unistd.h> // for read() and write()
#include <string.h> // for memcpy()

#define BUFSIZE 256

void fail(char *s1) {
  perror(s1);
  exit(3);
}

/* read a <string,int> pair from the file open at fd.
 * string is represented by a 1-byte length, followed by that many bytes
 * (no terminal null), followed by 4-byte int.
 * Precondition: strbuf (destination) is at least MAXSTRING bytes;
 */
int readpair(int fd, char *strbuf, int *countp) {
  char buf[BUFSIZE];
  unsigned char x;
  int rv;
  rv = read(fd,buf,BUFSIZE);
  if (rv < 0)
    fail("read() in readpair()");
  if (rv) {
    x = *(unsigned char *)buf;
    memcpy(strbuf,buf+1,x);
    strbuf[x] = 0;
    memcpy(countp,buf+1+x,sizeof(int));
  }
  return rv;
}

int writepair(int fd, char *strbuf, int *countp) {
  int rv;
  unsigned char buf[BUFSIZE];
  int len = strlen(strbuf);
  if (len >= BUFSIZE)
    fail("writepair: string too long");
  buf[0] = (unsigned char) len;
  memcpy(buf+1,strbuf,len);
  memcpy(buf+1+len,countp,sizeof(int));
  len += 1 + sizeof(int);
  rv = write(fd,buf,len);
  if (rv < len)
    fail("writepair - short write");
  else
    return rv;
}
