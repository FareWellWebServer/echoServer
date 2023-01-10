#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/event.h>
#include <fcntl.h>

typedef struct
{
	int fd;
	char *buf;
	size_t buflen;
} read_event_t;

// Event handler function for read events
void handle_read_event(int kq, struct kevent *event)
{
	read_event_t *re = (read_event_t *)event->udata;
	ssize_t nread = read(re->fd, re->buf, re->buflen);
	if (nread > 0)
	{
		printf("Read %ld bytes from fd %d\n", nread, re->fd);
	}
	else if (nread == 0)
	{
		printf("EOF on fd %d\n", re->fd);
		// Remove the event from the event loop
		EV_SET(event, re->fd, EVFILT_READ, EV_DELETE, 0, 0, re);
	}
	else
	{
		perror("read");
	}
}

int main(int argc, char **argv)
{
	int kq = kqueue();
	if (kq == -1)
	{
		perror("kqueue");
		return 1;
	}

	// Set up a read event for stdin
	read_event_t stdin_re = {.fd = STDIN_FILENO, .buf = (char *)malloc(1024), .buflen = 1024};
	struct kevent stdin_event;
	EV_SET(&stdin_event, STDIN_FILENO, EVFILT_READ, EV_ADD, 0, 0, &stdin_re);
	if (kevent(kq, &stdin_event, 1, NULL, 0, NULL) == -1)
	{
		perror("kevent");
		return 1;
	}

	// Set up a read event for a file
	read_event_t file_re = {.fd = open("/etc/passwd", O_RDONLY), .buf = (char *)malloc(1024), .buflen = 1024};
	struct kevent file_event;
	EV_SET(&file_event, file_re.fd, EVFILT_READ, EV_ADD, 0, 0, &file_re);
	if (kevent(kq, &file_event, 1, NULL, 0, NULL) == -1)
	{
		perror("kevent");
		return 1;
	}

	// Enter the event loop
	while (1)
	{
		struct kevent events[2];
		int nevents = kevent(kq, NULL, 0, events, 2, NULL);
		if (nevents == -1)
		{
			perror("kevent");
			return 1;
		}
		for (int i = 0; i < nevents; i++)
		{
			handle_read_event(kq, &events[i]);
		}
	}

	return 0;
}
