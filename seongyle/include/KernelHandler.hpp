#ifndef KernelHandler_HPP
#define KernelHandler_HPP

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

class KernelHandler {
 public:
  KernelHandler(void);
  virtual ~KernelHandler(void);

 private:
  int kq_;
  struct kevent events_;
};

#endif