#ifndef _SUN_STRING_HPP_
#define _SUN_STRING_HPP_

#include "sun_config.hpp"

namespace sun {

extern "C" {
// Return the length of final target string.
int sun_trim(const char *src, const size_t src_size, char *target);

size_t sun_strlen(const char *str);

int sun_trim(const char *src, const size_t src_size, char *target)
{
  size_t sfront = 0;
  for(; 
      sfront < src_size && (' ' == src[sfront] || '\t' == src[sfront] || '\n' == src[sfront]); 
      ++sfront) {
  }

  size_t srear = src_size - 1;
  for(; 
      srear > sfront && (' ' == src[srear] || '\n' == src[srear] || '\t' == src[srear]); 
      --srear) {
  }

  size_t i = 0;
  for(; sfront <= srear; ++sfront, ++i) {
    target[i] = src[sfront];
  }
  target[i] = '\0';

  return i;
}

size_t sun_strlen(const char *str) {
  const char *curr = str;
  for(; *curr; ++curr) {}
  return curr - str;
}

} // extern "C"

class SunString {
private:
  u_char *data_;

  size_t size_;

public:
  inline size_t size();

  inline u_char *c_str();
  
}; // class SunString

} // namespace sun

#endif // _SUN_STRING_HPP_ 
