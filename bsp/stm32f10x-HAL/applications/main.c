/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include "rtdevice.h"
#include <ulog.h>
#include <fal.h>
#include <dfs_fs.h>
#include "cJSON.h"

#define FS_PARTITION_NAME              "filesys"

void fs_init(void)
{
    struct rt_device *mtd_dev = RT_NULL;
    /* ��ʼ�� fal */    
    fal_init();
    /* ���� mtd �豸 */
    mtd_dev = fal_mtd_nor_device_create(FS_PARTITION_NAME);
    if (!mtd_dev)
    {
        LOG_E("Can't create a mtd device on '%s' partition.", FS_PARTITION_NAME);
    }
    else
    {
        /* ���� littlefs */
        if (dfs_mount(FS_PARTITION_NAME, "/", "lfs", 0, 0) == 0)
        {
            LOG_I("Filesystem initialized!");
        }
        else
        {
            /* ��ʽ���ļ�ϵͳ */
            dfs_mkfs("lfs", FS_PARTITION_NAME);
            /* ���� littlefs */
            if (dfs_mount("filesystem", "/", "lfs", 0, 0) == 0)
            {
                LOG_I("Filesystem initialized!");
            }
            else
            {
                LOG_E("Failed to initialize filesystem!");
            }
        }
    }
}

int main(void)
{
    /* user app entry */
	fs_init();
    return 0;
}
