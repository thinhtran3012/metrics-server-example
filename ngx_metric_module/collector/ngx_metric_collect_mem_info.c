#include "ngx_metric_collect_mem_info.h"

u_char *ngx_http_get_mem_info(ngx_http_request_t *r, u_char *buf)
{
    char *content = read_proc_file(MEM_INFO_FILE, r);
    if (content != NULL)
    {
        node_array_t tokens = str_split(content, '\n');
        int lengFor = (int)tokens.len;
        if (lengFor > 0)
        {
            char **data = tokens.data;
            int i;
            char *key = (char *)calloc(128, sizeof(char));
            for (i = 0; i < lengFor; i++)
            {
                char *line = *(data + i);
                unsigned long value;
                sscanf(line, "%s %lu", key, &value);
                // remove '(' and ')', which prometheus can't convert
                replace_char(key, '(', '_');
                remove_char(key, ')');
                remove_char(key, ':');
                // convert kbytes to bytes
                value *= 1024;
                buf = ngx_sprintf(buf, MEM_INFO_LINE_FORMAT, key, key, key, key, value);
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
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "MEM INFO DONE!!!\n");
    return buf;
}