#include "lib.h"
#include <sorter.h>

using namespace sorter;

int main()
{
    const std::vector<Filter> kFilter{
    	Filter{FilterPolicy::kNothing, 		  IpT{0,0,0,0}},
		Filter{FilterPolicy::kFirstOf, 		  IpT{1,0,0,0}},
    	Filter{FilterPolicy::kTwoFirstDigits, IpT{46,70,0,0}},
		Filter{FilterPolicy::kAnyOf,          IpT{46,46,46,46}}
    } ;
    try {
		Sorter s{kFilter};

		std::cin >> s;
		s.SortIps();
		std::cout << s;

    }  catch (std::runtime_error& e)  {
    	std::cout << "Unhandled exception " << std::string(e.what());
    	exit(1);
    }
}
