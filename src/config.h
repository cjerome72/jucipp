#ifndef JUCI_CONFIG_H_
#define JUCI_CONFIG_H_
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

class MainConfig {
public:
  MainConfig();
  void find_or_create_config_files();
  void PrintMenu();
  void GenerateSource();
  void GenerateKeybindings();
  void GenerateDirectoryFilter();
  void GenerateTerminalCommands();
private:
  boost::property_tree::ptree cfg;
  boost::property_tree::ptree key_tree;
};
#endif
