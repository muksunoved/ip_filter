#pragma once

#include <iostream>

int version();

namespace helloworld {

class Version {

public:
   Version();

   int get_version() const { return version; }

private:
   int version;
};

}

std::ostream& operator<< (std::ostream& stream, const helloworld::Version& v);