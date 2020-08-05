#ifndef _NGX_METRIC_COLLECT_CPU_STATS_H
#define _NGX_METRIC_COLLECT_CPU_STATS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ngx_util.h"
#include "global.h"

#define CPU_STATS_FILE "/opt/rootfs/proc/stat"
#define ENDLINE_FORMAT "\n"
#define CPU_INTRO   "# HELP node_cpu_seconds_total Seconds the cpus spent in each mode.\n" \
                    "# TYPE node_cpu_seconds_total counter\n"
#define CPU_NUMBER  "# HELP node_cpu_cores_total number of processors.\n" \
                    "# TYPE node_cpu_cores_total untyped\n" \
                    "node_cpu_cores_total %d\n"
#define CPU_START_WITH "cpu"
#define CPU_DATA_LINE_FORMAT "%s %lf %lf %lf %lf %lf %lf %lf"
#define CPU_PROM_FMT "node_cpu_seconds_total{cpu=\"%s\",mode=\"user\"} %.2f\n"   \
                     "node_cpu_seconds_total{cpu=\"%s\",mode=\"nice\"} %.2f\n"   \
                     "node_cpu_seconds_total{cpu=\"%s\",mode=\"system\"} %.2f\n" \
                     "node_cpu_seconds_total{cpu=\"%s\",mode=\"idle\"} %.2f\n"   \
                     "node_cpu_seconds_total{cpu=\"%s\",mode=\"iowait\"} %.2f\n" \
                     "node_cpu_seconds_total{cpu=\"%s\",mode=\"irq\"} %.2f\n"    \
                     "node_cpu_seconds_total{cpu=\"%s\",mode=\"softirq\"} %.2f\n"

#define USER "user"       // normal processes executing in user mode
#define NICE "nice"       // niced processes executing in user mode
#define SYSTEM "system"   // processes executing in kernel mode
#define IDLE "idle"       // twiddling thumbs
#define IOWAIT "iowait"   // waiting for I/O to complete
#define IRQ "irq"         // servicing interrupts
#define SOFTIRQ "softirq" // servicing softirqs

#define CPU_STATS_LEN 1024 * 5
#define CPU_STATS_LINE_MAX_LEN 256

/* check data is cpu's data?*/
int is_cpu_data(const char *str, const char *pre);

/* split cpu number*/
void split_cpu_name(char *cpuName);

/* convert val from hundredth seconds to seconds*/
double to_seconds(double val);

u_char *ngx_http_get_cpu_stats(ngx_http_request_t *r, u_char *buf);

#endif /*_NGX_METRIC_COLLECT_CPU_STATS_H*/