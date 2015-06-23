#ifndef SINGA_COMMUNICATION_MSG_H_
#define SINGA_COMMUNICATION_MSG_H_

#include <czmq.h>
#include <glog/logging.h>
#include <algorithm>
#include <string>

namespace singa {

#define USE_ZMQ

class Msg {
 public:
  Msg();
  ~Msg();

  /**
    * @param first worker/server group id
    * @param second worker/server id within the group
    * @param flag 0 for server, 1 for worker, 2 for stub
    */
  inline void set_src(int first, int second, int flag) {
    src_ = (first << kOff1) | (second << kOff2) | flag;
  }
  inline void set_dst(int first, int second, int flag) {
    dst_ = (first << kOff1) | (second << kOff2) | flag;
  }
  inline void set_src(int procs_id, int flag) { set_src(procs_id, 0, flag); }
  inline void set_dst(int procs_id, int flag) { set_dst(procs_id, 0, flag); }
  inline int src() const { return src_; }
  inline int dst() const { return dst_; }
  inline int src_first() const { return src_ >> kOff1; }
  inline int dst_first() const { return dst_ >> kOff1; }
  inline int src_second() const { return (src_ & kMask1) >> kOff2; }
  inline int dst_second() const { return (dst_ & kMask1) >> kOff2; }
  inline int src_flag() const { return src_&kMask2; }
  inline int dst_flag() const { return dst_&kMask2; }
  inline void SwapAddr() { std::swap(src_, dst_); }
  inline void set_type(int type) { type_ = type; }
  inline int type() const { return type_; }
  inline void set_target(int first, int second) {
    target_first_ = first;
    target_second_ = second;
  }
  inline int target_first() const { return target_first_; }
  inline int target_second() const { return target_second_; }
  /**
   * Copy src and dst address, including first, id, flag
   */
  inline Msg* CopyAddr() {
    Msg* msg = new Msg();
    msg->src_ = src_;
    msg->dst_ = dst_;
    return msg;
  }
  inline void SetAddr(Msg* msg) {
    src_ = msg->src_;
    dst_ = msg->dst_;
  }

  /**
   * Add a frame (a chunck of bytes) into the message
   */
  void add_frame(const void* addr, int nBytes);
  int frame_size();
  void* frame_data();
  /**
    * Move the cursor to the next frame
    * @return true if the next frame is not NULL; otherwise false
    */
  bool next_frame();
#ifdef USE_ZMQ
  void ParseFromZmsg(zmsg_t* msg);
  zmsg_t* DumpToZmsg();
#endif

 protected:
  static const unsigned int kOff1 = 16;
  static const unsigned int kOff2 = 4;
  static const unsigned int kMask1 = (1 << kOff1) - 1;
  static const unsigned int kMask2 = (1 << kOff2) - 1;

  int src_ = 0;
  int dst_ = 0;
  int type_ = 0;
  int target_first_ = 0;
  int target_second_ = 0;
#ifdef USE_ZMQ
  zmsg_t* msg_ = nullptr;
  zframe_t *frame_ = nullptr;
#endif
};

inline void DeleteMsg(Msg** msg) {
  delete *msg;
  *msg = nullptr;
}

}  // namespace singa

#endif  // SINGA_COMMUNICATION_MSG_H_