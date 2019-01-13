#ifndef _CONFIG_FILE_READER_HPP_
#define _CONFIG_FILE_READER_HPP_

#include <cstdio>
#include <cstdlib>

#include <map>
#include <string>
#include <vector>

#include "sun_string.hpp"

namespace sun {

using std::vector;
using std::string;
using std::map;

template <typename T>
class ConfigFile {
public:
  ConfigFile() {}

  explicit ConfigFile(const string& path);

  ~ConfigFile();

  virtual T *Read() = 0;

protected:
  string file_path_;

  T *content_;
};

struct ContentTini {
  struct Parameter {
    string key;
    string val;
  };

  using sections_map = map<string, string>;

  map<string, sections_map> sections;

  inline int Insert(const string& section,
                    const string& key,
                    const string& val);

};

class ConfigFileTini : public ConfigFile<ContentTini> {
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

  ConfigFileTini(const string& path) : ConfigFile<ContentTini>(path) {}

  virtual ContentTini *Read();

private:
  LineAttr ParseLine(const char *line);

  string last_section_;

public:
  const static size_t MAX_PROPERTY_LEN = 1024;
};

// ContentTini
int ContentTini::Insert(const string& section,
                        const string& key,
                        const string& val)
{
  sections[section][key] = val;
  return 0;
}

// ConfigFile<T>
template <typename T>
ConfigFile<T>::ConfigFile(const string& path) : file_path_(path),
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
ConfigFileTini::LineAttr ConfigFileTini::ParseLine(const char *line)
{
  char data[MAX_PROPERTY_LEN] = {};
  size_t data_size = sun_trim(line, sun_strlen(line), data);

  // Empty line.
  if(0 == data_size) {
    return EMPTY;
  }

  // Comment
  if(';' == data[0]) {
    return COMMENT;
  }

  if('[' == data[0] && ']' == data[data_size - 1]) {
    last_section_ = string(data + 1, data_size - 2);
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

  char key[MAX_PROPERTY_LEN] = {};
  int key_size = sun_trim(data, equal_sign_index, key);
  if(key_size < 1) {
    return EMPTY_KEY;
  }

  char val[MAX_PROPERTY_LEN] = {};
  int val_size = sun_trim(data + equal_sign_index + 1, data_size, val);
  if(val_size < 1) {
    return EMPTY_VAL;
  }

  this->content_->Insert(last_section_, key, val);

  return PARAMETER;
}

ContentTini *ConfigFileTini::Read()
{
  // TODO
  // File lock.
  FILE *file = fopen(this->file_path_.c_str(), "r");
  if(nullptr == file) {
    return nullptr;
  }
  char line[MAX_PROPERTY_LEN] = {};
  while(fgets(line, MAX_PROPERTY_LEN * 2, file)) {
    ParseLine(line);
  }
  fclose(file);
  return this->content_;
}

} // namespace sun

#endif // _CONFIG_FILE_READER_HPP_
