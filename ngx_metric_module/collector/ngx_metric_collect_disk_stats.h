#ifndef _METRIC_COLLECT_DISK_STATS_H_
#define _METRIC_COLLECT_DISK_STATS_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ngx_util.h"
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "global.h"

/*
The /proc/diskstats file displays the I/O statistics
		of block devices. Each line contains the following 14
		fields:

		 1 - major number
		 2 - minor mumber
		 3 - device name
		 4 - reads completed successfully
		 5 - reads merged
		 6 - sectors read
		 7 - time spent reading (ms)
		 8 - writes completed
		 9 - writes merged
		10 - sectors written
		11 - time spent writing (ms)
		12 - I/Os currently in progress
		13 - time spent doing I/Os (ms)
		14 - weighted time spent doing I/Os (ms)
	detail: https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats
*/

#define DISK_STATS_FILE "/opt/rootfs/proc/diskstats"
#define DISK_STATS_LEN 1024 * 5
#define DISK_STAT_LINE_MAX_LEN 256
#define DISK_MAJOR_NUMBER 8
#define START_METADATA_LABEL "{device=\""
#define END_METADATA_LABEL "\"} "
#define ENDLINE_FORMAT "\n"
#define DISK_INTRO "#metric_diskstats\n"

// Read I/O
#define METRIC_DISK_READ_FMT                                                                     \
	"# HELP node_disk_reads_completed_total The total number of reads completed successfully.\n" \
	"# TYPE node_disk_reads_completed_total counter\n"                                           \
	"node_disk_reads_completed_total{device=\"%s\"} %ul\n"                                       \
	"# HELP node_disk_reads_merged_total The total number of reads merged.\n"                    \
	"# TYPE node_disk_reads_merged_total counter\n"                                              \
	"node_disk_reads_merged_total{device=\"%s\"} %ul\n"                                          \
	"# HELP node_disk_read_bytes_total The total number of bytes read successfully.\n"           \
	"# TYPE node_disk_read_bytes_total counter\n"                                                \
	"node_disk_read_bytes_total{device=\"%s\"} %ul\n"                                            \
	"# HELP node_disk_read_time_seconds_total The total number of seconds spent by all reads.\n" \
	"# TYPE node_disk_read_time_seconds_total counter\n"                                         \
	"node_disk_read_time_seconds_total{device=\"%s\"} %ul\n"

// Write I/O
#define METRIC_DISK_WRITE_FMT                                                                              \
	"# HELP node_disk_writes_completed_total The total number of writes completed successfully.\n"         \
	"# TYPE node_disk_writes_completed_total counter\n"                                                    \
	"node_disk_writes_completed_total{device=\"%s\"} %ul\n"                                                \
	"# HELP node_disk_writes_merged_total The number of writes merged.\n"                                  \
	"# TYPE node_disk_writes_merged_total counter\n"                                                       \
	"node_disk_writes_merged_total{device=\"%s\"} %ul\n"                                                   \
	"# HELP node_disk_written_bytes_total The total number of bytes written successfully.\n"               \
	"# TYPE node_disk_written_bytes_total counter\n"                                                       \
	"node_disk_written_bytes_total{device=\"%s\"} %ul\n"                                                   \
	"# HELP node_disk_write_time_seconds_total This is the total number of seconds spent by all writes.\n" \
	"# TYPE node_disk_write_time_seconds_total counter\n"                                                  \
	"node_disk_write_time_seconds_total{device=\"%s\"} %ul\n"

#define METRIC_DISK_IO_FMT                                                                          \
	"# HELP node_disk_io_now The number of I/Os currently in progress.\n"                           \
	"# TYPE node_disk_io_now gauge\n"                                                               \
	"node_disk_io_now{device=\"%s\"} %ul\n"                                                         \
	"# HELP node_disk_io_time_seconds_total Total seconds spent doing I/Os.\n"                      \
	"# TYPE node_disk_io_time_seconds_total counter\n"                                              \
	"node_disk_io_time_seconds_total{device=\"%s\"} %ul\n"                                          \
	"# HELP node_disk_io_time_weighted_seconds_total The weighted # of seconds spent doing I/Os.\n" \
	"# TYPE node_disk_io_time_weighted_seconds_total counter\n"                                     \
	"node_disk_io_time_weighted_seconds_total{device=\"%s\"} %ul\n"

int is_device_name_not_contain_number(char str[]);

int is_disk_device(char *deviceName);

u_char *ngx_http_get_disk_stats(ngx_http_request_t *r, u_char *buf);

#endif /* _METRIC_COLLECT_DISK_STATS_H_ */