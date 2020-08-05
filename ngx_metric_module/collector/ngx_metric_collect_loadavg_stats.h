#ifndef _NGX_METRIC_COLLECT_LOAD_AVG_STATS_H
#define _NGX_METRIC_COLLECT_LOAD_AVG_STATS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include "ngx_util.h"

#define LOAD_AVG_STATS_LINE_MAX_LEN 256

#define LOAD_AVG_STATS_FMT                   \
    "# HELP node_load1 1m load average.\n"   \
    "# TYPE node_load1 gauge\n"              \
    "node_load1 %.2f\n"                      \
    "# HELP node_load15 15m load average.\n" \
    "# TYPE node_load15 gauge\n"             \
    "node_load5 %.2f\n"                      \
    "# HELP node_load5 5m load average.\n"   \
    "# TYPE node_load5 gauge\n"              \
    "node_load15 %.2f\n"

u_char *ngx_http_get_loadavg_stats(ngx_http_request_t *r, u_char *buf);
#endif /*_NGX_METRIC_COLLECT_LOAD_AVG_STATS_H*/