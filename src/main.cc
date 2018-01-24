#include "dsa/responder.h"
#include "dslink.h"

#include "sense_hat.h"
#include <iostream>

using namespace dsa;
using namespace std;

class SenseHatNodeMain : public NodeModel {

public:
  static inline unsigned short str2rgb565(const string_ color) {
    if (color == "red") {
      return 0xF800;
    } else if (color == "green") {
      return 0x07E0;
    } else if (color == "blue") {
      return 0x001F;
    } else if (color == "yellow") {
      return 0xFFE0;
    } else if (color == "purple") {
      return 0xF81F;
    } else if (color == "white") {
      return 0xFFFF;
    } else {
      return 0;
    }
  };

  ref_<NodeModel> value_node;
  ref_<SimpleInvokeNode> acion_node;

  explicit SenseHatNodeMain(LinkStrandRef strand)
      : NodeModel(std::move(strand), PermissionLevel::WRITE) {
    acion_node = make_ref_<SimpleInvokeNode>(
        _strand->get_ref(), [ this, kee_ref = get_ref() ](Var && v) {
          if (v.is_map() && v.get_map().count("color") &&
              v.get_map()["color"].is_string()) {
            const string_ &color = v.get_map()["color"].get_string();
            render(str2rgb565(color));
            acion_node->set_value(Var(v.get_map()["color"]));
          }
          return Var(static_cast<int64_t>(MessageStatus::INVALID_PARAMETER));
        });
    value_node =
        make_ref_<NodeModel>(_strand->get_ref(), PermissionLevel::WRITE);
    value_node->update_property("$type", Var("string"));
    add_list_child("value", value_node->get_ref());
    add_list_child("change", acion_node->get_ref());
  };
};

int main(int argc, const char *argv[]) {
  init_sense_hat();
  render(0);
  auto link = make_ref_<DsLink>(argc, argv, "mydslink", "1.0.0");
  link->init_responder<SenseHatNodeMain>();
  link->run();

  close_sense_hat();
  return 0;
}
