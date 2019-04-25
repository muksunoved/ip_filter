#include <gtest/gtest.h>

#include <sorter.h>

using namespace sorter;
using namespace ::testing;
using namespace std;

static const std::vector<Filter> kFilterDefault{Filter{FilterPolicy::kNothing}} ;

class MockSorter : public Sorter {
public:


	MockSorter(const std::vector<Filter>& filters  = kFilterDefault ) : Sorter{filters}  {}
	~MockSorter() = default;
	const IpTuples& get_v() const { return v_base_;}

	std::map<int, IpTuples>& get_v_filtering()  {
		return vs_after_filtering_;
	}

};

std::stringstream& operator>>(std::stringstream& is, MockSorter& s)  {
		(s.operator >>(is));
		return is;
}

std::stringstream& operator<<(std::stringstream& os, MockSorter& s)  {
		(s.operator<<(os));
		return os;
}

class SorterTest : public ::testing::Test {
public :
	void SetUp() override {

	}
	void TearDown() override {
	}
};

TEST_F(SorterTest, ShouldGetAllIpNumbers) {
	MockSorter s{};
	std::stringstream is("01.002.003.4\n");
	is >> s;
	EXPECT_EQ(get<0>(s.get_v()[0]), 1);
	EXPECT_EQ(get<1>(s.get_v()[0]), 2);
	EXPECT_EQ(get<2>(s.get_v()[0]), 3);
	EXPECT_EQ(get<3>(s.get_v()[0]), 4);
}

TEST_F(SorterTest, ShouldGetAllIpNumbersForMoreRecords) {
	MockSorter s{};
	std::stringstream is("01.002.003.4\n02.03.005.255\n055.53.004.01\t sss\t ddd\n");
	is >> s;
	EXPECT_EQ(get<0>(s.get_v()[0]), 1);
	EXPECT_EQ(get<1>(s.get_v()[0]), 2);
	EXPECT_EQ(get<2>(s.get_v()[0]), 3);
	EXPECT_EQ(get<3>(s.get_v()[0]), 4);

	EXPECT_EQ(get<0>(s.get_v()[2]), 55);
	EXPECT_EQ(get<1>(s.get_v()[2]), 53);
	EXPECT_EQ(get<2>(s.get_v()[2]), 4);
	EXPECT_EQ(get<3>(s.get_v()[2]), 1);
}

TEST_F(SorterTest, ShouldFilterIpsWitchCertainBytesToFilteredArrays) {
	MockSorter s{};
	std::stringstream is("01.002.003.4\t sss \t ddd \n02.03.005.255\t sdsdsd \t fgfgfg \n055.53.004.01\t ererre \t fgfg \n");
	is >> s;
	EXPECT_EQ(get<0>(s.get_v()[0]), 1);
	EXPECT_EQ(get<1>(s.get_v()[0]), 2);
	EXPECT_EQ(get<2>(s.get_v()[0]), 3);
	EXPECT_EQ(get<3>(s.get_v()[0]), 4);

	EXPECT_EQ(get<0>(s.get_v()[2]), 55);
	EXPECT_EQ(get<1>(s.get_v()[2]), 53);
	EXPECT_EQ(get<2>(s.get_v()[2]), 4);
	EXPECT_EQ(get<3>(s.get_v()[2]), 1);
}

TEST_F(SorterTest, ShouldThrowExceptIfAbracadabraInput)  {
	MockSorter s{};
	std::stringstream is("Abracadabra test \n");
	try {
		EXPECT_THROW( is >> s, std::runtime_error);
	} catch (...) {

	}
}

TEST_F(SorterTest, ShouldCollectForFilterkFirstOf)  {
	const auto kFilter = std::vector<Filter>{Filter{FilterPolicy::kFirstOf, 1}} ;

	MockSorter s({kFilter});

	std::stringstream is("01.002.003.4\t sss \t ddd \n02.03.005.255\t sdsdsd \t fgfgfg \n1.5.53.004.01\t ererre \t fgfg \n");
	is >> s;

	EXPECT_EQ(s.get_v_filtering()[0].size(), 2);

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][0]), 1);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][0]), 2);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][0]), 3);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][0]), 4);

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][1]), 1);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][1]), 5);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][1]), 53);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][1]), 4);

}

TEST_F(SorterTest, ShouldCollectForFilterkTwoFirstDigits)  {
	const auto kFilter = std::vector<Filter>{Filter{FilterPolicy::kFirstOf, 46,50}} ;

	MockSorter s{kFilter};

	std::stringstream is("46.50.003.4\t sss \t ddd \n02.03.005.255\t sdsdsd \t fgfgfg \n46.50.53.004.01\t ererre \t fgfg \n");
	is >> s;

	EXPECT_EQ(s.get_v_filtering()[0].size(), 2);

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][0]), 46);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][0]), 50);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][0]), 3);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][0]), 4);

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][1]), 46);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][1]), 50);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][1]), 53);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][1]), 4);
}

TEST_F(SorterTest, ShouldCollectForFilterkAnyOfDigits)  {
	const auto kFilter = std::vector<Filter>{Filter{FilterPolicy::kAnyOf, 46}} ;

	MockSorter s(kFilter);

	std::stringstream is("46.70.003.4\t sss \t ddd \n02.03.005.255\t sdsdsd \t fgfgfg \n10.70.46.004.01\t ererre \t fgfg \n");
	is >> s;

	EXPECT_EQ(s.get_v_filtering()[0].size(), 2);

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][0]), 46);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][0]), 70);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][0]), 3);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][0]), 4);

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][1]), 10);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][1]), 70);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][1]), 46);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][1]), 4);
}

TEST_F(SorterTest, ShouldSecondInputIpGreaterThanFirst1)  {
	MockSorter s;

	std::stringstream is("01.20.003.4\t sss \t ddd \n01.30.005.255\t sdsdsd \t fgfgfg \n");
	is >> s;

	s.SortIps();

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][0]), 1);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][0]), 30);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][0]), 5);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][0]), 255);

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][1]), 1);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][1]), 20);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][1]), 3);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][1]), 4);
}

TEST_F(SorterTest, ShouldSecondInputIpGreaterThanFirst2)  {
	MockSorter s;

	std::stringstream is("01.30.5.100\t sss \t ddd \n01.30.005.101\t sdsdsd \t fgfgfg \n");
	is >> s;

	s.SortIps();

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][0]), 1);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][0]), 30);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][0]), 5);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][0]), 101);

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][1]), 1);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][1]), 30);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][1]), 5);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][1]), 100);
}

TEST_F(SorterTest, ShouldSecondInputIpGreaterThanFirstAndNiceOutput)  {
	MockSorter s;

	std::stringstream is("01.30.5.100\t sss \t ddd \n01.30.005.101\t sdsdsd \t fgfgfg \n");
	is >> s;

	s.SortIps();

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][0]), 1);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][0]), 30);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][0]), 5);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][0]), 101);

	EXPECT_EQ(get<0>(s.get_v_filtering()[0][1]), 1);
	EXPECT_EQ(get<1>(s.get_v_filtering()[0][1]), 30);
	EXPECT_EQ(get<2>(s.get_v_filtering()[0][1]), 5);
	EXPECT_EQ(get<3>(s.get_v_filtering()[0][1]), 100);

	std::stringstream os;
	os << s;

	EXPECT_EQ(os.str(), std::string{"1.30.5.101\n1.30.5.100\n"});
}
