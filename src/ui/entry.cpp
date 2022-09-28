
#include <lvtk/ui/entry.hpp>

namespace lvtk {
namespace detail {
class Entry {
public:
    Entry (lvtk::Entry& o) : owner (o) {}

private:
    friend class lvtk::Entry;
    lvtk::Entry& owner;
};

}

Entry::Entry() {
    impl = std::make_unique<detail::Entry> (*this);
}

Entry::~Entry() {
    impl.reset();
}

}
