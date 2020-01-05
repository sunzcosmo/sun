#ifndef SUN_PIPE_HPP
#define SUN_PIPE_HPP

#include "../sun_util.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>

namespace sun {

extern "C" {

}

class Pipe : public noncopyable {
public:
	Pipe();

	~Pipe();

private:
	// read: pipe[0], write: pipe[1];
	int pipe[2];

public:
	int open(int flags = O_CLOEXEC);

	int close_read();

	int close_write();

	int read(char *output, int len);

	int write(const char *str);

private:
	int close_fd(int fd);

}; // class Pipe

Pipe::Pipe() : pipe{-1, -1} {

}

Pipe::~Pipe() {
	close_read();
	close_write();
}

int Pipe::open(int flags) {
	return ::pipe2(this->pipe, flags);
}

int Pipe::close_read() {
	return close_fd(this->pipe[0]);
}

int Pipe::close_write() {
	return close_fd(this->pipe[1]);
}

int Pipe::read(char *output, int len) {
	if(len > PIPE_BUF) {
		return -1;
	}
	return ::read(pipe[0], output, len);
}

int Pipe::write(const char *str) {
	int len = strlen(str);
	if(len > PIPE_BUF) {
		return -1;
	}
  return ::write(pipe[1], str, len);
}

int Pipe::close_fd(int fd) {
  return -1 == fd ? 0 : close(fd);
}

} // namespace sun

#endif // SUN_PIPE_HPP
