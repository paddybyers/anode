#ifndef BRIDGE_JREVM_H
#define BRIDGE_JREVM_H

#include "VM.h"
#include <uv.h>

namespace bridge {

class JREVM : public VM {
public:
	JREVM();
	virtual ~JREVM();
	static int initProcess();
  virtual int createContext(jobject jEnv, jobject jExports, jobject *jCtx);

private:
	static char   *buildClasspath(const char *anodeRoot, size_t len);
	static char   *buildLibrarypath(const char *anodeRoot, size_t len);
	static int     createVM();
	int            attach();
};

} // namespace bridge
#endif
