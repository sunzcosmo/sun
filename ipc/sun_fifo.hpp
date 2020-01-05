#ifndef _SUN_FIFO_HPP_
#define _SUN_FIFO_HPP_

#include "../sun_util.hpp"

#include <fstream>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace sun {

class FIFO {
public:

	FIFO() = delete;

	FIFO(const char *file_name);

	~FIFO();

private:
	std::string fifo_name;

public:
	int create(int access = 0600);

	int open(int flags);

};

FIFO::FIFO(const char *fifo_name) : fifo_name(fifo_name) {
}

FIFO::~FIFO() {
  unlink(this->fifo_name.c_str());
}

int FIFO::create(int access) {
	std::fstream fs(fifo_name);
	if(fs.good()) {
		unlink(this->fifo_name.c_str());
	}
  return mknod(fifo_name.c_str(), S_IFIFO | access, 0);
}

extern "C" {


} // extern "C"

} // namespace sun

#endif // _SUN_FIFO_HPP_
