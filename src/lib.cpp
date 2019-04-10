#include "lib.h"

#include "version.h"


namespace helloworld {

Version::Version() : version(PROJECT_VERSION_PATCH)  {
}

}

std::ostream& operator<< (std::ostream& stream, const helloworld::Version& v)  {
   stream << v.get_version();
   return stream;
}