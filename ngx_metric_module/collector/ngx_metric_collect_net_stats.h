#ifndef _NGX_METRIC_COLLECT_NET_STATS_H_
#define _NGX_METRIC_COLLECT_NET_STATS_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ngx_util.h"
#include "global.h"

#define NET_STAT_FILE "/opt/rootfs/proc/net/netstat"
#define SNMP_STAT_FILE "/opt/rootfs/proc/net/snmp"

#define TCP_EXT_FORMAT "# HELP node_netstat_TcpExt_%s Statistic TcpExt%s.\n" \
                       "# TYPE node_netstat_TcpExt_%s untyped\n"             \
                       "node_netstat_TcpExt_%s %ul\n"
#define IP_EXT_FORMAT "# HELP node_netstat_IpExt_%s Statistic IpExt%s.\n" \
                      "# TYPE node_netstat_IpExt_%s untyped\n"            \
                      "node_netstat_IpExt_%s %ul\n"

#define IP_FORMAT "# HELP node_netstat_Ip_%s Statistic Ip%s.\n" \
                  "# TYPE node_netstat_Ip_%s untyped\n"         \
                  "node_netstat_Ip_%s %ul\n"

#define ICMP_FORMAT "# HELP node_netstat_Icmp_%s Statistic Icmp%s.\n" \
                    "# TYPE node_netstat_Icmp_%s untyped\n"           \
                    "node_netstat_Icmp_%s %ul\n"

#define TCP_FORMAT "# HELP node_netstat_Tcp_%s Statistic Tcp%s.\n" \
                   "# TYPE node_netstat_Tcp_%s untyped\n"          \
                   "node_netstat_Tcp_%s %ul\n"

#define UDP_FORMAT "# HELP node_netstat_Udp_%s Statistic Udp%s.\n" \
                   "# TYPE node_netstat_Udp_%s untyped\n"          \
                   "node_netstat_Udp_%s %ul\n"

#define UDP_LITE_FORMAT "# HELP node_netstat_UdpLite_%s Statistic UdpLite%s.\n" \
                        "# TYPE node_netstat_UdpLite_%s untyped\n"              \
                        "node_netstat_UdpLite_%s %ul\n"
// read from file netstat for snmp

u_char *ngx_http_get_net_stats(ngx_http_request_t *r, u_char *buf);

#endif /* _NGX_METRIC_COLLECT_NET_STATS_H_ */