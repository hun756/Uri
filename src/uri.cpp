#include "uri.hpp"
#include <iostream>
#include <sstream>

namespace Uri
{
    struct Uri::Imp
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


    Uri::Uri() : imp(new Imp)
    {

    }


    Uri::~Uri() = default;


    bool Uri::parseFromString(const std::string& uriString)
    {
        //> First parse schema
        const auto schemeEnd = uriString.find(':');
        imp->scheme = uriString.substr(0, schemeEnd);
        auto rest = uriString.substr(schemeEnd + 1);


        //> Second parse host
        imp->hasPort = false;
        if (rest.substr(0, 2) == "//")
        {
            const auto authorityEnd = rest.find('/', 2);
            const auto portDelimiter = rest.find(':');
            if (portDelimiter == std::string::npos)
            {
                imp->host = rest.substr(2, authorityEnd - 2);
            }
            else
            {
                imp->host = rest.substr(2, portDelimiter - 2);
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

                imp->port = static_cast<uint16_t>(newPort);
                imp->hasPort = true;
            }

            rest = rest.substr(authorityEnd);
        }
        else
        {
            imp->host.clear();
        }


        //> Finally, parse the path
        imp->path.clear();
        if (rest == "/")
        {
            imp->path.emplace_back("");
        }
        else if (!rest.empty())
        {
            for (;;)
            {
                auto pathDelimiter = rest.find('/');
                if (pathDelimiter == std::string::npos)
                {
                    imp->path.push_back(rest);
                    break;
                }
                else
                {
                    imp->path.emplace_back(
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
        return imp->scheme;
    }


    std::string Uri::getHost() const
    {
        return imp->host;
    }


    std::vector<std::string> Uri::getPath() const
    {
        return imp->path;
    }


    bool Uri::hasPort() const
    {
        return imp->hasPort;
    }


    uint16_t Uri::getPort() const
    {
        return imp->port;
    }
}