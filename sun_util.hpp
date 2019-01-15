#ifndef _SUN_UTIL_HPP_
#define _SUN_UTIL_HPP_

namespace sun {
  class noncopyable
  {
  public:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;
  
  protected:
  noncopyable() = default;
  ~noncopyable() = default;
  };
}


#endif // _SUN_UTIL_HPP_