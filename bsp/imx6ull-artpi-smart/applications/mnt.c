/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtthread.h>

#ifdef RT_USING_DFS

#include <dfs_fs.h>
#include <dfs_romfs.h>

int mnt_init(void)
{
    rt_thread_mdelay(500);
    if (dfs_mount("sd0", "/", "elm", 0, NULL) != 0)
    {
        rt_kprintf("Dir / mount failed!\n");
        return -1;
    }
    else
    {
        mkdir("/romfs", 777);
        mkdir("/flash", 777);
        mkdir("/download", 777);
    }
    rt_kprintf("file system initialization done!\n");
    return 0;
}
INIT_APP_EXPORT(mnt_init);


#endif
