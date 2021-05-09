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

TEST(UriTests, ParseFromStringHasPort) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("http://www.example.com:8080/foo/bar"));
    ASSERT_EQ("http", uri.getScheme());
    ASSERT_EQ("www.example.com", uri.getHost());
    ASSERT_EQ((std::vector<std::string>{
            "",
            "foo",
            "bar"
    }), uri.getPath()
    );

    ASSERT_TRUE(uri.hasPort());
    ASSERT_EQ(8080, uri.getPort());
}


TEST(UriTests, ParseFromStringDoesNotHavePort) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("http://www.example.com/foo/bar"));

    ASSERT_EQ("www.example.com", uri.getHost());
    ASSERT_FALSE(uri.hasPort());
}

TEST(UriTests, ParseFromStringTwiceFirstWithPortNumberThenWithout) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("http://www.example.com:8080/foo/bar"));
    ASSERT_TRUE(uri.parseFromString("http://www.example.com/foo/bar"));

    ASSERT_EQ("www.example.com", uri.getHost());
    ASSERT_FALSE(uri.hasPort());
}


TEST(Uritests, ParseFromStringBadPortNumberPurelyAlphabetic) {
    Uri::Uri uri;

    ASSERT_FALSE(uri.parseFromString("https://example.com:spam/foo/bar"));
}

TEST(Uritests, ParseFromStringBadPortNumberStartsNumericEndsAlphabetic) {
    Uri::Uri uri;

    ASSERT_FALSE(uri.parseFromString("https://example.com:8080spam/foo/bar"));
}


TEST(Uritests, ParseFromStringLargestValidPortNumber) {
    Uri::Uri uri;

    ASSERT_TRUE(uri.parseFromString("https://example.com:65535/foo/bar"));
}

TEST(Uritests, ParseFromStringBadPortNumberTooBig) {
    Uri::Uri uri;

    ASSERT_FALSE(uri.parseFromString("https://example.com:65536/foo/bar"));
}


TEST(Uritests, ParseFromStringBadPortNumberNegative) {
    Uri::Uri uri;

    ASSERT_FALSE(uri.parseFromString("https://example.com:-1564/foo/bar"));
}
