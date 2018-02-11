#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BOLD");
    BOOST_CHECK(GetBoolArg("-BOLD"));
    BOOST_CHECK(GetBoolArg("-BOLD", false));
    BOOST_CHECK(GetBoolArg("-BOLD", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BOLDo"));
    BOOST_CHECK(!GetBoolArg("-BOLDo", false));
    BOOST_CHECK(GetBoolArg("-BOLDo", true));

    ResetArgs("-BOLD=0");
    BOOST_CHECK(!GetBoolArg("-BOLD"));
    BOOST_CHECK(!GetBoolArg("-BOLD", false));
    BOOST_CHECK(!GetBoolArg("-BOLD", true));

    ResetArgs("-BOLD=1");
    BOOST_CHECK(GetBoolArg("-BOLD"));
    BOOST_CHECK(GetBoolArg("-BOLD", false));
    BOOST_CHECK(GetBoolArg("-BOLD", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBOLD");
    BOOST_CHECK(!GetBoolArg("-BOLD"));
    BOOST_CHECK(!GetBoolArg("-BOLD", false));
    BOOST_CHECK(!GetBoolArg("-BOLD", true));

    ResetArgs("-noBOLD=1");
    BOOST_CHECK(!GetBoolArg("-BOLD"));
    BOOST_CHECK(!GetBoolArg("-BOLD", false));
    BOOST_CHECK(!GetBoolArg("-BOLD", true));

    ResetArgs("-BOLD -noBOLD");  // -BOLD should win
    BOOST_CHECK(GetBoolArg("-BOLD"));
    BOOST_CHECK(GetBoolArg("-BOLD", false));
    BOOST_CHECK(GetBoolArg("-BOLD", true));

    ResetArgs("-BOLD=1 -noBOLD=1");  // -BOLD should win
    BOOST_CHECK(GetBoolArg("-BOLD"));
    BOOST_CHECK(GetBoolArg("-BOLD", false));
    BOOST_CHECK(GetBoolArg("-BOLD", true));

    ResetArgs("-BOLD=0 -noBOLD=0");  // -BOLD should win
    BOOST_CHECK(!GetBoolArg("-BOLD"));
    BOOST_CHECK(!GetBoolArg("-BOLD", false));
    BOOST_CHECK(!GetBoolArg("-BOLD", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BOLD=1");
    BOOST_CHECK(GetBoolArg("-BOLD"));
    BOOST_CHECK(GetBoolArg("-BOLD", false));
    BOOST_CHECK(GetBoolArg("-BOLD", true));

    ResetArgs("--noBOLD=1");
    BOOST_CHECK(!GetBoolArg("-BOLD"));
    BOOST_CHECK(!GetBoolArg("-BOLD", false));
    BOOST_CHECK(!GetBoolArg("-BOLD", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", "eleven"), "eleven");

    ResetArgs("-BOLD -bar");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", "eleven"), "");

    ResetArgs("-BOLD=");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", "eleven"), "");

    ResetArgs("-BOLD=11");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", "eleven"), "11");

    ResetArgs("-BOLD=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BOLD", 0), 0);

    ResetArgs("-BOLD -bar");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BOLD=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BOLD=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BOLD");
    BOOST_CHECK_EQUAL(GetBoolArg("-BOLD"), true);

    ResetArgs("--BOLD=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BOLD", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBOLD");
    BOOST_CHECK(!GetBoolArg("-BOLD"));
    BOOST_CHECK(!GetBoolArg("-BOLD", true));
    BOOST_CHECK(!GetBoolArg("-BOLD", false));

    ResetArgs("-noBOLD=1");
    BOOST_CHECK(!GetBoolArg("-BOLD"));
    BOOST_CHECK(!GetBoolArg("-BOLD", true));
    BOOST_CHECK(!GetBoolArg("-BOLD", false));

    ResetArgs("-noBOLD=0");
    BOOST_CHECK(GetBoolArg("-BOLD"));
    BOOST_CHECK(GetBoolArg("-BOLD", true));
    BOOST_CHECK(GetBoolArg("-BOLD", false));

    ResetArgs("-BOLD --noBOLD");
    BOOST_CHECK(GetBoolArg("-BOLD"));

    ResetArgs("-noBOLD -BOLD"); // BOLD always wins:
    BOOST_CHECK(GetBoolArg("-BOLD"));
}

BOOST_AUTO_TEST_SUITE_END()
