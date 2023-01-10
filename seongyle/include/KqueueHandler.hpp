#ifndef KQUEUE_HANDLER_HPP
#define KQUEUE_HANDLER_HPP

#include <sys/event.h>

#include <vector>

class KqueueHandler {
 public:
  KqueueHandler(void);
  virtual ~KqueueHandler(void);

 private:
  int kq_;
  std::vector<struct kevent> events_;
};

#endif