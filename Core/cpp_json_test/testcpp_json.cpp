#include "testcpp_json.h"
#include "json/json.h"
#include <iostream>
#include <memory>
/** \brief Write the Value object to a stream.
 * Example Usage:
 * $g++ streamWrite.cpp -ljsoncpp -std=c++11 -o streamWrite
 * $./streamWrite
 * {
 *     "Age" : 20,
 *     "Name" : "robin"
 * }
 */
int maincpp() {
  Json::Value root;
  Json::StreamWriterBuilder builder;
  const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

  root["Name"] = "robin";
  root["Age"] = 20;
  writer->write(root, &std::cout);

  return EXIT_SUCCESS;
}
