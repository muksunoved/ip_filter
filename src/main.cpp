#include "lib.h"
#include <sorter.h>

using namespace sorter;

int main()
{
    const auto kFilters = std::vector<Filter>{
    	{Filter{FilterPolicy::kNothing               }},
		{Filter{FilterPolicy::kFirstOf,        1     }},
    	{Filter{FilterPolicy::kTwoFirstDigits, 46, 70}},
		{Filter{FilterPolicy::kAnyOf,          46    }}
    } ;

    try {
        Sorter s(kFilters);

		std::cin >> s;
		s.SortIps();
		std::cout << s;

    }  catch (std::runtime_error& e)  {
    	std::cout << "Unhandled exception " << std::string(e.what());
    	exit(1);
    }
}
