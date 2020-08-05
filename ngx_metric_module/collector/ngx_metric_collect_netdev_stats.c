#include "ngx_metric_collect_netdev_stats.h"

u_char *ngx_http_get_netdev_stats(ngx_http_request_t *r, u_char *buf)
{
    char *content = read_proc_file(NETDEV_STATS_FILE, r);
	if (content != NULL)
	{
        node_array_t tokens = str_split(content, '\n');
        int lengFor = (int)tokens.len;
        if (lengFor > 2)
		{
			char** data = tokens.data;
            int fields;
            char* ifname = (char*)calloc(64, sizeof(char));
            size_t r_bytes, r_packets, r_errs, r_drop, r_fifo, r_frame, r_compressed, r_multicast;
            size_t t_bytes, t_packets, t_errs, t_drop, t_fifo, t_colls, t_carrier, t_compressed;
            //ignore 2 first lines
            int i;
			for (i = 2; i < lengFor; i++)
			{
				char* line = *(data+ i);
				fields = sscanf(line, NETDEV_READ_FORMAT, ifname, 
                        &r_bytes, &r_packets, &r_errs, &r_drop, &r_fifo, &r_frame, &r_compressed, &r_multicast, 
                        &t_bytes, &t_packets, &t_errs, &t_drop, &t_fifo, &t_colls, &t_carrier, &t_compressed);
                if(strlen(ifname) > 2)
                {
                     ifname[strlen(ifname) - 1] = '\0';
                }
                if (fields == 17)
                {
                    buf = ngx_sprintf(buf, NETDEV_STATS_FMT, ifname, r_bytes, ifname, r_packets,
                            ifname, r_errs, ifname, r_drop, ifname, r_fifo,
                            ifname, r_frame, ifname, r_compressed, ifname, r_multicast,
                            ifname, t_bytes, ifname, t_packets, ifname, t_errs,
                            ifname, t_drop, ifname, t_fifo, ifname, t_colls,
                            ifname, t_carrier, ifname, t_compressed);
                }

                if (line)
                {
                    free(line);
                }
			}
            if (ifname)
            {
                free(ifname);
            }
            if (data)
            {
                free(data);
            }
		}
		free(content);
    }
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "NETDEV STATS DONE!!!");
    return buf;
}