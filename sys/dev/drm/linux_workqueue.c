/*
 * Copyright (c) 2015-2018 François Tigeot <ftigeot@wolfpond.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <drm/drmP.h>
#include <linux/workqueue.h>

struct workqueue_struct *system_wq;
struct workqueue_struct *system_long_wq;
struct workqueue_struct *system_power_efficient_wq;
struct workqueue_struct *system_unbound_wq;

static int init_workqueues(void *arg)
{
	system_wq = alloc_workqueue("system_wq", 0, 1);
	system_long_wq = alloc_workqueue("system_long_wq", 0, 1);
	system_power_efficient_wq = alloc_workqueue("system_power_efficient_wq", 0, 1);
	system_unbound_wq = alloc_workqueue("system_unbound_wq", 0, 1);

	return 0;
}

static int destroy_workqueues(void *arg)
{
	destroy_workqueue(system_wq);
	destroy_workqueue(system_long_wq);
	destroy_workqueue(system_power_efficient_wq);
	destroy_workqueue(system_unbound_wq);

	return 0;
}

struct workqueue_struct *
_create_workqueue_common(char *name, int cpus)
{
	struct workqueue_struct *wq;

	wq = kmalloc(sizeof(*wq), M_DRM, M_WAITOK);
	wq->taskqueue = taskqueue_create((name), M_WAITOK,
	    taskqueue_thread_enqueue,  &wq->taskqueue);
	taskqueue_start_threads(&wq->taskqueue, cpus,
				TDPRI_KERN_DAEMON, -1, "%s", name);

	return (wq);
}

void
destroy_workqueue(struct workqueue_struct *wq)
{
	taskqueue_free(wq->taskqueue);
	kfree(wq);
}

SYSINIT(linux_workqueue_init, SI_SUB_DRIVERS, SI_ORDER_MIDDLE, init_workqueues, NULL);
SYSUNINIT(linux_workqueue_destroy, SI_SUB_DRIVERS, SI_ORDER_MIDDLE, destroy_workqueues, NULL);

bool
flush_delayed_work(struct delayed_work *dwork)
{
	callout_drain(&dwork->timer);
	return flush_work(&dwork->work);
}

void
drain_workqueue(struct workqueue_struct *wq)
{
	lockmgr(&wq->flags_lock, LK_EXCLUSIVE);
	wq->is_draining = true;
	lockmgr(&wq->flags_lock, LK_RELEASE);

	flush_taskqueue(wq->taskqueue);

	lockmgr(&wq->flags_lock, LK_EXCLUSIVE);
	wq->is_draining = false;
	lockmgr(&wq->flags_lock, LK_RELEASE);
}
