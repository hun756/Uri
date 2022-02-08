#include <corecrt.h>
#include <gtest/gtest.h>

#include "uri.hpp"
#include <vector>

TEST(UriTests, ParseFromStringNoSchema) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("foo/bar"));
    ASSERT_EQ("", uri.getScheme());
    ASSERT_EQ((std::vector<std::string>{
            "foo",
            "bar"
    }), uri.getPath()
    );
}

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

TEST(UriTests, ParseFromStringPathCornerCases) {
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


TEST(UriTests, ParseFromStringBadPortNumberPurelyAlphabetic) {
    Uri::Uri uri;

    ASSERT_FALSE(uri.parseFromString("https://example.com:spam/foo/bar"));
}

TEST(UriTests, ParseFromStringBadPortNumberStartsNumericEndsAlphabetic) {
    Uri::Uri uri;

    ASSERT_FALSE(uri.parseFromString("https://example.com:8080spam/foo/bar"));
}


TEST(UriTests, ParseFromStringLargestValidPortNumber) {
    Uri::Uri uri;

    ASSERT_TRUE(uri.parseFromString("https://example.com:65535/foo/bar"));
}

TEST(UriTests, ParseFromStringBadPortNumberTooBig) {
    Uri::Uri uri;

    ASSERT_FALSE(uri.parseFromString("https://example.com:65536/foo/bar"));
}


TEST(UriTests, ParseFromStringBadPortNumberNegative) {
    Uri::Uri uri;

    ASSERT_FALSE(uri.parseFromString("https://example.com:-1564/foo/bar"));
}

TEST(UriTests, ParseFromStringAfterAuthority) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("http://wwww.example.com"));
}

TEST(UriTests, ParseFromStringRelativeVsNonRelativeReferences) {
    struct TestVector {
        std::string uriString;
        bool isRelative;
    };

    const std::vector<TestVector> testVecs {
        { "http://wwww.example.com/", false },
        { "http://wwww.example.com", false },
        { "/", true },
        { "foo", true }
    };
    
    size_t index = 0;
    for (const auto& testVec : testVecs ) {
        Uri::Uri uri;
        ASSERT_TRUE(uri.parseFromString(testVec.uriString)) << index;
        ASSERT_EQ(testVec.isRelative, uri.isRelativeReference()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringRelativeVsNonRelativePaths) {
    struct TestVector {
        std::string uriString;
        bool containsRelativePath;
    };

    const std::vector<TestVector> testVecs {
        { "http://wwww.example.com/", false },
        { "http://wwww.example.com", true },
        { "/", false },
        { "foo", true },

        /**
         * This is only a valid vector if we understand
         * correctly that an empty string Is a valid
         * "relative reference" URI with an wmpty path.
        **/
        {"", true}
    };
    
    size_t index = 0;
    for (const auto& testVec : testVecs ) {
        Uri::Uri uri;
        ASSERT_TRUE(uri.parseFromString(testVec.uriString)) << index;
        ASSERT_EQ(testVec.containsRelativePath, uri.containsRelativePath()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringFragmentsAndQuery) {
    struct TestVector {
        std::string uriString;
        std::string host;
        std::string query;
        std::string fragment;
    };

    const std::vector<TestVector> testVecs {
        {"http://www.example.com/", "www.example.com", "", ""},
        {"http://www.example.com?foo", "www.example.com", "foo", ""},
        {"http://www.example.com#foo", "www.example.com", "", "foo"},
        {"http://www.example.com?foo#bar", "www.example.com", "foo", "bar"},
        {"http://www.example.com?earth#bar", "www.example.com", "earth", "bar"},
        {"http://www.example.com/spam?foo#bar", "www.example.com", "foo", "bar"},

        /**
         * NOTE: curiously, but we think is coorect, that
         * having a trailing quesiton mark is equivalent to not having
         * any question mark, because in both cases, in the query element
         * is emty string. Perhaps research deeper to see if this is right.
        **/
        {"http://www.example.com/?", "www.example.com", "", ""}
    };

    size_t index = 0;
    for (const auto& testVec : testVecs) {
        Uri::Uri uri;
        ASSERT_TRUE(uri.parseFromString(testVec.uriString)) << index;
        ASSERT_EQ(testVec.host, uri.getHost()) << index;
        ASSERT_EQ(testVec.query, uri.getQuery()) << index;
        ASSERT_EQ(testVec.fragment, uri.getFragment()) << index;
        ++index;
    }
}