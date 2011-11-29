#ifndef BRIDGE_DEFINES_H
#define BRIDGE_DEFINES_H

#define LIB_EXPORT __attribute__ ((visibility("default")))

enum {
	OK            = 0,
	ErrorMem      = -1,
	ErrorConfig   = -2,
	ErrorVM       = -3,
	ErrorIO       = -4,
	ErrorNotfound = -5,
	ErrorInvalid  = -6
};

#endif
