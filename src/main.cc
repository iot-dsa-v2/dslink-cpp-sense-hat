#include "dsa/responder.h"
#include "dslink.h"

#include "sense_hat.h"
#include <iostream>

using namespace dsa;
using namespace std;

class SenseHatNodeChild : public NodeModel {
public:
  explicit SenseHatNodeChild(LinkStrandRef strand)
      : NodeModel(std::move(strand)) {
    update_property("$type", Var("string"));
    update_property("@attr", Var("test attribute value"));
    set_value(Var("test string value 1"));
  };
};

class SenseHatNodeMain : public NodeModel {
public:
  explicit SenseHatNodeMain(LinkStrandRef strand)
      : NodeModel(std::move(strand), PermissionLevel::WRITE) {
    update_property("$type", Var("string"));
    add_list_child("child_a", new SenseHatNodeChild(_strand));
    add_list_child("child_b", new SenseHatNodeChild(_strand));
  };
};

int main(int argc, const char *argv[]) {
  init_sense_hat();
  auto link = make_ref_<DsLink>(argc, argv, "mydslink", "1.0.0");
  link->init_responder<SenseHatNodeMain>();
  // link->run();

  // add a callback when connected to broker
  link->run([&](const shared_ptr_<Connection> connection) {

  });

  close_sense_hat();
  return 0;
}
