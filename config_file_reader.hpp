#ifndef _CONFIG_FILE_READER_HPP_
#define _CONFIG_FILE_READER_HPP_

#include <cstdio>
#include <cstdlib>

#include <map>
#include <string>
#include <vector>

namespace sun {

using std::vector;
using std::string;
using std::map;

extern "C" {
// Return the length of final target string.
int sun_trim(const char *src, const size_t src_size, char *target);

size_t sun_strlen(const char *str);

} // extern "C"

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

  inline string GetParameter(const string& section, const string& key) const;

  map<string, vector<int>> sections;

  vector<Parameter> parameters;
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

  ContentTini *Read();

private:
  LineAttr ParseLine(const char *line);

  string last_section_;

public:
  const static size_t MAX_PROPERTY_LEN = 1024;
};

extern "C" {

int sun_trim(const char *src, const size_t src_size, char *target)
{
  size_t sfront = 0;
  for(; sfront < src_size && (' ' == src[sfront] || '\t' == src[sfront] || '\n' == src[sfront]); ++sfront) {}

  size_t srear = src_size - 1;
  if(srear > sfront) {
    for(; ' ' == src[srear] || '\n' == src[srear] || '\t' == src[srear]; --srear) {}
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

//ContentTini
string ContentTini::GetParameter(const string& section, const string& in_key) const {
  auto sec = sections.find(section);
  if(sections.end() == sec) {
    return "";
  }
  for(auto& i : sec->second) {
    if(parameters[i].key == in_key) {
      return parameters[i].val;
    }
  }
  return "";
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
typename ConfigFileTini::LineAttr ConfigFileTini::ParseLine(const char *line)
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
    if(this->content_->sections.find(last_section_) == this->content_->sections.end()) {
      this->content_->sections.insert(make_pair(last_section_, vector<int>()));
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

  this->content_->sections[last_section_].push_back(this->content_->parameters.size());
  this->content_->parameters.push_back({string(key), string(val)});

  return PARAMETER;
}

ContentTini *ConfigFileTini::Read()
{
  // TODO
  // File lock does not seems neccesery.
  FILE *file = fopen(this->file_path_.c_str(), "r");
  if(nullptr == file) {
    return nullptr;
  }
  char line[MAX_PROPERTY_LEN] = {};
  while(fgets(line, MAX_PROPERTY_LEN, file)) {
    ParseLine(line);
  }
  fclose(file);
  return this->content_;
}

} // namespace sun

#endif // _CONFIG_FILE_READER_HPP_
