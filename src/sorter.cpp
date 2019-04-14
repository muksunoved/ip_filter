#include <algorithm>
#include <numeric>
#include <future>

#include <sorter.h>


namespace sorter {

using namespace std;

std::map<FilterPolicy, std::function<bool(const Filter& f, const IpT&)>>  Filter::declared_filters_ = {
		{FilterPolicy::kNothing, [](const Filter& f, const IpT& in)->bool {return true;} },
		{FilterPolicy::kFirstOf, [](const Filter& f, const IpT& in)->bool {
			if (get<0>(f.get_filter_value()) == get<0>(in)) {
				return true;
			}
			return false;
		} },
		{FilterPolicy::kTwoFirstDigits, [](const Filter& f, const IpT& in)->bool {
			auto& [f_ip1, f_ip2, f_ip3, f_ip4 ] = f.get_filter_value();
			auto& [in_ip1, in_ip2, in_ip3, in_ip4 ] = in;

			if (f_ip1 == in_ip1 && f_ip2 == in_ip2) {
				return true;
			}
			return false;
		} },
		{FilterPolicy::kAnyOf, [](const Filter& f, const IpT& in)->bool {
			auto& [f_ip1, f_ip2, f_ip3, f_ip4 ] = f.get_filter_value();
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

Filter::Filter() {
}

Filter::Filter(const FilterPolicy& policy, const IpT& filter) : Filter()  {
	policy_ = policy;
	filter_ = filter;
}

bool Filter::filter(const std::tuple<int,int,int,int>& ip_in)  {
	auto f_it = declared_filters_.find(policy_);
	if (f_it != declared_filters_.end())  {
		return f_it->second(*this, ip_in);
	}
	return false;
}

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


