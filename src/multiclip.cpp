#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h> 

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


void my_sleep(unsigned msec) 
{
    struct timespec req, rem;
    int err;
    req.tv_sec = msec / 1000;
    req.tv_nsec = (msec % 1000) * 1000000;
    while ((req.tv_sec != 0) || (req.tv_nsec != 0)) {
        if (nanosleep(&req, &rem) == 0)
            break;
        err = errno;
        // Interrupted; continue
        if (err == EINTR) {
            req.tv_sec = rem.tv_sec;
            req.tv_nsec = rem.tv_nsec;
        }
        // Unhandleable error (EFAULT (bad pointer), EINVAL (bad timeval in tv_nsec), or ENOSYS (function not supported))
        break;
    }
}

void findip( char * line, char * ip )
{
    char *ss = strstr(line, "inet ");
    char *se = strstr(line, "/");
    if( ss != NULL && se != NULL )
    {
        int nsz = se - ss -5;
        if( nsz > 0 )
        {
            ip = strncpy(ip, ss+5, nsz);
            printf("ip=%s\n", ip);
        }
    }
}

void getiparr(char * ip1,char * ip2,char *ip3)
{
    FILE *fp;
    char bufline[1035];
    bzero( bufline, sizeof(bufline ) );

    /* Open the command for reading. */
    fp = popen("ip addr | grep 'inet ' | grep global", "r");
    if (fp == NULL) {
      printf("Failed to run command\n" );
      return;
    }
    if( fgets(bufline, sizeof(bufline)-1, fp) != NULL )
    {
       findip( bufline, ip1 );  
    }
    if( fgets(bufline, sizeof(bufline)-1, fp) != NULL )
    {
       findip( bufline, ip2 );  
    }
    if( fgets(bufline, sizeof(bufline)-1, fp) != NULL )
    {
       findip( bufline, ip3 );  
    }    
//    /* Read the output a line at a time - output it. */
//    while (fgets(path, sizeof(path)-1, fp) != NULL) {
//      printf("%s", path);
//    }

    /* close */
    pclose(fp);   
}
//======================================

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[1024] = "Multicast test message lol122!";
int datalen = sizeof(databuf);
 
int main (int argc, char *argv[ ])
{
    //return 0;
    
    /* Create a datagram socket on which to send. */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
    {
      perror("Opening datagram socket error");
      exit(1);
    }
    else
      printf("Opening the datagram socket...OK.\n");

    /* Initialize the group sockaddr structure with a */
    /* group address of 225.1.1.1 and port 5555. */
    bzero( &groupSock, sizeof( groupSock ) ) ;
    //memset((char *) &groupSock, 0, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr("226.0.0.1");
    groupSock.sin_port = htons(4321);
 
    // Disable loopback so you do not receive your own datagrams.
//    {
//        char loopch = 0;
//        if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
//        {
//            perror("Setting IP_MULTICAST_LOOP error");
//            close(sd);
//            exit(1);
//        }
//        else{
//            printf("Disabling the loopback...OK.\n");
//        } 
//    }



    /* Set local interface for outbound multicast datagrams. */
    /* The IP address specified must be associated with a local, */
    /* multicast capable interface. */
    int flagk= 0;
    while( 1 )
    {
        char ip1[20],ip2[20],ip3[20];
        bzero( ip1, sizeof(ip1));
        bzero( ip2, sizeof(ip2));
        bzero( ip3, sizeof(ip3));
        getiparr(ip1,ip2,ip3);
        
        bzero( databuf, sizeof(databuf));
        if( flagk == 0 )
        {
            flagk = 1 ;
            if( strlen( ip1 ) == 0 ) continue;
            localInterface.s_addr = inet_addr( ip1 );
            strcpy( databuf, ip1);
        }
        else if(flagk == 1)
        {
            flagk = 2 ;
            if( strlen( ip2 ) == 0 ) continue;
            localInterface.s_addr = inet_addr(ip2);
            strcpy( databuf, ip2);
        }
        else
        {
            flagk = 0;
            if( strlen( ip3 ) == 0 ) continue;
            localInterface.s_addr = inet_addr(ip3);
            strcpy( databuf, ip3);
        }
        datalen = strlen( databuf );
        if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
        {
            perror("Setting local interface error");
            exit(1);
        }
        else
        {
            printf("Setting the local interface...OK\n");
        }
        /* Send a message to the multicast group specified by the*/
        /* groupSock sockaddr structure. */
        /*int datalen = 1024;*/
        if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
        {
            perror("Sending datagram message error");
        }
        else
        {
            printf("Sending datagram message...OK\n");
        }

        /* Try the re-read from the socket if the loopback is not disable
        if(read(sd, databuf, datalen) < 0)
        {
            perror("Reading datagram message error\n");
            close(sd);
            exit(1);
        }
        else
        {
            printf("Reading datagram message from client...OK\n");
            printf("The message is: %s\n", databuf);
        }
        */
        my_sleep(1000);
    }
    return 0;
}


