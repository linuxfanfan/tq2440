#ifndef _TSLIB_H_
#define _TSLIB_H_
/*
 *  tslib/src/tslib.h
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the LGPL.
 *
 *
 * Touch screen library interface definitions.
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdarg.h>
#include <sys/time.h>

#ifdef WIN32
  #define TSIMPORT __declspec(dllimport)
  #define TSEXPORT __declspec(dllexport)
  #define TSLOCAL
#else
  #define TSIMPORT
  #ifdef GCC_HASCLASSVISIBILITY
    #define TSEXPORT __attribute__ ((visibility("default")))
    #define TSLOCAL __attribute__ ((visibility("hidden")))
  #else
    #define TSEXPORT
    #define TSLOCAL
  #endif
#endif

#ifdef TSLIB_INTERNAL
  #define TSAPI TSEXPORT
#else
  #define TSAPI TSIMPORT
#endif /* TSLIB_INTERNAL */

struct tsdev;

struct ts_sample {
	int		x;
	int		y;
	unsigned int	pressure;
	struct timeval	tv;
};

struct ts_sample_mt {
	/* most recent ABS_MT_* event codes.
	 * see linux/input.h for descriptions */
	int		x;
	int		y;
	unsigned int	pressure;
	int		slot;
	int		tracking_id;

	int		tool_type;
	int		tool_x;
	int		tool_y;
	unsigned int	touch_major;
	unsigned int	width_major;
	unsigned int	touch_minor;
	unsigned int	width_minor;
	int		orientation;
	int		distance;
	int		blob_id;

	struct timeval	tv;

	/* BTN_TOUCH state */
	short		pen_down;

	/* valid is set to 1 if this sample
	 * contains new data; set to 0 otherwise */
	short		valid;
};

enum ts_param {
	TS_SCREEN_RES = 0,		/* 2 integer args, x and y */
	TS_SCREEN_ROT			/* 1 integer arg, 1 = rotate */
};

/*
 * Close the touchscreen device, free all resources.
 */
TSAPI int ts_close(struct tsdev *);

/*
 * Reloads all modules - useful to reload calibration data.
 */
TSAPI int ts_reconfig(struct tsdev *);

/*
 * Configure the touchscreen device.
 */
TSAPI int ts_config(struct tsdev *);

/*
 * Changes a setting.
 */
TSAPI int ts_option(struct tsdev *, enum ts_param, ...);

/*
 * Change this hook to point to your custom error handling function.
 */
extern TSAPI int (*ts_error_fn)(const char *fmt, va_list ap);

/*
 * Returns the file descriptor in use for the touchscreen device.
 */
TSAPI int ts_fd(struct tsdev *);

/*
 * Load a filter/scaling module
 */
TSAPI int ts_load_module(struct tsdev *, const char *mod, const char *params);

/*
 * Open the touchscreen device.
 */
TSAPI struct tsdev *ts_open(const char *dev_name, int nonblock);

/*
 * Find, open and configure the touchscreen device.
 */
TSAPI struct tsdev *ts_setup(const char *dev_name, int nonblock);

/*
 * Return a scaled touchscreen sample.
 */
TSAPI int ts_read(struct tsdev *, struct ts_sample *, int);

/*
 * Returns a raw, unscaled sample from the touchscreen.
 */
TSAPI int ts_read_raw(struct tsdev *, struct ts_sample *, int);

/*
 * Return a scaled touchscreen multitouch sample.
 */
TSAPI int ts_read_mt(struct tsdev *, struct ts_sample_mt **, int slots, int nr);

/*
 * Return a raw, unscaled touchscreen multitouch sample.
 */
TSAPI int ts_read_raw_mt(struct tsdev *, struct ts_sample_mt **, int slots, int nr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _TSLIB_H_ */
