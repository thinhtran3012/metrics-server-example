#ifndef _METRIC_COLLECT_MEM_STATS_H_
#define _METRIC_COLLECT_MEM_STATS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ngx_util.h"
#include "global.h"

#define MEM_INFO_LEN 1024 * 5
#define LINE_MAX_LEN 256
#define MEM_INFO_FILE "/opt/rootfs/proc/meminfo"
#define MEM_INTRO "#metric_meminfo\n"
#define MEM_INFO_LINE_FORMAT                                           \
    "# HELP node_memory_%s_bytes Memory information field %s_bytes.\n" \
    "# TYPE node_memory_%s_bytes gauge\n"                              \
    "node_memory_%s_bytes %ul\n"

u_char *ngx_http_get_mem_info(ngx_http_request_t *r, u_char *buf);
#endif /* _METRIC_COLLECT_MEM_STATS_H_ */