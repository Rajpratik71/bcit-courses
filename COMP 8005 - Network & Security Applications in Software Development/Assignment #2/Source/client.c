/*-----------------------------------------------------------------------------
 * client.c
 * Copyright (C) 2010 Steffen L. Norgren <ironix@trollop.org>
 * 
 * client.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * client.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *----------------------------------------------------------------------------*/

#include "client.h"

void client_init(void)
{
	struct event timeout;
	struct timeval tv;
	int i, socket_fd, retry = 0;
	char c = MIN_CHAR;
	
	/* Initialize libevent. */
	event_init();
	
	/* allocate space for the test string */
	if ((test_vars.string_test = malloc(sizeof(char) * test_vars.string_length + 1)) == NULL)
		err(1, "malloc");
	
	/* create test string */
	for (i = 0; i < test_vars.string_length; i++) {
		if (c > MAX_CHAR)
			c = MIN_CHAR;
		else
			c++;
		
		test_vars.string_test[i] = c;
	}
	
	/* set currently open connections to 0 */
	test_vars.open_conns = 0;
	
	/* create as many sockets as needed */
	for (i = 0; i < test_vars.conns; i++) {
		socket_fd = get_socket(i);
		
		if (socket_fd == ERROR) {
			if (retry < MAX_CONNECT_ERRORS) {
				retry++;
				i--;
				usleep(1000);
			}
			else
				err(1, "get_socket");
		}
		else {
			/* setup write events */
			event_set(&srv_stats[i].ev_write, srv_stats[i].fd_server,
					  EV_WRITE|EV_PERSIST, on_write, &srv_stats[i]);
			event_add(&srv_stats[i].ev_write, NULL);
			
			/* setup read events */
			event_set(&srv_stats[i].ev_read, srv_stats[i].fd_server,
					  EV_READ|EV_PERSIST, on_read, &srv_stats[i]);
			event_add(&srv_stats[i].ev_read, NULL);
			
			test_vars.open_conns++;
			retry = 0;
		}

		usleep(1);
	}
	
	/* create a timer event for writing stats to disk */
	evtimer_set(&timeout, event_timer, &timeout);
	evutil_timerclear(&tv);
	tv.tv_usec = 0;
	event_add(&timeout, &tv);
	
	/* Start the libevent event loop. */
	event_dispatch();
	
	write_stats();
}


int get_socket(int connection)
{
	struct sockaddr_in addr;
    struct hostent *he;
	int sock_buf_size;
	
	/* create the socket */
    if ((srv_stats[connection].fd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err(1, "socket");
	
    bzero((char *)&addr, sizeof(addr));
	
	/* resolve the hostname */
    if ((he = gethostbyname(test_vars.server)) == NULL)
        err(1, "gethostbyname");

    bcopy(he->h_addr, &addr.sin_addr, he->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(test_vars.port);
		
	/* connect! */
    if (connect(srv_stats[connection].fd_server,
				(struct sockaddr *) &addr, sizeof(addr)) < 0) {
		warn("connect");
		return ERROR;
	}

	/* insert initial values for this connection */
	srv_stats[connection].time = time_usec();
	srv_stats[connection].local_port = get_local_Port(srv_stats[connection].fd_server);
	srv_stats[connection].requests = 0;
	srv_stats[connection].sent_data = 0;
	srv_stats[connection].recv_data = 0;
	srv_stats[connection].delay = 0;
	
	/* increase the buffer size */
	sock_buf_size = MAX_IOSIZE * 2;
	setsockopt(srv_stats[connection].fd_server, SOL_SOCKET, SO_SNDBUF,
			   &sock_buf_size, sizeof(sock_buf_size));
	setsockopt(srv_stats[connection].fd_server, SOL_SOCKET, SO_RCVBUF,
			   &sock_buf_size, sizeof(sock_buf_size));	

    return srv_stats[connection].fd_server;
}


int get_local_Port(int socket_fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	
	/* get the local port */
	if (getsockname(socket_fd, (struct sockaddr *)&addr, &addr_len) == ERROR)
		err(1, "getsockname");
	
	return addr.sin_port;
}


unsigned long time_usec(void)
{
	/* for time in µsec */
	struct timeval tv;
	
	if (gettimeofday(&tv, NULL) == ERROR)
		err(1, "gettimeofday");
	
	return tv.tv_usec + (tv.tv_sec * 1000000L);
}

void on_write(int fd, short ev, void *arg)
{
	ServerStats *server = (ServerStats *)arg;
	int write_len;
	
	write_len = write(server->fd_server, test_vars.string_test,
					  test_vars.string_length);
	
	/* update statistics */
	server->time = time_usec();
	server->requests++;
	server->sent_data += write_len;

	/* close the socket and delete the event once all data sent */
	if (server->requests == test_vars.repeat) {
		event_del(&server->ev_write);
	}
}

void on_read(int fd, short ev, void *arg)
{
	ServerStats *server = (ServerStats *)arg;
	u_char buffer[MAX_IOSIZE];
	int read_len;
	
	read_len = read(server->fd_server, buffer, sizeof(buffer));
	
	/* update statistic */
	server->delay += time_usec() - server->time;
	server->recv_data += read_len;
	
	/* close the socket and delete the event once all data received */
	if (server->recv_data == (test_vars.repeat * test_vars.string_length)) {
		event_del(&server->ev_read);
		close(fd);

		/* exit the event loop when we have no more connections */
		if (--test_vars.open_conns == 0)
			event_loopexit(0);
		
		return;	
	}
}

/*-----------------------------------------------------------------------------
 * FUNCTION:    set_nonblocking
 * 
 * DATE:        March 11, 2010
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Steffen L. Norgren <ironix@trollop.org>
 * 
 * PROGRAMMER:  Steffen L. Norgren <ironix@trollop.org>
 * 
 * INTERFACE:   int set_nonblocking(int fd)
 *                  fd - the socket to set to non-blocking
 * 
 * RETURNS: error or success code
 * 
 * NOTES: Sets the socket to non-blocking mode.
 *
 *----------------------------------------------------------------------------*/
int set_nonblocking(int socket_fd)
{
	int flags;
	
	flags = fcntl(socket_fd, F_GETFL);
	if (flags < 0)
		return flags;
	
	flags |= O_NONBLOCK;
	if (fcntl(socket_fd, F_SETFL, flags) < 0)
		return ERROR;
	
	return ERROR_NONE;
}

/*-----------------------------------------------------------------------------
 * FUNCTION:    print_usage
 * 
 * DATE:        March 5, 2010
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Steffen L. Norgren <ironix@trollop.org>
 * 
 * PROGRAMMER:  Steffen L. Norgren <ironix@trollop.org>
 * 
 * INTERFACE:   void print_usage(char *command, int err)
 *                   command - the command line option used
 *                   err - error produced
 * 
 * RETURNS: void
 * 
 * NOTES: Prints out the program's useage information if the user incorrectly
 *        entered an option or used the -h or --help option.
 *
 *----------------------------------------------------------------------------*/
void print_usage(char *command, int err)
{
    if (err == ERROR_NONE) {
        printf("usage: client [arguments]\n\n");
        printf("Arguments:\n");
        printf("  -r  or  --repeat  Number of times to repeat test\n");
        printf("  -c  or  --conn    Number of simultaneous connections\n");
        printf("  -l  or  --length  Lengh of string to send\n");
        printf("  -s  or  --server  Sever to connect with\n");
        printf("  -p  or  --port    Port to connect on\n");
        printf("  -f  or  --file    Where to save statistics\n");
        printf("  -h  or  --help    Prints out this screen\n");
    }
	else if (err == ERROR_OPTS)
        printf("Try `client --help` for more information.\n");
	else {
        printf("%s: unknown error\n", command);
        printf("Try `client --help` for more information.\n");
    }
    
    exit(err);
}

/*-----------------------------------------------------------------------------
 * FUNCTION:    print_settings
 * 
 * DATE:        March 5, 2010
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Steffen L. Norgren <ironix@trollop.org>
 * 
 * PROGRAMMER:  Steffen L. Norgren <ironix@trollop.org>
 * 
 * INTERFACE:   void print_settings(int argc, PRIME_OPTIONS *opts)
 *                   argc - argument count
 * 
 * RETURNS: void
 * 
 * NOTES: Prints out the current settings that are being used in the current
 *        run of the application.
 *
 *----------------------------------------------------------------------------*/
void print_settings(int argc)
{
	printf("  Number of times to repeat:   %d\n", test_vars.repeat);
	printf("  Number of connections:       %d\n", test_vars.conns);
	printf("  Lengh of string to send:     %d\n", test_vars.string_length);
	printf("  Sever to connect with:       %s\n", test_vars.server);
	printf("  Port to connect on:          %d\n", test_vars.port);
	printf("  Where to save statistics:    %s\n\n", test_vars.output_file);
}

static void event_timer(int fd, short event, void *arg)
{
	struct timeval tv;
	struct event *timeout = arg;
	
	/* write the data */
	write_stats();
	
	evutil_timerclear(&tv);
	tv.tv_usec = EVENT_TIMER;
	event_add(timeout, &tv);
}

/*-----------------------------------------------------------------------------
 * FUNCTION:    write_stats
 * 
 * DATE:        March 11, 2010
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Steffen L. Norgren <ironix@trollop.org>
 * 
 * PROGRAMMER:  Steffen L. Norgren <ironix@trollop.org>
 * 
 * INTERFACE:   void write_stats(void)
 * 
 * RETURNS: void
 * 
 * NOTES: Writes the contents of the ServerStats struct to a file.
 *
 *----------------------------------------------------------------------------*/
int write_stats(void)
{
	FILE *file;
	int i, fdes;
	struct stat fileStat;
	
	/* write data to file */
	file = fopen(test_vars.output_file, "a+");
	if (file == NULL)
		err(1, "fopen");
	
	/* grab the size of the file we're writing to */
	fdes = open(test_vars.output_file, O_RDONLY);
	if(fstat(fdes, &fileStat) < 0)    
        err(1, "fstat");
	close(fdes);
	
	/* write a header to the file if it doesn't have one */
	if (fileStat.st_size == 0)
		fprintf(file, "time, local_port, requests, sent_data, delay\n");

	
	/* write statistics to the file */
	for (i = 0; i < test_vars.conns; i++)
		fprintf(file, "%lu, %d, %d, %lu, %d\n", srv_stats[i].time, srv_stats[i].local_port,
				srv_stats[i].requests, srv_stats[i].sent_data, srv_stats[i].delay);
	
	fclose(file);
	
	return ERROR_NONE;
}

/*-----------------------------------------------------------------------------
 * FUNCTION:    main
 * 
 * DATE:        March 6, 2010
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Steffen L. Norgren <ironix@trollop.org>
 * 
 * PROGRAMMER:  Steffen L. Norgren <ironix@trollop.org>
 * 
 * INTERFACE:   int main(int argc, char **argv)
 *                  argc - argument count
 *                  argv - array of arguments
 * 
 * RETURNS: Result on success or failure.
 * 
 * NOTES: Main entry point into the application. Checks command-line options
 *        and sets appropriate defaults.
 *
 *----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	int c, option_index = 0;
	
    static struct option long_options[] =
    {
        {"repeat"	, required_argument, 0, 'r'},
        {"conn"		, required_argument, 0, 'c'},
        {"length"	, required_argument, 0, 'l'},
        {"server"	, required_argument, 0, 's'},
        {"port"		, required_argument, 0, 'p'},
        {"file"		, required_argument, 0, 'f'},
        {"help"		, no_argument      , 0, 'h'},
        {0, 0, 0, 0}
    };
	
	/* Set Defaults */
	test_vars.repeat		= TV_REPEAT;
	test_vars.conns			= TV_CONNS;
	test_vars.string_length	= TV_LENGTH;
	test_vars.server		= TV_SERVER;
	test_vars.port			= TV_PORT;
	test_vars.output_file	= TV_FILE;
	
	while (1) {
		c = getopt_long(argc, argv, "r:c:l:s:p:f:h", long_options, &option_index);
		
		if (c == -1)
			break;
		
		switch (c) {
			case 0:
				/* If the option set a flag, do nothing */
				if (long_options[option_index].flag != 0)
					break;
				break;
				
			case 'r':
				if (atoi(optarg) > 0)
					test_vars.repeat = atoi(optarg);
				break;
				
			case 'c':
				if (atoi(optarg) > MAX_CONNS)
					test_vars.conns = MAX_CONNS;
				else if (atoi(optarg) > 0)
					test_vars.conns = atoi(optarg);
				break;

			case 'l':
				if (atoi(optarg) > MAX_IOSIZE)
					test_vars.string_length = MAX_IOSIZE;
				else if (atoi(optarg) > 0)
					test_vars.string_length = atoi(optarg);
				break;
				
			case 's':
				test_vars.server = optarg;
				break;
				
			case 'p':
				if (atoi(optarg) > 0)
					test_vars.port = atoi(optarg);
				break;
				
			case 'f':
				test_vars.output_file = optarg;
				break;
				
			case 'h':
				print_usage(argv[0], ERROR_NONE);
				break;

			default:
				print_usage(argv[0], ERROR_OPTS);
				break;
		}
	}
	
	/* print current settings */
	print_settings(argc);
	
	/* initialise the client */
	client_init();
	
	return ERROR_NONE;
}
