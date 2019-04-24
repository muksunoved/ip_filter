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

			if (f.args_base_[0] == get<0>(in)) {
				return true;
			}
			return false;
		} },
		{FilterPolicy::kTwoFirstDigits, [](const FilterBase& f, const IpT& in)->bool {
		    assert(f.args_base_.size() > 1);

			auto& f_ip1 = f.args_base_[0];
			auto& f_ip2 = f.args_base_[1];
			auto& [in_ip1, in_ip2, in_ip3, in_ip4 ] = in;

			if (f_ip1 == in_ip1 && f_ip2 == in_ip2) {
				return true;
			}
			return false;
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
    v_base_.reserve(2048);
	vs_after_filtering_[FilterPolicy::kNothing].reserve(2048);
	vs_after_filtering_[FilterPolicy::kFirstOf].reserve(2048);
	vs_after_filtering_[FilterPolicy::kTwoFirstDigits].reserve(2048);
	vs_after_filtering_[FilterPolicy::kAnyOf].reserve(2048);
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
		for(std::string line; std::getline(is, line, '\n');)  {
			auto result = std::sscanf(line.c_str(), "%03d.%03d.%03d.%03d\n", &ip1, &ip2, &ip3, &ip4);
			if (result != 4)  {
				throw (std::runtime_error("Fail parse string"));
			}
			v_base_.push_back(ip);
			for (auto f : filters_)  {
				if (f.filter(ip))  {
					vs_after_filtering_[f.get_policy()].push_back(ip);
				}
			}
			is >> ws;
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


