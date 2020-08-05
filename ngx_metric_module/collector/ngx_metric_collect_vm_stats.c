#include "ngx_metric_collect_vm_stats.h"

int match_vm(char *line)
{
    // (oom_kill|pgpg|pswp|pg.*fault)
    return (start_with(line, "pg") && end_with(line, "fault")) || start_with(line, "pswp") || start_with(line, "pgpg") || (strcmp(line, "oom_kill") == 0);
}

u_char *ngx_http_get_vm_stats(ngx_http_request_t *r, u_char *buf)
{
    char *content = read_proc_file(VM_STATS_FILE, r);
    if (content != NULL)
    {
        node_array_t tokens = str_split(content, '\n');
        int lengFor = (int)tokens.len;
        if (lengFor > 0)
        {
            char **data = tokens.data;
            int i;
            char *key = (char *)calloc(64, sizeof(char));
            for (i = 0; i < lengFor; i++)
            {
                char *line = *(data + i);
                unsigned long value;
                sscanf(line, "%s %lu", key, &value);
                if (match_vm(key))
                {
                    buf = ngx_sprintf(buf, VM_STATS_LINE_FORMAT, key, key, key, key, value);
                }
                if (line)
                {
                    free(line);
                }
            }
            if (key)
            {
                free(key);
            }
            if (data)
            {
                free(data);
            }
        }
        free(content);
    }
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "VM STATS DONE!!!");
    return buf;
}