#include "KqueueHandler.hpp"

KqueueHandler::KqueueHandler(void) {
  kq_ = kqueue();
  if (kq_ < 0) {
    throw std::runtime_error("kqueue failed");
  }
}

KqueueHandler::~KqueueHandler(void) {
  if (kq_ > 0) {
    close(kq_);
  }
}
