#include "ngx_metric_collect_uname_info.h"

u_char *ngx_http_get_uname_info(ngx_http_request_t *r, u_char *buf)
{
    struct utsname uts;
    if (uname(&uts) < 0)
    {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "uname() error");
        return buf;
    }
    buf = ngx_sprintf(buf, METRIC_UNAME_INFO_LINE_FORMAT, uts.__domainname, uts.machine,
                      uts.nodename, uts.release, uts.sysname, uts.version);
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "UNAME INFO DONE!!!");
    return buf;
}