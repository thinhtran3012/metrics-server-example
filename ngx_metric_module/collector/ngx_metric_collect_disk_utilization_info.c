#include "ngx_metric_collect_disk_utilization_info.h"

u_char *ngx_http_get_disk_utilization_info(ngx_http_request_t *r, u_char *buf)
{
    char *content = read_proc_file(DISK_SPACE_INFO_FILE, r);
    if (!strcmp(content, ""))
    {
        content = read_proc_file(DISK_SPACE_INFO_ALTERNATIVE_FILE, r);
        if (!strcmp(content, ""))
        {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Could not open file: %s\n", DISK_SPACE_INFO_ALTERNATIVE_FILE);
            return buf;
        }
    }

    node_array_t tokens = str_split(content, '\n');
    int lengFor = (int)tokens.len;
    if (lengFor > 0)
    {
        char **data = tokens.data;
        int i;
        char *device = (char *)calloc(256, sizeof(char));
        char *fsTypes = (char *)calloc(256, sizeof(char));
        char *mountPoint = (char *)calloc(256, sizeof(char));
        char *options = (char *)calloc(256, sizeof(char));

        for (i = 0; i < lengFor; i++)
        {
            char *line = *(data + i);
            sscanf(line, READ_LINE_FORMAT, device, mountPoint, fsTypes, options);

            if (!ngx_is_ignored_mount_points(mountPoint) || !ngx_is_ignored_fs_types(fsTypes))
            {
                if (!ngx_is_ignored_fs_types(fsTypes) && !ngx_is_ignored_mount_points(mountPoint))
                {
                    char *newMountPoint = (char *)calloc(strlen(mountPoint) + strlen(MOUNTPOINT_WITH_ROOT_FS_PATH_PREFIX_FORMAT) + 1, sizeof(char));
                    ngx_add_root_fs_prefix(newMountPoint, mountPoint);
                    struct statvfs diskSpaceData;
                    int deviceErr = 0;
                    if (statvfs(newMountPoint, &diskSpaceData) < 0)
                    {
                        deviceErr = 1;
                    }
                    else
                    {
                        unsigned long blockSize = diskSpaceData.f_blocks * diskSpaceData.f_bsize;
                        unsigned long blockFree = diskSpaceData.f_bfree * diskSpaceData.f_bsize;
                        unsigned long blockAvail = diskSpaceData.f_favail * diskSpaceData.f_bsize;
                        unsigned long files = diskSpaceData.f_files;
                        unsigned long fileFree = diskSpaceData.f_ffree;
                        unsigned long fileAvail = diskSpaceData.f_favail;
                        int readOnly = 0;
                        if (start_with(options, "ro"))
                            readOnly = 1;

                        buf = ngx_sprintf(buf, CPU_DISK_UTILIZATION_INFO_FORMAT,
                                          device, fsTypes, mountPoint, blockSize,
                                          device, fsTypes, mountPoint, blockFree,
                                          device, fsTypes, mountPoint, blockAvail,
                                          device, fsTypes, mountPoint, files,
                                          device, fsTypes, mountPoint, fileFree,
                                          device, fsTypes, mountPoint, fileAvail,
                                          device, fsTypes, mountPoint, readOnly);
                    }
                    buf = ngx_sprintf(buf, FILESYSTEM_DEVICE_ERR, device, fsTypes, mountPoint, deviceErr);
                    if (newMountPoint)
                    {
                        free(newMountPoint);
                    }
                }
            }
            if (line)
            {
                free(line);
            }
        }
        
        if (device)
        {
            free(device);
        }
        if (fsTypes)
        {
            free(fsTypes);
        }
        if (options)
        {
            free(options);
        }
        if (mountPoint)
        {
            free(mountPoint);
        }
        if (data)
        {
            free(data);
        }
    }
    free(content);
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "DISK_UTILIZATION_DONE!");
    return buf;
}

ngx_int_t ngx_is_ignored_mount_points(char *mount_point)
{
    if (start_with(mount_point, "/dev") ||
        start_with(mount_point, "/proc") ||
        start_with(mount_point, "/sys") ||
        start_with(mount_point, "/var/lib") ||
        start_with(mount_point, "/home/kubernetes"))
        return 1;
    return 0;
}

//(autofs|binfmt_misc|bpf|cgroup2?|configfs|debugfs|devpts|devtmpfs|fusectl|hugetlbfs|
//iso9660|mqueue|nsfs|overlay|proc|procfs|pstore|rpc_pipefs|securityfs|selinuxfs|squashfs|sysfs|tracefs)$"

ngx_int_t ngx_is_ignored_fs_types(char *fs_types)
{
    if (!ngx_strcmp(fs_types, "autofs") ||
        !ngx_strcmp(fs_types, "binfmt_misc") ||
        !ngx_strcmp(fs_types, "bpf") ||
        !ngx_strcmp(fs_types, "cgroup") ||
        !ngx_strcmp(fs_types, "configfs") ||
        !ngx_strcmp(fs_types, "debugfs") ||
        !ngx_strcmp(fs_types, "devpts") ||
        !ngx_strcmp(fs_types, "devtmpfs") ||
        !ngx_strcmp(fs_types, "fusectl") ||
        !ngx_strcmp(fs_types, "hugetlbfs") ||
        !ngx_strcmp(fs_types, "iso9660") ||
        !ngx_strcmp(fs_types, "mqueue") ||
        !ngx_strcmp(fs_types, "nsfs") ||
        !ngx_strcmp(fs_types, "overlay") ||
        !ngx_strcmp(fs_types, "proc") ||
        !ngx_strcmp(fs_types, "procfs") ||
        !ngx_strcmp(fs_types, "pstore") ||
        !ngx_strcmp(fs_types, "rpc_pipefs") ||
        !ngx_strcmp(fs_types, "securityfs") ||
        !ngx_strcmp(fs_types, "selinuxfs") ||
        !ngx_strcmp(fs_types, "squashfs") ||
        !ngx_strcmp(fs_types, "sysfs") ||
        !ngx_strcmp(fs_types, "tracefs"))
        return 1;
    return 0;
}

char *ngx_add_root_fs_prefix(char *new_mount_point, char *mount_point)
{
    sprintf(new_mount_point, MOUNTPOINT_WITH_ROOT_FS_PATH_PREFIX_FORMAT, mount_point);
    return new_mount_point;
}
