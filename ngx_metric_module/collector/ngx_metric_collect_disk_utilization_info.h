#ifndef _NGX_METRIC_COLLECT_DISK_UTILIZATION_H
#define _NGX_METRIC_COLLECT_DISK_UTILIZATION_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "global.h"
#include "ngx_util.h"
#include <sys/statvfs.h>

#define BUFSIZE 1024 * 5
#define READ_LINE_FORMAT "%s %s %s %10[^,],"
#define CMD_GET_CPU_UTILIZATION_INFO "df -T"
#define DISK_SPACE_INFO_FILE "/opt/rootfs/proc/1/mounts"
#define DISK_SPACE_INFO_ALTERNATIVE_FILE "/opt/rootfs/proc/mounts"
#define CPU_UTILIZATION_INTRO "#metric_disk_utilization\n"
#define MOUNTPOINT_WITH_ROOT_FS_PATH_PREFIX_FORMAT "/opt/rootfs%s"

#define CPU_DISK_UTILIZATION_INFO_FORMAT                                                          \
	"# HELP node_filesystem_size_bytes Filesystem size in bytes.\n"                               \
	"# TYPE node_filesystem_size_bytes gauge\n"                                                   \
	"node_filesystem_size_bytes{device=\"%s\",fstype=\"%s\",mountpoint=\"%s\"} %ul\n"             \
	"# HELP node_filesystem_avail_bytes Filesystem space available to non-root users in bytes.\n" \
	"# TYPE node_filesystem_avail_bytes gauge\n"                                                  \
	"node_filesystem_avail_bytes{device=\"%s\",fstype=\"%s\",mountpoint=\"%s\"} %ul\n"            \
	"# HELP node_filesystem_free_bytes Filesystem free space in bytes.\n"                         \
	"# TYPE node_filesystem_free_bytes gauge\n"                                                   \
	"node_filesystem_free_bytes{device=\"%s\",fstype=\"%s\",mountpoint=\"%s\"} %ul\n"             \
	"# HELP node_filesystem_files Filesystem total file nodes.\n"                                 \
	"# TYPE node_filesystem_files gauge\n"                                                        \
	"node_filesystem_files{device=\"%s\",fstype=\"%s\",mountpoint=\"%s\"} %ul\n"                  \
	"# HELP node_filesystem_files_free Filesystem total free file nodes.\n"                       \
	"# TYPE node_filesystem_files_free gauge\n"                                                   \
	"node_filesystem_files_free{device=\"%s\",fstype=\"%s\",mountpoint=\"%s\"} %ul\n"             \
	"# HELP node_filesystem_files_avail Filesystem total avail file nodes.\n"                     \
	"# TYPE node_filesystem_files_avail gauge\n"                                                  \
	"node_filesystem_files_avail{device=\"%s\",fstype=\"%s\",mountpoint=\"%s\"} %ul\n"            \
	"# HELP node_filesystem_readonly Filesystem read-only status.\n"                              \
	"# TYPE node_filesystem_readonly gauge\n"                                                     \
	"node_filesystem_readonly{device=\"%s\",fstype=\"%s\",mountpoint=\"%s\"} %d\n"

#define FILESYSTEM_DEVICE_ERR                                                                                        \
	"# HELP node_filesystem_device_error Whether an error occurred while getting statistics for the given device.\n" \
	"# TYPE node_filesystem_device_error gauge\n"                                                                    \
	"node_filesystem_device_error{device=\"%s\",fstype=\"%s\",mountpoint=\"%s\"} %d\n"

u_char *ngx_http_get_disk_utilization_info(ngx_http_request_t *r, u_char *buf);

ngx_int_t ngx_is_ignored_mount_points(char *mount_point);

ngx_int_t ngx_is_ignored_fs_types(char *fs_types);

char *ngx_add_root_fs_prefix(char *new_mount_point, char *mount_point);

#endif /* _NGX_METRIC_COLLECT_DISK_UTILIZATION_H */
