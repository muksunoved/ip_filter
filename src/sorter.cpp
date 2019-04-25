#include <algorithm>
#include <numeric>
#include <future>
#include <cassert>

#include <sorter.h>


namespace sorter {

using namespace std;

std::map<FilterPolicy, std::function<bool(const FilterBase& f, const IpT&)>>  FilterBase::declared_filters_ = {
		{FilterPolicy::kNothing, [](const FilterBase& f, const IpT& in)->bool {return true;} },
		{FilterPolicy::kFirstOf, [](const FilterBase& f, const IpT& in)->bool {
		    assert(f.args_base_.size() > 0);

		    auto& [in_ip1, in_ip2, in_ip3, in_ip4 ] = in;
		    auto inv = std::vector<int>{in_ip1, in_ip2, in_ip3, in_ip4 };
		    bool result = true;

		    for (int i=0; i<inv.size() && i<f.args_base_.size(); i++)  {
		        result = result && (inv[i] == f.args_base_[i]);
		    }
		    return result;

		} },
		{FilterPolicy::kAnyOf, [](const FilterBase& f, const IpT& in)->bool {
		    assert(f.args_base_.size() > 0);

			auto& f_ip1 = f.args_base_[0];
			auto& [in_ip1, in_ip2, in_ip3, in_ip4 ] = in;

			if (f_ip1 == in_ip1
					|| f_ip1 == in_ip2
					|| f_ip1 == in_ip3
					|| f_ip1 == in_ip4 ) {
				return true;
			}
			return false;
		} }
};

Sorter::Sorter(const std::vector<Filter>& filters)
	: filters_(filters)  {
}

void Sorter::SortIps()  {
	vector<future<void>> sorts_pull;

	for (auto& v_it : vs_after_filtering_)  {
		sorts_pull.push_back(async(launch::async, [&]() {
			std::sort(v_it.second.begin(), v_it.second.end(), std::greater<IpT>());
		}));
	}
	for (auto& s : sorts_pull)  {
		s.get();
	}
}

std::istream& Sorter::operator>>(std::istream& is )  {
	using namespace std;
	auto ip = make_tuple(0,0,0,0);

	try {
		is >> ws;
		auto& [ip1, ip2, ip3, ip4] = ip;
		int order;

		for(std::string line; std::getline(is, line, '\n');)  {
			auto result = std::sscanf(line.c_str(), "%03d.%03d.%03d.%03d\n", &ip1, &ip2, &ip3, &ip4);
			if (result != 4)  {
				throw (std::runtime_error("Fail parse string"));
			}
			v_base_.push_back(ip);
			is >> ws;
		}
		order = 0;
		for (auto& f : filters_)  {
		    vs_after_filtering_[order].reserve(v_base_.size());
		    order++;
		}
		for (auto &ip : v_base_)  {
		    order = 0;
		    for (auto f : filters_)  {
		        if (f.filter(ip))  {
		            vs_after_filtering_[order].push_back(ip);
		        }
		    order++;
		    }
		}

	} catch (std::runtime_error& e)  {
		std::cout << "Fail parse IP table : " << std::string(e.what()) << endl;
		throw(e);
	}

	return is;
}


std::ostream& operator<<(std::ostream& os, const IpT& ipt)  {
	os << get<0>(ipt) <<"."<< get<1>(ipt) << "." << get<2>(ipt) << "." << get<3>(ipt);
	return os;
}

std::ostream& Sorter::operator<<(std::ostream& os)  {
	for (auto& vs : vs_after_filtering_)  {
		for_each(vs.second.begin(), vs.second.end(), [&](IpT& ip) {
			os << ip << "\n";
		});
	}
	return os;
}

std::istream& operator>>(std::istream& is,  sorter::Sorter& s) {
	return s.Sorter::operator>>(is);
}
std::ostream& operator<<(std::ostream& os,  sorter::Sorter& s)  {
	return s.Sorter::operator<<(os);
}


}


