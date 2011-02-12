#include <v8.h>
#include <node.h>
#include <git2.h>

using namespace v8;
using namespace node;

class Git2 : ObjectWrap {
    private:
        int m_count;
    public:
};
