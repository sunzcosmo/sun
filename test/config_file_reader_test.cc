#include "../config_file_reader.hpp"

#include <iostream>

using namespace sun;

int main(int argc, char **argv)
{
  auto cfg = new ConfigFileTini<ContentTini>(std::string("test.ini"));

  auto content = cfg->Read();

  std::cout<<"Finish Reading\n";

  for(auto& parameter : content->parameters) {
    std::cout<<"parameter: "<<parameter.key<<" = "<<parameter.val<<"\n";
  }

  return 0;
}
