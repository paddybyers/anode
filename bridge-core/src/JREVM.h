#ifndef BRIDGE_JREVM_H
#define BRIDGE_JREVM_H

#include "VM.h"
#include <uv.h>

class JREVM : public VM {
public:
	JREVM();
	virtual ~JREVM();
	static int static_init();

private:
	static char   *buildClasspath(const char *anodeRoot, size_t len);
	static char   *buildLibrarypath(const char *anodeRoot, size_t len);
	static int     createVM();
	int            attach();
};

#endif
