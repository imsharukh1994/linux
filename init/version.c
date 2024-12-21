// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/init/version.c
 *
 *  Enhanced hostname handling and kernel version initialization.
 *  Copyright (C) 1992 Theodore Ts'o
 *  May be freely distributed as part of Linux.
 */

#include <generated/compile.h>
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/uts.h>
#include <linux/utsname.h>
#include <linux/proc_ns.h>

/**
 * early_hostname - Set system hostname during early initialization.
 * @arg: The hostname passed as a kernel parameter.
 *
 * Validates the hostname and truncates it if it exceeds the allowed length.
 * Logs warnings if any issues occur.
 */
static int __init early_hostname(char *arg)
{
    size_t bufsize = sizeof(init_uts_ns.name.nodename);
    size_t maxlen  = bufsize - 1; // Leave space for the null terminator
    ssize_t arglen;

    if (!arg || *arg == '\0') {
        pr_warn("No hostname provided. Using default hostname.");
        return -EINVAL;
    }

    arglen = strscpy(init_uts_ns.name.nodename, arg, bufsize);
    if (arglen < 0) {
        pr_warn("Hostname parameter exceeds %zd characters and was truncated.", maxlen);
    } else {
        pr_info("Hostname set to: %s", init_uts_ns.name.nodename);
    }

    return 0;
}
early_param("hostname", early_hostname);

/* Linux kernel version banner */
const char linux_proc_banner[] =
    "%s version %s"
    " (" LINUX_COMPILE_BY "@" LINUX_COMPILE_HOST ")"
    " (" LINUX_COMPILER ") %s\n";

/* Metadata for build reproducibility and debugging */
BUILD_SALT;
BUILD_LTO_INFO;

/*
 * Weak symbols for namespace and banner information.
 * These can be overridden during the build process.
 */
struct uts_namespace init_uts_ns __weak;
const char linux_banner[] __weak;

/* Include build timestamp and version information */
#include "version-timestamp.c"

/* Export init_uts_ns to make it accessible to modules */
EXPORT_SYMBOL_GPL(init_uts_ns);
