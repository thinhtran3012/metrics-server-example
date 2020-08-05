#ifndef _NGX_METRIC_COLLECT_NETDEV_STATS_H
#define _NGX_METRIC_COLLECT_NETDEV_STATS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ngx_util.h"
#include "global.h"

#define NETDEV_STATS_LINE_MAX_LEN 256
#define NETDEV_STATS_LEN 1024 * 10
#define NETDEV_STATS_INTRO "#netdev_stats_metric\n"
#define NETDEV_STATS_FILE "/opt/rootfs/proc/net/dev"

#define NETDEV_READ_FORMAT "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu"

// Description
#define NETDEV_STATS_FMT                                                                            \
    "# HELP node_network_receive_bytes_total Network device statistic receive_bytes.\n"             \
    "# TYPE node_network_receive_bytes_total counter\n"                                             \
    "node_network_receive_bytes_total{device=\"%s\"} %uz\n"                                         \
    "# HELP node_network_receive_packets_total Network device statistic receive_packets.\n"         \
    "# TYPE node_network_receive_packets_total counter\n"                                           \
    "node_network_receive_packets_total{device=\"%s\"} %uz\n"                                       \
    "# HELP node_network_receive_errs_total Network device statistic receive_errs.\n"               \
    "# TYPE node_network_receive_errs_total counter\n"                                              \
    "node_network_receive_errs_total{device=\"%s\"} %uz\n"                                          \
    "# HELP node_network_receive_drop_total Network device statistic receive_drop.\n"               \
    "# TYPE node_network_receive_drop_total counter\n"                                              \
    "node_network_receive_drop_total{device=\"%s\"} %uz\n"                                          \
    "# HELP node_network_receive_fifo_total Network device statistic receive_fifo.\n"               \
    "# TYPE node_network_receive_fifo_total counter\n"                                              \
    "node_network_receive_fifo_total{device=\"%s\"} %uz\n"                                          \
    "# HELP node_network_receive_frame_total Network device statistic receive_frame.\n"             \
    "# TYPE node_network_receive_frame_total counter\n"                                             \
    "node_network_receive_frame_total{device=\"%s\"} %uz\n"                                         \
    "# HELP node_network_receive_compressed_total Network device statistic receive_compressed.\n"   \
    "# TYPE node_network_receive_compressed_total counter\n"                                        \
    "node_network_receive_compressed_total{device=\"%s\"} %uz\n"                                    \
    "# HELP node_network_receive_multicast_total Network device statistic receive_multicast.\n"     \
    "# TYPE node_network_receive_multicast_total counter\n"                                         \
    "node_network_receive_multicast_total{device=\"%s\"} %uz\n"                                     \
    "# HELP node_network_transmit_bytes_total Network device statistic transmit_bytes.\n"           \
    "# TYPE node_network_transmit_bytes_total counter\n"                                            \
    "node_network_transmit_bytes_total{device=\"%s\"} %uz\n"                                        \
    "# HELP node_network_transmit_packets_total Network device statistic transmit_packets.\n"       \
    "# TYPE node_network_transmit_packets_total counter\n"                                          \
    "node_network_transmit_packets_total{device=\"%s\"} %uz\n"                                      \
    "# HELP node_network_transmit_errs_total Network device statistic transmit_errs.\n"             \
    "# TYPE node_network_transmit_errs_total counter\n"                                             \
    "node_network_transmit_errs_total{device=\"%s\"} %uz\n"                                         \
    "# HELP node_network_transmit_drop_total Network device statistic transmit_drop.\n"             \
    "# TYPE node_network_transmit_drop_total counter\n"                                             \
    "node_network_transmit_drop_total{device=\"%s\"} %uz\n"                                         \
    "# HELP node_network_transmit_fifo_total Network device statistic transmit_fifo.\n"             \
    "# TYPE node_network_transmit_fifo_total counter\n"                                             \
    "node_network_transmit_fifo_total{device=\"%s\"} %uz\n"                                         \
    "# HELP node_network_transmit_colls_total Network device statistic transmit_colls.\n"           \
    "# TYPE node_network_transmit_colls_total counter\n"                                            \
    "node_network_transmit_colls_total{device=\"%s\"} %uz\n"                                        \
    "# HELP node_network_transmit_carrier_total Network device statistic transmit_carrier.\n"       \
    "# TYPE node_network_transmit_carrier_total counter\n"                                          \
    "node_network_transmit_carrier_total{device=\"%s\"} %uz\n"                                      \
    "# HELP node_network_transmit_compressed_total Network device statistic transmit_compressed.\n" \
    "# TYPE node_network_transmit_compressed_total counter\n"                                       \
    "node_network_transmit_compressed_total{device=\"%s\"} %uz\n"

u_char *ngx_http_get_netdev_stats(ngx_http_request_t *r, u_char *buf);

#endif /* _NGX_METRIC_COLLECT_NETDEV_STATS_H */