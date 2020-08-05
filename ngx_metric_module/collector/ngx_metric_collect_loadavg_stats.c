#include "ngx_metric_collect_loadavg_stats.h"

u_char *ngx_http_get_loadavg_stats(ngx_http_request_t *r, u_char *buf)
{
    struct sysinfo si;
    if (sysinfo(&si) < 0)
    {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "Error: fail get sysinfo()");
        return buf;
    }
    // Why?
    float load1 = si.loads[0] / 65536.0;
    float load5 = si.loads[1] / 65536.0;
    float load15 = si.loads[2] / 65536.0;
    // 1m load average.
    // 5m load average.
    // 15m load average.
    buf = ngx_sprintf(buf, LOAD_AVG_STATS_FMT, load1, load5, load15);
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "LOADAVG STATS DONE!!!");
    return buf;
}