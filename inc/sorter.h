#pragma once

#include <iostream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

namespace sorter {

using IpTuples = std::vector<std::tuple<int,int,int,int>>;
using IpT = std::tuple<int,int,int,int>;


//! Simple filters logical policy
enum class FilterPolicy {
	kNothing,
	kFirstOf,
	kTwoFirstDigits,
	kAnyOf,
};

//! Represent of one iteration data filter
/**
 *
 */
class Filter {
public:
	Filter();
	Filter(const FilterPolicy& policy, const IpT& filter);
	~Filter() = default;

	bool filter(const IpT& ip_in) ;

	FilterPolicy get_policy() const { return policy_; }
	const IpT& get_filter_value() const { return filter_; }
private:
	FilterPolicy policy_{FilterPolicy::kNothing};
	IpT filter_{0,0,0,0};
	static std::map<FilterPolicy, std::function<bool(const Filter& f, const IpT&)>> declared_filters_;
};

//! Represent input data, filter, sort and output for stream
/**
 *
 */
class Sorter {
public:
	Sorter() = delete;
	Sorter(const std::vector<Filter>& filters);
	virtual ~Sorter() = default;

	void SortIps();

	friend std::istream& operator>>(std::istream& is, sorter::Sorter& s);
	friend std::ostream& operator<<(std::ostream& os, sorter::Sorter& s);

	std::istream& operator>>(std::istream& is);
	std::ostream& operator<<(std::ostream& os);
protected:
	const std::vector<Filter>& filters_;
	IpTuples v_base_;
	std::map<FilterPolicy, IpTuples> vs_after_filtering_;

};




}
