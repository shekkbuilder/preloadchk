#include <link.h>

using UnknownStruct = struct unknown_struct {
   void*  pointers[3];
   struct unknown_struct* ptr;
};
using LinkMap = struct link_map;

auto* handle = dlopen(NULL, RTLD_NOW);
auto* p = reinterpret_cast<UnknownStruct*>(handle)->ptr;
auto* map = reinterpret_cast<LinkMap*>(p->ptr);

while (map) {
  std::cout << map->l_name << std::endl;
  // do something with |map| like with handle, returned by |dlopen()|.
  map = map->l_next;
}
