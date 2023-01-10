#include "robust_inout.hpp"

namespace rio
{
ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
	int cnt;

	while (rp->rio_cnt <= 0) /* Refill if buf is empty */
	{
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
		if (rp->rio_cnt < 0)
		{
			if (errno != EINTR) 
				return -1;
		}
		else if (rp->rio_cnt == 0) /* EOF */
			return 0;
		else
			rp->rio_bufptr = rp->rio_buf; /* Reset buffer ptr */
	}

	/* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
	cnt = n;
	if (rp->rio_cnt < static_cast<int>(n))
		cnt = rp->rio_cnt;
	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;
	return cnt;
}
void rio_readinitb(rio_t *rp, int fd)
{
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
	int n, rc;
	char c, *bufp = reinterpret_cast<char *>(usrbuf);

	for (n = 1; n < static_cast<int>(maxlen); ++n)
	{
		rc = rio_read(rp, &c, 1);
		if (rc == 1)
		{
			*bufp++ = c;
			if (c == '\n')
			{
				++n;
				break;
			}
		}
		else if (rc == 0)
		{
			if (n == 1)
				return 0; /* EOF, no data read */
			else
				break; /* EOF, some data was read */
		}
		else
			return -1;
	}
	*bufp = 0;
	return n - 1;
}
ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = reinterpret_cast<char *>(usrbuf);

	while (nleft > 0)
	{
		nread = read(fd, bufp, nleft);
		if (nread < 0)
		{
			if (errno == EINTR) /* Interrupted by sig handler return */
				nread = 0; /* and call read() agina */
			else
				nread = -1; /* errno set by read() */
		}
		else if (nread == 0)
			break; /* EOF */
		nleft -= nread;
		bufp += nread;
	}
	return n - nleft;	/* return >= 0 */
}
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = reinterpret_cast<char *>(usrbuf);

	while (nleft > 0)
	{
		nread = rio_read(rp, bufp, nleft);
		if (nread < 0)
			return -1; /* errno set by read() */
		else if (nread == 0)
			break; /* EOF */
		nleft -= nread;
		bufp += nread;
	}
	return n - nleft; /* return >= 0 */
}
ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nwritten;
	char *bufp = reinterpret_cast<char *>(usrbuf);

	while (nleft > 0)
	{
		nwritten = write(fd, bufp, nleft);
		if (nwritten <= 0)
		{
			if(errno == EINTR) /* Interrupted by sig handler return */
				nwritten = 0; /* and call write() again() */
			else
				return -1; /* errno set by write() */
		}
		nleft -= nwritten;
		bufp += nwritten;
	}
	return n;
}

};