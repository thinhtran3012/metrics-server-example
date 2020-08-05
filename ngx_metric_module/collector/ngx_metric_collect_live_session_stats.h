#ifndef _NGX_METRIC_COLLECT_LIVE_SESSION_INFO_H_
#define _NGX_METRIC_COLLECT_LIVE_SESSION_INFO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_string.h>
#include "ngx_util.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "global.h"

// #define CMD_FIND_PID_UIZA_FFMPEG_PROC "grep /opt/uiza-ffmpeg/uiza-ffmpeg /opt/rootfs/proc/*/cmdline"
#define CMD_FIND_PID_UIZA_FFMPEG_PROC "find /opt/rootfs/proc/*/cmdline -type f -exec grep -l \"/opt/uiza-ffmpeg/uiza-ffmpeg\" {} \\;"
#define PID_CMDLINE_PATH "/opt/rootfs/proc/%s/cmdline"
#define PID_IO_PATH "/opt/rootfs/proc/%s/io"
#define PID_STATUS_PATH "/opt/rootfs/proc/%s/status"

#define LIVE_SESSION_IO_RCHAR_FORMAT                                                                                    \
    "# HELP live_session_io_rchar_size_bytes The number of bytes which this task has caused to be read from storage.\n" \
    "# TYPE live_session_io_rchar_size_bytes gauge\n"                                                                   \
    "live_session_io_rchar_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_IO_WCHAR_FORMAT                                                            \
    "# HELP live_session_io_wchar_size_bytes The number of bytes which this task has caused,\n" \
    "# or shall cause to be written to disk.\n"                                                 \
    "# TYPE live_session_io_wchar_size_bytes gauge\n"                                           \
    "live_session_io_wchar_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_IO_SYSCR_FORMAT                                                                 \
    "# HELP live_session_io_syscr_total_count Attempt to count the number of read I/O operations.\n" \
    "# TYPE live_session_io_syscr_total_count gauge\n"                                               \
    "live_session_io_syscr_total_count{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_IO_SYSCW_FORMAT                                                                  \
    "# HELP live_session_io_syscw_total_count Attempt to count the number of write I/O operations.\n" \
    "# TYPE live_session_io_syscw_total_count gauge\n"                                                \
    "live_session_io_syscw_total_count{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_IO_READ_FORMAT                                                 \
    "# HELP live_session_io_read_size_bytes Attempt to count the number of bytes\n" \
    "# which this process really did cause to be fetched from the storage layer.\n" \
    "# TYPE live_session_io_read_size_bytes gauge\n"                                \
    "live_session_io_read_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_IO_WRITE_FORMAT                                                 \
    "# HELP live_session_io_write_size_bytes Attempt to count the number of bytes\n" \
    "# which this process caused to be sent to the storage layer.\n"                 \
    "# TYPE live_session_io_write_size_bytes gauge\n"                                \
    "live_session_io_write_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_IO_CANCELLED_WRITE_FORMAT                                                      \
    "# HELP live_session_io_cancelled_write_size_bytes this field represents the number of bytes\n" \
    "# which this process caused to not happen, by truncating page‐cache.\n"                      \
    "# TYPE live_session_io_cancelled_write_size_bytes gauge\n"                                     \
    "live_session_io_cancelled_write_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"

#define LIVE_SESSION_MEMORY_VM_PEAK_FORMAT                                      \
    "# HELP live_session_memory_vm_peak_size_bytes Peak virtual memory size.\n" \
    "# TYPE live_session_memory_vm_peak_size_bytes gauge\n"                     \
    "live_session_memory_vm_peak_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_SIZE_FORMAT                            \
    "# HELP live_session_memory_vm_size_bytes Virtual memory size.\n" \
    "# TYPE live_session_memory_vm_size_bytes gauge\n"                \
    "live_session_memory_vm_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_LCK_FORMAT                                \
    "# HELP live_session_memory_vm_lck_size_bytes Locked memory size.\n" \
    "# TYPE live_session_memory_vm_lck_size_bytes gauge\n"               \
    "live_session_memory_vm_lck_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_PIN_FORMAT                                \
    "# HELP live_session_memory_vm_pin_size_bytes Pinned memory size.\n" \
    "# TYPE live_session_memory_vm_pin_size_bytes gauge\n"               \
    "live_session_memory_vm_pin_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_HWM_FORMAT                                    \
    "# HELP live_session_memory_vm_hwm_size_bytes Peak resident set size.\n" \
    "# TYPE live_session_memory_vm_hwm_size_bytes gauge\n"                   \
    "live_session_memory_vm_hwm_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_RSS_FORMAT                                                                                                      \
    "# HELP live_session_memory_vm_rss_size_bytes Resident set size. Note that the value here is the sum of RssAnon, RssFile, and RssShmem.\n" \
    "# TYPE live_session_memory_vm_rss_size_bytes gauge\n"                                                                                     \
    "live_session_memory_vm_rss_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_RSS_ANON_FORMAT                                               \
    "# HELP live_session_memory_rss_anon_size_bytes Size of resident anonymous memory.\n" \
    "# TYPE live_session_memory_rss_anon_size_bytes gauge\n"                              \
    "live_session_memory_rss_anon_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_RSS_FILE_FORMAT                                            \
    "# HELP live_session_memory_rss_file_size_bytes Size of resident file mappings.\n" \
    "# TYPE live_session_memory_rss_file_size_bytes gauge\n"                           \
    "live_session_memory_rss_file_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_RSS_SHMEM_FORMAT                                               \
    "# HELP live_session_memory_rss_shmem_size_bytes Size of resident anonymous memory.\n" \
    "# TYPE live_session_memory_rss_shmem_size_bytes gauge\n"                              \
    "live_session_memory_rss_shmem_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_RSS_ANON_FORMAT                                               \
    "# HELP live_session_memory_rss_anon_size_bytes Size of resident anonymous memory.\n" \
    "# TYPE live_session_memory_rss_anon_size_bytes gauge\n"                              \
    "live_session_memory_rss_anon_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_DATA_FORMAT                          \
    "# HELP live_session_memory_vm_data_size_bytes Size of data.\n" \
    "# TYPE live_session_memory_vm_data_size_bytes gauge\n"         \
    "live_session_memory_vm_data_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_STK_FORMAT                           \
    "# HELP live_session_memory_vm_stk_size_bytes Size of stack.\n" \
    "# TYPE live_session_memory_vm_stk_size_bytes gauge\n"          \
    "live_session_memory_vm_stk_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_EXE_FORMAT                                   \
    "# HELP live_session_memory_vm_exe_size_bytes Size of text segments.\n" \
    "# TYPE live_session_memory_vm_exe_size_bytes gauge\n"                  \
    "live_session_memory_vm_exe_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_LIB_FORMAT                                      \
    "# HELP live_session_memory_vm_lib_size_bytes Shared library code size.\n" \
    "# TYPE live_session_memory_vm_lib_size_bytes gauge\n"                     \
    "live_session_memory_vm_lib_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_PTE_FORMAT                                     \
    "# HELP live_session_memory_vm_pte_size_bytes Page table entries size.\n" \
    "# TYPE live_session_memory_vm_pte_size_bytes gauge\n"                    \
    "live_session_memory_vm_pte_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_PMD_FORMAT                                              \
    "# HELP live_session_memory_vm_pmd_size_bytes Size of second-level page tables.\n" \
    "# TYPE live_session_memory_vm_pmd_size_bytes gauge\n"                             \
    "live_session_memory_vm_pmd_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_SWAP_FORMAT                                                                        \
    "# HELP live_session_memory_vm_swap_size_bytes Swapped-out virtual memory size by anonymous private pages.\n" \
    "# TYPE live_session_memory_vm_swap_size_bytes gauge\n"                                                       \
    "live_session_memory_vm_swap_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"
#define LIVE_SESSION_MEMORY_VM_HUGE_TLB_PAGES_FORMAT                                             \
    "# HELP live_session_memory_vm_huge_tlb_pages_size_bytes Size of hugetlb memory portions.\n" \
    "# TYPE live_session_memory_vm_huge_tlb_pages_size_bytes gauge\n"                            \
    "live_session_memory_vm_huge_tlb_pages_size_bytes{stream_name=\"%s\",session_id=\"\"} %ul\n"

#define LIVE_SESSION_CPU_UTILIZATION_FORMAT                                         \
    "# HELP live_session_cpu_utilization_in_percentage Cpu usage of the process.\n" \
    "# TYPE live_session_cpu_utilization_in_percentage gauge\n"                     \
    "live_session_cpu_utilization_in_percentage{stream_name=\"%s\",session_id=\"\"} %.2f\n"

#define LIVE_SESSION_MEMORY_UTILIZATION_FORMAT                                         \
    "# HELP live_session_memory_utilization_in_percentage Memory usage of the process.\n" \
    "# TYPE live_session_memory_utilization_in_percentage gauge\n"                     \
    "live_session_memory_utilization_in_percentage{stream_name=\"%s\",session_id=\"\"} %.2f\n"

// get cmdline from /proc/[PID]/cmdline
char *get_cmdline(ngx_http_request_t *r, char *pid);

// get all pid of processes uiza-ffmpeg that handle each live session
char *pidof(ngx_http_request_t *r, const char *cmdlines);

// parse stream_key from cmd
char *parse_stream_key(ngx_http_request_t *r, char *cmd_param);

// get stream_key using pid
char *get_stream_key(ngx_http_request_t *r, char *pid);

// get io stat of each live session
u_char *ngx_http_get_io_stat(ngx_http_request_t *r, u_char *buf, char *pid, char *stream_key);

// get cpu utilization of each live session
u_char *ngx_http_get_cpu_utilization(ngx_http_request_t *r, u_char *buf, char *pid, char *stream_key);

// get memory utilization of each live session
u_char *ngx_http_get_memory_utilization(ngx_http_request_t *r, u_char *buf, char *pid, char *stream_key);

// get live session metrics
u_char *ngx_http_get_live_session_stats(ngx_http_request_t *r, u_char *buf);

// check dir is exists
int is_file_exist(char *file_path);

// get cpu_total_time
long unsigned int get_cpu_total_time();

// get get_process_cpu_total_time
long unsigned int get_cpu_total_time_of_process(char *pid);

#endif /* _NGX_METRIC_COLLECT_LIVE_SESSION_INFO_H_ */