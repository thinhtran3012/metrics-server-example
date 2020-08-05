#include "ngx_metric_collect_live_session_stats.h"

int is_file_exist(char *file_path)
{
    struct stat sb;
    /* check if path exists and is a directory */
    return (stat(file_path, &sb) == 0);
}

static double jiffy = 0.0;

static unsigned long process_adjust_time(unsigned long t)
{
    if (jiffy == 0.0)
        jiffy = sysconf(_SC_CLK_TCK);
    double jiffytime = 1.0 / jiffy;
    return (unsigned long)t * jiffytime * 100;
}

unsigned long get_cpu_total_time()
{
    // get total_cpu_usage from /proc/stat
    unsigned long cpu_total_time = 0;
    char *content = read_proc_file("/opt/rootfs/proc/stat", READ_ONLY_MODE);
    if(content != NULL)
    {
        node_array_t tokens = str_split(content, '\n');
        int lengFor = (int)tokens.len;
        if (lengFor > 0)
		{
            char** data = tokens.data;
            int i;
			for (i = 0; i < lengFor; i++)
			{
                char *line = *(data + i);
                if (start_with(line, "cpu"))
                {
                    char cpuName[5];
                    unsigned long userVal, niceVal, systemVal, idleVal, iowaitVal, irqVal, softirqVal;
                    sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu",
                                cpuName, &userVal, &niceVal, &systemVal, &idleVal, &iowaitVal, &irqVal, &softirqVal);
                    if (!strcmp(cpuName, "cpu"))
                    {
                        cpu_total_time = userVal + niceVal + systemVal + idleVal + iowaitVal + irqVal + softirqVal;
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
    return cpu_total_time;
}

// unsigned long get_cpu_total_time_of_process(char *pid)
// {
//     unsigned long cpu_total_time_of_process = 0;
//     unsigned long utime_ticks;
//     long cutime_ticks;
//     unsigned long stime_ticks;
//     long cstime_ticks;
//     char stat_filepath[64];
//     sprintf(stat_filepath, "/opt/rootfs/proc/%s/task/%s/stat", pid, pid);
//     FILE *fpstat = fopen(stat_filepath, READ_ONLY_MODE);
//     if (fpstat)
//     {
//         fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu"
//                            "%lu %ld %ld %*d %*d %*d %*d %*u",
//                    &utime_ticks, &stime_ticks, &cutime_ticks, &cstime_ticks);
//         fclose(fpstat);
//     }
//     cpu_total_time_of_process = process_adjust_time(utime_ticks) + 
//                                 process_adjust_time(stime_ticks);

//     return cpu_total_time_of_process;
// }

unsigned long get_cpu_total_time_of_process(char *pid)
{
    unsigned long cpu_total_time_of_process = 0;
    unsigned long utime_ticks;      // CPU time spent in user code, measured in jiffies
    long cutime_ticks;              // CPU time spent in kernel code, measured in jiffies
    unsigned long stime_ticks;      // CPU time spent in user code, including time from children
    long cstime_ticks;              // CPU time spent in kernel code, including time from children 
    unsigned long long starttime;   // Time when the process started, measured in jiffies
    unsigned long Hertz;

    char stat_filepath[64];
    sprintf(stat_filepath, "/opt/rootfs/proc/%s/stat", pid);
    FILE *fpstat = fopen(stat_filepath, READ_ONLY_MODE);
    if (fpstat)
    {
        fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu"
                           "%lu %ld %ld %*d %*d %*d %*d %llu",
                   &utime_ticks, &stime_ticks, &cutime_ticks, &cstime_ticks, &starttime);
        fclose(fpstat);
    }
    cpu_total_time_of_process = utime_ticks + stime_ticks + cutime_ticks + cstime_ticks;
    return cpu_total_time_of_process;
}

char *get_cmdline(ngx_http_request_t *r, char *pid)
{
    const int BUFFER_SIZE = 16;       // should really get PAGESIZE or something instead...
    char path[64];
    sprintf(path, PID_CMDLINE_PATH, pid);

    if (is_file_exist(path))
    {
        FILE *fp = fopen(path, "rb");
        char buf[BUFFER_SIZE];
        char *cmd;
        size_t ret = 1;
        int len = 0;
        int i = 0;
        while ((ret = fread(buf, 1, BUFFER_SIZE, fp)) > 0)
        {
            len += ret;
            cmd = realloc(cmd, len);
            memcpy(cmd + len - ret, buf, ret);
        }
        for (i = 0; i < len - 1; i++)
        {
            if (cmd[i] == '\0')
            {
                cmd[i] = ' ';
            }
        }
        fclose(fp);
        return cmd;
    }

    return NULL;
}

char *pidof(ngx_http_request_t *r, const char *cmdline)
{
    FILE *fpidof = popen(cmdline, READ_ONLY_MODE);
    char *spid = (char *)calloc(256, sizeof(char));
    if (fpidof == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Can't find process running that have process name: /opt/uiza-ffmpeg/uiza-ffmpeg.");
        return NULL;
    }
    while (!feof(fpidof))
    {
        char *buffer = (char *)calloc(256, sizeof(char));
        if (fgets(buffer, 256, fpidof))
        {
            spid = (char *)realloc(spid, (strlen(buffer) + strlen(spid) + 1L) * sizeof(char));
            strcat(spid, buffer);
        }
        free(buffer);
    }
    pclose(fpidof);
    return spid;
}

char *parse_stream_key(ngx_http_request_t *r, char *line)
{
    char *stream_key = NULL;
    // Skip command -i rtmp://localhost/live/live_snghiw8UYti 2>&1
    if (strstr(line, "2>&1") != NULL)
        return stream_key;
    node_array_t cmd_params = str_split(line, ' ');
    int cmd_len = (int)cmd_params.len;
    if (cmd_len > 0)
    {
        char **cmd_data = cmd_params.data;
        int cmd_params_index;
        for (cmd_params_index = 0; cmd_params_index < cmd_len; cmd_params_index++)
        {
            char *cmd_param = *(cmd_data + cmd_params_index);
            // exp: rtmp://localhost/live/live_snghiw8UYti
            if (start_with(cmd_param, "rtmp://"))
            {
                node_array_t part_params = str_split(cmd_param, '/');
                int part_params_len = (int)part_params.len;
                if (part_params_len > 0)
                {
                    char **part_param_data = part_params.data;
                    int part_params_index;
                    for (part_params_index = 0; part_params_index < part_params_len; part_params_index++)
                    {
                        char *part_param = *(part_param_data + part_params_index);
                        if (start_with(part_param, "live_"))
                        {
                            stream_key = strdup(part_param);
                        }
                        if (part_param)
                        {
                            free(part_param);
                        }
                    }
                    if (part_param_data)
                    {
                        free(part_param_data);
                    }
                }
            }
            if (cmd_param)
            {
                free(cmd_param);
            }
        }
        if (cmd_data)
        {
            free(cmd_data);
        }
    }
    return stream_key;
}

char *get_stream_key(ngx_http_request_t *r, char *pid)
{
    char *stream_key = NULL;
    char *cmd_content = get_cmdline(r, pid);

    if (cmd_content !=  NULL)
    {
        stream_key = parse_stream_key(r, cmd_content);
    }
    free(cmd_content);
    return stream_key;
}

u_char *ngx_http_get_io_stat(ngx_http_request_t *r, u_char *buf, char *pid, char *stream_key)
{
    char path[64];
    sprintf(path, PID_IO_PATH, pid);
    if (is_file_exist(path))
    {
        char *content = read_proc_file(path, READ_ONLY_MODE);
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
                    if (sscanf(line, "%s %lu", key, &value) == 2)
                    {
                        if (start_with(key, "rchar"))
                        {
                            buf = ngx_sprintf(buf, LIVE_SESSION_IO_RCHAR_FORMAT, stream_key, value);
                        }
                        else if (start_with(key, "wchar"))
                        {
                            buf = ngx_sprintf(buf, LIVE_SESSION_IO_WCHAR_FORMAT, stream_key, value);
                        }
                        else if (start_with(key, "syscr"))
                        {
                            buf = ngx_sprintf(buf, LIVE_SESSION_IO_SYSCR_FORMAT, stream_key, value);
                        }
                        else if (start_with(key, "syscw"))
                        {
                            buf = ngx_sprintf(buf, LIVE_SESSION_IO_SYSCW_FORMAT, stream_key, value);
                        }
                        else if (start_with(key, "read_bytes"))
                        {
                            buf = ngx_sprintf(buf, LIVE_SESSION_IO_READ_FORMAT, stream_key, value);
                        }
                        else if (start_with(key, "write_bytes"))
                        {
                            buf = ngx_sprintf(buf, LIVE_SESSION_IO_WRITE_FORMAT, stream_key, value);
                        }
                        else if (start_with(key, "cancelled_write_bytes"))
                        {
                            buf = ngx_sprintf(buf, LIVE_SESSION_IO_CANCELLED_WRITE_FORMAT, stream_key, value);
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
                if (key)
                {
                    free(key);
                }
            }
            free(content);
        }
    }

    return buf;
}

// u_char *ngx_http_get_cpu_utilization(ngx_http_request_t *r, u_char *buf, char *pid, char *stream_key)
// {
//     float process_cpu_usage;
//     // sample for 1500 ms
//     struct timespec tm;
//     tm.tv_sec  = 1;
//     // tm.tv_nsec = 500;
//     tm.tv_nsec = 500 * 1000 * 1000; /* 500 ms */

//     unsigned int number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);

//     ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "number proc: %d", number_of_processors);
//     // get total_cpu_usage from /proc/stat
//     unsigned long cpu_total_time_last = get_cpu_total_time();
//     ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "cpu time last: %ul", cpu_total_time_last);
//     unsigned long cpu_total_time_of_process_last = get_cpu_total_time_of_process(pid);
//     ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "proc time last: %ul", cpu_total_time_of_process_last);

//     nanosleep(&tm, NULL);

//     unsigned long cpu_total_time_current = get_cpu_total_time();
//     ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "cpu time cur: %ul", cpu_total_time_current);
//     unsigned long cpu_total_time_of_process_current = get_cpu_total_time_of_process(pid);
//     ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "proc time cur: %ul", cpu_total_time_of_process_current);
//     process_cpu_usage = number_of_processors * 100 * 
//                     (cpu_total_time_of_process_current - cpu_total_time_of_process_last) /
//                     (float)(cpu_total_time_current - cpu_total_time_last);

//     buf = ngx_sprintf(buf, LIVE_SESSION_CPU_UTILIZATION_FORMAT, stream_key, process_cpu_usage);
//     ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "LIVE_SESSION_CPU_UTILIZATION_DONE!!!");
//     return buf;
// }

u_char *ngx_http_get_cpu_utilization(ngx_http_request_t *r, u_char *buf, char *pid, char *stream_key)
{
    double process_cpu_usage;
    unsigned long cpu_total_time_of_process;    // jiffies used by this process
    unsigned long utime_ticks;      // CPU time spent in user code, measured in jiffies
    long cutime_ticks;              // CPU time spent in kernel code, measured in jiffies
    unsigned long stime_ticks;      // CPU time spent in user code, including time from children
    long cstime_ticks;              // CPU time spent in kernel code, including time from children 
    unsigned long long starttime;   // Time when the process started, measured in jiffies
    unsigned long Hertz;
    double seconds_since_boot;
    unsigned long long seconds;     // seconds of process life

    char stat_filepath[64];
    sprintf(stat_filepath, "/opt/rootfs/proc/%s/stat", pid);
    if (is_file_exist(stat_filepath))
    {
        FILE *fpstat = fopen(stat_filepath, READ_ONLY_MODE);
        if (fpstat)
        {
            fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu"
                           "%lu %ld %ld %*d %*d %*d %*d %llu",
                   &utime_ticks, &stime_ticks, &cutime_ticks, &cstime_ticks, &starttime);
            fclose(fpstat);
        }
        cpu_total_time_of_process = utime_ticks + stime_ticks + cutime_ticks + cstime_ticks;
    }

    FILE *fpuptime = fopen("/opt/rootfs/proc/uptime", READ_ONLY_MODE);
    if (fpuptime)
    {
        fscanf(fpuptime, "%lf %*lf", &seconds_since_boot);
        fclose(fpuptime);
    }

    Hertz = sysconf(_SC_CLK_TCK);

    seconds = seconds_since_boot - starttime / Hertz;
    process_cpu_usage = (((double)cpu_total_time_of_process / Hertz) / seconds) * 100;
    buf = ngx_sprintf(buf, LIVE_SESSION_CPU_UTILIZATION_FORMAT, stream_key, process_cpu_usage);
    
    return buf;
}

u_char *ngx_http_get_memory_utilization(ngx_http_request_t *r, u_char *buf, char *pid, char *stream_key)
{
    unsigned long memory_total;
    unsigned int page_size = sysconf(_SC_PAGESIZE);
    unsigned long rss;
    float process_memory_usage = 0.00f;
    char statm_filepath[64];
    sprintf(statm_filepath, "/opt/rootfs/proc/%s/statm", pid);
    if (is_file_exist(statm_filepath))
    {
        FILE *fpstatm = fopen(statm_filepath, READ_ONLY_MODE);
        if (fpstatm)
        {
            fscanf(fpstatm, "%*lu %lu %*lu %*lu %*lu %*lu %*lu", &rss);
            fclose(fpstatm);
        }
    }

    FILE *fpmeminfo = fopen("/opt/rootfs/proc/meminfo", READ_ONLY_MODE);
    if (fpmeminfo)
    {
        char buf[256];
        while (fgets(buf, 256, fpmeminfo))
        {
            if (start_with(buf, "MemTotal"))
            {
                sscanf(buf, "%*s %lu", &memory_total);
                break;
            }
        }
        fclose(fpmeminfo);
    }

    if (memory_total) process_memory_usage = (float)(rss * page_size / 1024) / memory_total * 100;

    buf = ngx_sprintf(buf, LIVE_SESSION_MEMORY_UTILIZATION_FORMAT, stream_key, process_memory_usage);
    
    return buf;
}

u_char *ngx_http_get_live_session_stats(ngx_http_request_t *r, u_char *buf)
{
    // Search all process that running with process name: /opt/uiza-ffmpeg/uiza-ffmpeg
    char *spid = pidof(r, CMD_FIND_PID_UIZA_FFMPEG_PROC);
    if (spid == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Get pid of live session failed.");
        return buf;
    }
    node_array_t tokens = str_split(spid, '\n');
    free(spid);
    int lengFor = (int)tokens.len;
    if (lengFor > 0)
    {
        char **data = tokens.data;
        int i;
        for (i = 0; i < lengFor; i++)
        {
            char *pid = *(data + i);
            if (is_file_exist(pid))
            {
                remove_all_characters(pid);
                if (!strcmp(pid, ""))
                {
                    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Get PID failed.", pid);
                    return buf;
                }
                ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "Get PID: %s", pid);

                char *stream_key = get_stream_key(r, pid);
                ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "Get stream_key: %s", stream_key);
                if (stream_key == NULL)
                {
                    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Get stream_key failed.");
                    return buf;
                }

                buf = ngx_http_get_io_stat(r, buf, pid, stream_key);
                ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "LIVE_SESSION_MEMORY_STAT_DONE!!!");

                buf = ngx_http_get_cpu_utilization(r, buf, pid, stream_key);
                ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "LIVE_SESSION_CPU_UTILIZATION_DONE!!!");

                buf = ngx_http_get_memory_utilization(r, buf, pid, stream_key);
                ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "LIVE_SESSION_MEMORY_UTILIZATION_DONE!!!");

                free(stream_key);
            }
            free(pid);
        }
        if (data)
        {
            free(data);
        }
    }
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "LIVE SESSION STATS DONE!!!");
    return buf;
}