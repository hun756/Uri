#include "uri.hpp"
#include <iostream>
#include <sstream>

namespace Uri
{
    struct Uri::Impl
    {
        /**
         *  @brief
         *      This is the "scheme" element of URI.
        **/
        std::string scheme;


        /**
         *  @brief
         *      This is the "host" element of URI.
        **/
        std::string host;


        /**
         *  @brief
         *      The flag indicates whether or not the
         *      URI includes a port number.
        **/
        bool hasPort = false;


        /**
         *  @brief
         *      This is the port number element of the uri
        **/
        uint16_t port = 0;

        /**
        *  @brief
        *      This is the "path" elements of URI.
       **/
        std::vector<std::string> path;
    };


    Uri::Uri() : impl(new Impl)
    {

    }


    Uri::~Uri() = default;


    bool Uri::parseFromString(const std::string& uriString)
    {
        //> First parse schema
        const auto schemeEnd = uriString.find(':');
        impl->scheme = uriString.substr(0, schemeEnd);
        auto rest = uriString.substr(schemeEnd + 1);


        //> Second parse host
        impl->hasPort = false;
        if (rest.substr(0, 2) == "//")
        {
            const auto authorityEnd = rest.find('/', 2);
            const auto portDelimiter = rest.find(':');
            if (portDelimiter == std::string::npos)
            {
                impl->host = rest.substr(2, authorityEnd - 2);
            }
            else
            {
                impl->host = rest.substr(2, portDelimiter - 2);
//                const auto portNumStr = rest.substr(portDelimiter + 1, authorityEnd - portDelimiter - 1);

                uint32_t newPort = 0;
                for (auto c : rest.substr(portDelimiter + 1, authorityEnd - portDelimiter - 1)) {
                    if (!isdigit(c)) {
                        return false;
                    }

                    newPort *= 10;
                    newPort += static_cast<uint16_t>(c - '0');

                    if ((newPort & ~((1 << 16) - 1)) != 0)  {
                        return false;
                    }
                }

                impl->port = static_cast<uint16_t>(newPort);
                impl->hasPort = true;
            }

            rest = rest.substr(authorityEnd);
        }
        else
        {
            impl->host.clear();
        }


        //> Finally, parse the path
        impl->path.clear();
        if (rest == "/")
        {
            impl->path.emplace_back("");
        }
        else if (!rest.empty())
        {
            for (;;)
            {
                auto pathDelimiter = rest.find('/');
                if (pathDelimiter == std::string::npos)
                {
                    impl->path.push_back(rest);
                    break;
                }
                else
                {
                    impl->path.emplace_back(
                            rest.begin(),
                            rest.begin() + pathDelimiter
                    );
                    rest = rest.substr(pathDelimiter + 1);
                }
            }
        }

        return true;
    }


    std::string Uri::getScheme() const
    {
        return impl->scheme;
    }


    std::string Uri::getHost() const
    {
        return impl->host;
    }


    std::vector<std::string> Uri::getPath() const
    {
        return impl->path;
    }


    bool Uri::hasPort() const
    {
        return impl->hasPort;
    }


    uint16_t Uri::getPort() const
    {
        return impl->port;
    }
}