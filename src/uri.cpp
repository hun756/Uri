#include "uri.hpp"
#include <iostream>

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
        if (rest.substr(0, 2) == "//")
        {
            const auto authorityEnd = rest.find('/', 2);
            imp->host = rest.substr(2, authorityEnd - 2);
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
//        return imp->host;
        return imp->host;
    }


    std::vector<std::string> Uri::getPath() const
    {
        return imp->path;
    }

}