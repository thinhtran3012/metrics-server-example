#ifndef _NGX_METRIC_COLLECT_VM_STATS_H_
#define _NGX_METRIC_COLLECT_VM_STATS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ngx_util.h"
#include "global.h"

#define VM_STATS_FILE "/opt/rootfs/proc/vmstat"
#define VM_STATS_LEN 1024 * 5
#define VM_STATS_LINE_MAX_LEN 256
#define VM_INTRO "# metric_vmstats\n"
#define VM_STATS_LINE_FORMAT                                     \
    "# HELP node_vmstat_%s /proc/vmstat information field %s.\n" \
    "# TYPE node_vmstat_%s untyped\n"                            \
    "node_vmstat_%s %ul\n"

u_char *ngx_http_get_vm_stats(ngx_http_request_t *r, u_char *buf);

#endif /*_NGX_METRIC_COLLECT_VM_STATS_H_*/
