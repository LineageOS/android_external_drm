/* i915.h -- Intel I915 DRM template customization -*- linux-c -*-
 */
/**************************************************************************
 * 
 * Copyright 2003 Tungsten Graphics, Inc., Cedar Park, Texas.
 * All Rights Reserved.
 * 
 **************************************************************************/

#ifndef __I915_H__
#define __I915_H__

/* This remains constant for all DRM template files.
 */
#define DRM(x) i915_##x

/* General customization:
 */
#define __HAVE_AGP		1
#define __MUST_HAVE_AGP		1
#define __HAVE_MTRR		1
#define __HAVE_CTX_BITMAP	1

#define DRIVER_AUTHOR		"Tungsten Graphics, Inc."

#define DRIVER_NAME		"i915"
#define DRIVER_DESC		"Intel Graphics"
#define DRIVER_DATE		"20040405"

/* Interface history:
 *
 * 1.1: Original.
 */
#define DRIVER_MAJOR		1
#define DRIVER_MINOR		1
#define DRIVER_PATCHLEVEL	0


#define DRIVER_IOCTLS							    \
	[DRM_IOCTL_NR(DRM_IOCTL_I915_INIT)]   = { i915_dma_init,    1, 1 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_I915_FLUSH)]  = { i915_flush_ioctl, 1, 0 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_I915_FLIP)]   = { i915_flip_bufs,   1, 0 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_I915_BATCHBUFFER)] = { i915_batchbuffer, 1, 0 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_I915_IRQ_EMIT)] = { i915_irq_emit,  1, 0 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_I915_IRQ_WAIT)] = { i915_irq_wait,  1, 0 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_I915_GETPARAM)] = { i915_getparam,  1, 0 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_I915_SETPARAM)] = { i915_setparam,  1, 1 }, \
        [DRM_IOCTL_NR(DRM_IOCTL_I915_ALLOC)]   = { i915_mem_alloc,  1, 0 }, \
        [DRM_IOCTL_NR(DRM_IOCTL_I915_FREE)]    = { i915_mem_free,    1, 0 }, \
        [DRM_IOCTL_NR(DRM_IOCTL_I915_INIT_HEAP)] = { i915_mem_init_heap, 1, 1 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_I915_CMDBUFFER)] = { i915_cmdbuffer, 1, 0 }


#define __HAVE_COUNTERS         4
#define __HAVE_COUNTER6         _DRM_STAT_IRQ
#define __HAVE_COUNTER7         _DRM_STAT_PRIMARY
#define __HAVE_COUNTER8         _DRM_STAT_SECONDARY
#define __HAVE_COUNTER9         _DRM_STAT_DMA

/* Driver customization:
 */
#define DRIVER_PRETAKEDOWN(dev) do {					\
	if ( dev->dev_private ) {					\
		drm_i915_private_t *dev_priv = dev->dev_private;	\
	        i915_mem_takedown( &(dev_priv->agp_heap) );             \
 	}								\
	i915_dma_cleanup( dev );					\
} while (0)


/* When a client dies:
 *    - Free any alloced agp memory.
 */
#define DRIVER_PRERELEASE() 						\
do {									\
	if ( dev->dev_private ) {					\
		drm_i915_private_t *dev_priv = dev->dev_private;	\
                i915_mem_release( dev, filp, dev_priv->agp_heap );	\
	}								\
} while (0)




/* We use our own dma mechanisms, not the drm template code.  However,
 * the shared IRQ code is useful to us:
 */
#define __HAVE_DMA		0
#define __HAVE_IRQ		1
#define __HAVE_SHARED_IRQ	1


#define __HAVE_PM		1

#endif