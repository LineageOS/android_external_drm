/* -*- mode: C; c-file-style: "k&r"; tab-width 4; indent-tabs-mode: t; -*- */

/*
 * Copyright (C) 2013 Rob Clark <robclark@freedesktop.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Rob Clark <robclark@freedesktop.org>
 */

#include "msm_priv.h"


static int msm_pipe_get_param(struct fd_pipe *pipe,
		enum fd_param_id param, uint64_t *value)
{
	struct msm_pipe *msm_pipe = to_msm_pipe(pipe);
	switch(param) {
	case FD_DEVICE_ID: // XXX probably get rid of this..
	case FD_GPU_ID:
		*value = msm_pipe->gpu_id;
		return 0;
	case FD_GMEM_SIZE:
		*value = msm_pipe->gmem;
		return 0;
	case FD_CHIP_ID:
		*value = msm_pipe->chip_id;
		return 0;
	default:
		ERROR_MSG("invalid param id: %d", param);
		return -1;
	}
}

static int msm_pipe_wait(struct fd_pipe *pipe, uint32_t timestamp)
{
	struct fd_device *dev = pipe->dev;
	struct drm_msm_wait_fence req = {
			.fence = timestamp,
	};
	int ret;

	get_abs_timeout(&req.timeout, 5000);

	ret = drmCommandWrite(dev->fd, DRM_MSM_WAIT_FENCE, &req, sizeof(req));
	if (ret) {
		ERROR_MSG("wait-fence failed! %d (%s)", ret, strerror(errno));
		return ret;
	}

	return 0;
}

static void msm_pipe_destroy(struct fd_pipe *pipe)
{
	struct msm_pipe *msm_pipe = to_msm_pipe(pipe);
	free(msm_pipe);
}

static struct fd_pipe_funcs funcs = {
		.ringbuffer_new = msm_ringbuffer_new,
		.get_param = msm_pipe_get_param,
		.wait = msm_pipe_wait,
		.destroy = msm_pipe_destroy,
};

static uint64_t get_param(struct fd_device *dev, uint32_t pipe, uint32_t param)
{
	struct drm_msm_param req = {
			.pipe = pipe,
			.param = param,
	};
	int ret;

	ret = drmCommandWriteRead(dev->fd, DRM_MSM_GET_PARAM, &req, sizeof(req));
	if (ret) {
		ERROR_MSG("get-param failed! %d (%s)", ret, strerror(errno));
		return 0;
	}

	return req.value;
}

struct fd_pipe * msm_pipe_new(struct fd_device *dev, enum fd_pipe_id id)
{
	static const uint32_t pipe_id[] = {
			[FD_PIPE_3D] = MSM_PIPE_3D0,
			[FD_PIPE_2D] = MSM_PIPE_2D0,
	};
	struct msm_pipe *msm_pipe = NULL;
	struct fd_pipe *pipe = NULL;

	msm_pipe = calloc(1, sizeof(*msm_pipe));
	if (!msm_pipe) {
		ERROR_MSG("allocation failed");
		goto fail;
	}

	pipe = &msm_pipe->base;
	pipe->funcs = &funcs;

	msm_pipe->pipe = pipe_id[id];
	msm_pipe->gpu_id = get_param(dev, pipe_id[id], MSM_PARAM_GPU_ID);
	msm_pipe->gmem   = get_param(dev, pipe_id[id], MSM_PARAM_GMEM_SIZE);
	msm_pipe->chip_id = get_param(dev, pipe_id[id], MSM_PARAM_CHIP_ID);

	if (! msm_pipe->gpu_id)
		goto fail;

	INFO_MSG("Pipe Info:");
	INFO_MSG(" GPU-id:          %d", msm_pipe->gpu_id);
	INFO_MSG(" Chip-id:         0x%08x", msm_pipe->chip_id);
	INFO_MSG(" GMEM size:       0x%08x", msm_pipe->gmem);

	return pipe;
fail:
	if (pipe)
		fd_pipe_del(pipe);
	return NULL;
}
