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
#include <memory>

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

//! Represent of base data filter
/**
 *
 */
class FilterBase {
public :
    FilterBase(const FilterPolicy& pol) { policy_=pol; }
    virtual ~FilterBase() = default;

    virtual bool filter(const IpT& ip_in) = 0;
    FilterPolicy get_policy() const { return policy_; }

protected:
    FilterPolicy policy_{FilterPolicy::kNothing};

protected:
    static std::map<FilterPolicy, std::function<bool(const FilterBase& f, const IpT&)>> declared_filters_;
public:
    std::vector<int> args_base_;
};

//! Represent of one iteration data filter
/**
 *
 */
class Filter : public FilterBase {
public:
    using FilterBase::FilterBase;

    Filter() = delete;
    ~Filter() = default;

    template<typename T>
    void push_back_to_args(const T& arg)  {
        args_base_.push_back(arg);
    }
    template<typename T, typename... Args>
    void push_back_to_args(const T&  arg, Args... args)  {
        args_base_.push_back(arg);
        push_back_to_args(args...);
    }
    template<typename... Args>
    Filter(const FilterPolicy& pol, Args... args) : FilterBase(pol) {
        policy_ = pol;
        push_back_to_args(args...);
    }
    bool filter(const IpT& ip_in) {
        auto f_it = declared_filters_.find(policy_);
	    if (f_it != declared_filters_.end())  {
	        return f_it->second(*this, ip_in);
	    }
	    return false;
	};
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
