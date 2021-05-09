#include <gtest/gtest.h>

#include "uri.hpp"
#include <vector>

TEST(UriTests, ParseFromStringUrl) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("http://www.example.com/foo/bar"));
    ASSERT_EQ("http", uri.getScheme());
    ASSERT_EQ("www.example.com", uri.getHost());
    ASSERT_EQ((std::vector<std::string>{
            "",
            "foo",
            "bar"
    }), uri.getPath()
    );
}


TEST(UriTests, ParseFromStringUrnDefaultPathDelimiter) {
    Uri::Uri uri;

    ASSERT_TRUE(uri.parseFromString("urn:book:fantasy:hobbit"));
    ASSERT_EQ("urn", uri.getScheme());
    ASSERT_EQ("", uri.getHost());
    ASSERT_EQ((std::vector<std::string>{
            "book:fantasy:hobbit"
    }), uri.getPath()
    );

}

TEST(Uritests, ParseFromStringPathCornerCases) {
    struct TestVector {
        std::string pathIn;
        std::vector<std::string> pathOut;
    };

    const std::vector<TestVector> testVectors {
        {"", {}},
        {"/", {""}},
        {"/foo", {"", "foo"}},
        {"foo/", {"foo", ""}}
    };

    for(const auto& var : testVectors) {
        Uri::Uri uri;

        ASSERT_TRUE(uri.parseFromString(var.pathIn));
        ASSERT_EQ(var.pathOut, uri.getPath());
    }
}