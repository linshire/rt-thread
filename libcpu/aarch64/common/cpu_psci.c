/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */
#include <rthw.h>
#include <rtthread.h>
#include <stdint.h>

#define DBG_TAG "libcpu.aarch64.cpu_psci"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#include "cpu.h"
#include "errno.h"
#include "psci.h"
#include "psci_api.h"
#include "entry_point.h"

int (*_psci_init)(void) = psci_init;

static int __call_method_init()
{
    int (*init)(void) = _psci_init;
    _psci_init = RT_NULL;

    return init();
}

/** return 0 on success, otherwise failed */
#define _call_method_init() ((_psci_init) ? __call_method_init() : 0);

static int cpu_psci_cpu_init(rt_uint32_t cpuid)
{
    // init psci only once
    return _call_method_init();
}

static int cpu_psci_cpu_boot(rt_uint32_t cpuid)
{
    rt_uint64_t secondary_entry_pa = (rt_uint64_t)_secondary_cpu_entry + PV_OFFSET;
    if (!psci_ops.cpu_on) {
        LOG_E("Uninitialized psci operation");
        return -1;
    }
    return psci_ops.cpu_on(cpuid_to_hwid(cpuid), secondary_entry_pa);
}

struct cpu_ops_t cpu_ops_psci = {
    .method = "psci",
    .cpu_boot = cpu_psci_cpu_boot,
    .cpu_init = cpu_psci_cpu_init,
    .cpu_shutdown = RT_NULL
};
