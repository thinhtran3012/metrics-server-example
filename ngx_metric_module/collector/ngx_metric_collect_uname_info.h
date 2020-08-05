#ifndef _NGX_METRIC_COLLECT_UNAME_INFO_H_
#define _NGX_METRIC_COLLECT_UNAME_INFO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ngx_util.h"
#include "global.h"
#include <sys/utsname.h>

#define METRIC_UNAME_INFO_LINE_FORMAT                                                           \
    "# HELP node_uname_info Labeled system information as provided by the uname system call.\n" \
    "# TYPE node_uname_info gauge\n"                                                            \
    "node_uname_info{domainname=\"%s\",machine=\"%s\",nodename=\"%s\",release=\"%s\",sysname=\"%s\",version=\"%s\"} 1\n"
#define UNAME_STATS_LINE_MAX_LEN 512

u_char *ngx_http_get_uname_info(ngx_http_request_t *r, u_char *buf);

#endif /*_NGX_METRIC_COLLECT_UNAME_INFO_H_*/