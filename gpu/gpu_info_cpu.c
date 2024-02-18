#include "gpu_info.h"
// Fallbacks for CPU mode

#ifdef _WIN32
#include <sysinfoapi.h>
void cpu_check_ram(mem_info_t *resp) {
  resp->err = NULL;
  MEMORYSTATUSEX info;
  info.dwLength = sizeof(info);
  if (GlobalMemoryStatusEx(&info) != 0) {
    resp->count = 1;
    resp->total = info.ullTotalPhys;
    resp->free = info.ullAvailPhys;
  } else {
    resp->err = LOAD_ERR();
  }
  return;
}

#elif __linux__
#include <errno.h>
#include <string.h>
#include <sys/sysinfo.h>
void cpu_check_ram(mem_info_t *resp) {
  struct sysinfo info;
  resp->err = NULL;
  if (sysinfo(&info) != 0) {
    resp->err = strdup(strerror(errno));
  } else {
    resp->count = 1;
    resp->total = info.totalram * info.mem_unit;
    resp->free = info.freeram * info.mem_unit;
  }
  return;
}

#elif __APPLE__
// TODO consider an Apple implementation that does something useful
// mem_info_t cpu_check_ram() {
//   mem_info_t resp = {0, 0, NULL};
//   return resp;
// }
#elif __OpenBSD__
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <string.h>
#include <errno.h>
void cpu_check_ram(mem_info_t *resp) {
  int mib[2] = {CTL_VM, VM_UVMEXP};
  struct uvmexp uvmexp;
  size_t len = sizeof uvmexp;

  if(sysctl(mib, 2, &uvmexp, &len, NULL, 0) < 0) {
	resp->err = strdup(strerror(errno));
  } else {
	resp->err   = NULL;
	resp->count = 1;
	resp->total = uvmexp.pagesize * uvmexp.npages;
	resp->free  = uvmexp.pagesize * uvmexp.free; /* should we count inactive pages as well? */
  }
}
#else
#error "Unsupported platform"
#endif
