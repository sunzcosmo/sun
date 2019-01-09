#ifndef _CONFIG_FILE_READER_HPP_
#define _CONFIG_FILE_READER_HPP_

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <map>
#include <string>
#include <vector>

namespace sun {

extern "C" {

// Return the length of final target string.
int trim(const char *src, const size_t src_size, char *target);

} // extern "C"

template <typename T>
class ConfigFile {
public:
  ConfigFile() {}

  explicit ConfigFile(const std::string& path);

  ~ConfigFile();

  virtual T *Read() = 0;

protected:
  std::string file_path_;

  T *content_;
};

struct ContentTini {
  struct Parameter {
    std::string key;
    std::string val;
  };
  std::map<std::string, std::vector<int>> sections;
  std::vector<Parameter> parameters;
};

template <typename T>
class ConfigFileTini : public ConfigFile<T> {
public:
  enum LineAttr {
    EMPTY,
    COMMENT,
    WRONG_SYNTAX,
    EMPTY_KEY,
    EMPTY_VAL,
    SECTION,
    PARAMETER,
    SIZE,
  };

  ConfigFileTini<T>(const std::string& path) : ConfigFile<T>(path) {}

  T *Read();

private:
  LineAttr ParseLine(const char *line);
  std::string last_section_;
};

extern "C" {

int trim(const char *src, const size_t src_size, char *target)
{
  size_t sfront = 0;
  for(; sfront < src_size && ' ' == src[sfront]; ++sfront) {}

  size_t srear = src_size - 1;
  if(srear > sfront) {
    for(; ' ' == src[srear] || '\n' == src[srear]; --srear) {}
  }

  size_t i = 0;
  for(; sfront <= srear; ++sfront, ++i) {
    target[i] = src[sfront];
  }
  target[i] = '\0';

  return i;
}

} // extern "C"

template <typename T>
ConfigFile<T>::ConfigFile(const std::string& path) : file_path_(path),
                                                     content_(new T)
{
}

template <typename T>
ConfigFile<T>::~ConfigFile()
{
  if(content_) {
    delete content_;
  }
}

// ConfigFileTini
template <typename T>
typename ConfigFileTini<T>::LineAttr ConfigFileTini<T>::ParseLine(const char *line)
{
  char data[1024] = {};
  size_t data_size = trim(line, strlen(line), data);

  // Empty line.
  if(0 == data_size) {
    return EMPTY;
  }

  // Comment
  if(';' == data[0]) {
    return COMMENT;
  }

  if('[' == data[0] && ']' == data[data_size - 1]) {
    last_section_ = std::string(data + 1, data_size - 2);
    if(ConfigFile<T>::content_->sections.find(last_section_) == ConfigFile<T>::content_->sections.end()) {
      ConfigFile<T>::content_->sections.insert(make_pair(last_section_, std::vector<int>()));
    }
    return SECTION;
  }

  if(last_section_.empty()) {
    return WRONG_SYNTAX;
  }

  size_t equal_sign_index = 0;
  for(;
      equal_sign_index < data_size && '=' != data[equal_sign_index];
      ++equal_sign_index) {
  }

  if(data_size == equal_sign_index) {
    return WRONG_SYNTAX;
  }

  char key[1024] = {};
  int key_size = trim(line, equal_sign_index, key);
  if(key_size < 1) {
    return EMPTY_KEY;
  }

  char val[1024] = {};
  int val_size = trim(line + equal_sign_index + 1, data_size, val);
  if(val_size < 1) {
    return EMPTY_VAL;
  }

  ConfigFile<T>::content_->sections[last_section_].push_back(ConfigFile<T>::content_->parameters.size());
  ConfigFile<T>::content_->parameters.push_back({std::string(key), std::string(val)});

  return PARAMETER;
}

template <typename T>
T *ConfigFileTini<T>::Read()
{
  FILE *file = fopen(this->file_path_.c_str(), "r");
  if(nullptr == file) {
    return nullptr;
  }
  char line[1024] = {};
  while(fgets(line, 1024, file)) {
    ParseLine(line);
  }
  return this->content_;
}

} // namespace sun

#endif // _CONFIG_FILE_READER_HPP_
