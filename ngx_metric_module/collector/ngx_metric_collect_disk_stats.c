#include "ngx_metric_collect_disk_stats.h"

int is_device_name_not_contain_number(char str[])
{
    int index, len;
    len = strlen(str);
    for (index = 0; index < len; index++)
    {
        if (str[index] >= '0' && str[index] <= '9')
            return 0;
    }
    return 1;   
}

int is_disk_device(char *deviceName)
{
    if (start_with(deviceName, "sd") ||
        start_with(deviceName, "hd") ||
        start_with(deviceName, "vd") ||
        start_with(deviceName, "xvd"))
        return 1;
    return 0;
}

u_char *ngx_http_get_disk_stats(ngx_http_request_t *r, u_char *buf)
{
    char dev_name[10];
    int fields;
    unsigned long ios_pgr, tot_ticks, rq_ticks;
    unsigned long rd_ios, rd_merges, rd_sec, rd_ticks;
    unsigned long wr_ios, wr_merges, wr_sec, wr_ticks;
    unsigned long major, minor;

    char *content = read_proc_file(DISK_STATS_FILE, READ_ONLY_MODE);

    if (content != NULL)
    {
        node_array_t tokens = str_split(content, '\n');
        int lengFor = (int)tokens.len;
        if (lengFor > 0)
        {
            char **data = tokens.data;
            int i;
            for (i = 0; i < lengFor; i++)
            {
                char *line = *(data + i);
                /* major minor name rio rmerge rsect ruse wio wmerge wsect wuse running use aveq dcio dcmerge dcsect dcuse*/
                fields = sscanf(line, "%lu %lu %s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                                &major, &minor, dev_name,
                                &rd_ios, &rd_merges, &rd_sec, &rd_ticks,
                                &wr_ios, &wr_merges, &wr_sec, &wr_ticks,
                                &ios_pgr, &tot_ticks, &rq_ticks);
                if (fields == 14)
                {
                    if (is_disk_device(dev_name) && is_device_name_not_contain_number(dev_name))
                    {

                        //The total number of reads completed successfully.
                        //The total number of reads merged.
                        //The total number of bytes read successfully.
                        //The total number of seconds spent by all reads.
                        buf = ngx_sprintf(buf, METRIC_DISK_READ_FMT,
                                          dev_name, rd_ios,
                                          dev_name, rd_merges,
                                          dev_name, rd_sec,
                                          dev_name, rd_ticks);

                        //The total number of writes completed successfully.
                        //The total number of writes merged.
                        //The total number of bytes write successfully.
                        //The total number of seconds spent by all writes.
                        buf = ngx_sprintf(buf, METRIC_DISK_WRITE_FMT,
                                          dev_name, wr_ios,
                                          dev_name, wr_merges,
                                          dev_name, wr_sec,
                                          dev_name, wr_ticks);

                        //The number of I/Os currently in progress.
                        //Total seconds spent doing I/Os.
                        //The weighted # of seconds spent doing I/Os.
                        buf = ngx_sprintf(buf, METRIC_DISK_IO_FMT,
                                          dev_name, ios_pgr,
                                          dev_name, tot_ticks,
                                          dev_name, rq_ticks);
                    }
                }
                if (line)
                {
                    free(line);
                }
            }
            if (data)
            {
                free(data);
            }
        }
        free(content);
    }
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "DISK STATS DONE!!!");
    return buf;
}