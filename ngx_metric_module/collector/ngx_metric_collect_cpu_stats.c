#include "ngx_metric_collect_cpu_stats.h"

/* check data is cpu's data?*/
int is_cpu_data(const char *str, const char *pre)
{
	return start_with(str, pre);
}

/* split cpu number*/
void split_cpu_name(char *cpuName)
{
	remove_all_characters(cpuName);
}

/* convert val from hundredth seconds to seconds*/
double to_seconds(double val)
{
	return val/100.0;
}

u_char *parse_cpu_stats_to_prometheus(u_char *buf, char *data)
{
    int fields;
    char cpuName[5];
    double userVal, niceVal, systemVal, idleVal, iowaitVal, irqVal, softirqVal;
    fields = sscanf(data, CPU_DATA_LINE_FORMAT,
                    cpuName, &userVal, &niceVal, &systemVal, &idleVal, &iowaitVal, &irqVal, &softirqVal);
    split_cpu_name(cpuName);
    // skip cpu, only get cpu0, cpu1, cpu2,....
    if (strcmp(cpuName, ""))
    {
        buf = ngx_sprintf(buf, CPU_PROM_FMT,
                          cpuName, to_seconds(userVal),
                          cpuName, to_seconds(niceVal),
                          cpuName, to_seconds(systemVal),
                          cpuName, to_seconds(idleVal),
                          cpuName, to_seconds(iowaitVal),
                          cpuName, to_seconds(irqVal),
                          cpuName, to_seconds(softirqVal));
    }
    return buf;
}

u_char *ngx_http_get_cpu_stats(ngx_http_request_t *r, u_char *buf)
{
    unsigned int number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
    buf = ngx_sprintf(buf, CPU_NUMBER, number_of_processors);
    //
    char* content = read_proc_file(CPU_STATS_FILE, r);
    if(content != NULL)
    {
        buf = ngx_sprintf(buf, CPU_INTRO);
        node_array_t tokens = str_split(content, '\n');
        int lengFor = (int)tokens.len;
        if (lengFor > 0)
		{
            char** data = tokens.data;
            int i;
			for (i = 0; i < lengFor; i++)
			{
                char* line = *(data+ i);
                if (is_cpu_data(line, CPU_START_WITH))
                {
                    buf = parse_cpu_stats_to_prometheus(buf, line);
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
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "CPU STATS DONE!!!");
    return buf;
}