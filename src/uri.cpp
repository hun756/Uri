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

		/**
         *  @brief 
         *      This is the "fragment" element of the URI.
         *      if it has one.
         */
		std::string fragment;

		/**
         *  @brief 
         *      This is the "query" element of the URI.
         *      if it has one.
         */
		std::string query;
	};

	Uri::Uri()
		: impl(new Impl)
	{ }

	Uri::~Uri() = default;

	bool Uri::parseFromString(const std::string& uriString)
	{
		//> First parse schema
		const auto schemeEnd = uriString.find(':');
		std::string rest;
		if(schemeEnd == std::string::npos)
		{
			impl->scheme.clear();
			rest = uriString;
		}
		else
		{
			impl->scheme = uriString.substr(0, schemeEnd);
			rest = uriString.substr(schemeEnd + 1);
		}

		//> Second parse host
		impl->hasPort = false;
		const auto pathEnd = rest.find_first_of("?#");
		auto hostandPathString = rest.substr(0, pathEnd);
		const auto queryAndOrFragment = rest.substr(hostandPathString.length());
		if(hostandPathString.substr(0, 2) == "//")
		{
			auto authorityEnd = hostandPathString.find('/', 2);
			if(authorityEnd == std::string::npos)
			{
				authorityEnd = hostandPathString.length();
			}
			const auto portDelimiter = hostandPathString.find(':');
			if(portDelimiter == std::string::npos)
			{
				impl->host = hostandPathString.substr(2, authorityEnd - 2);
			}
			else
			{
				impl->host = hostandPathString.substr(2, portDelimiter - 2);
				// const auto portNumStr = hostandPathString.substr(portDelimiter + 1, authorityEnd - portDelimiter - 1);

				uint32_t newPort = 0;
				for(auto c :
					hostandPathString.substr(portDelimiter + 1, authorityEnd - portDelimiter - 1))
				{
					if(!isdigit(c))
						return false;

					newPort *= 10;
					newPort += static_cast<uint16_t>(c - '0');

					if((newPort & ~((1 << 16) - 1)) != 0)
					{
						return false;
					}
				}

				impl->port = static_cast<uint16_t>(newPort);
				impl->hasPort = true;
			}

			hostandPathString = hostandPathString.substr(authorityEnd);
		}
		else
		{
			impl->host.clear();
		}

		auto pathString = hostandPathString;
		//> Next, parse the path
		impl->path.clear();
		if(pathString == "/")
		{
			impl->path.emplace_back("");
			pathString.clear();
		}
		else if(!pathString.empty())
		{
			for(;;)
			{
				auto pathDelimiter = pathString.find('/');
				if(pathDelimiter == std::string::npos)
				{
					impl->path.push_back(pathString);
					pathString.clear();
					break;
				}
				else
				{
					impl->path.emplace_back(pathString.begin(), pathString.begin() + pathDelimiter);
					pathString = pathString.substr(pathDelimiter + 1);
				}
			}
		}

		// next parse the fragment if there is one
		const auto fragmentDelimiter = queryAndOrFragment.find('#');
		if(fragmentDelimiter == std::string::npos)
		{
			impl->fragment.clear();
			rest = queryAndOrFragment;
		}
		else
		{
			impl->fragment = queryAndOrFragment.substr(fragmentDelimiter + 1);
			rest = queryAndOrFragment.substr(0, fragmentDelimiter);
		}

		// finally, if anything is left, it's the query.
		if(rest.empty())
		{
			impl->query.clear();
		}
		else
		{
			impl->query = rest.substr(1);
		}

		return true;
	}

	bool Uri::isRelativeReference() const
	{
		return impl->scheme.empty();
	}

	bool Uri::containsRelativePath() const
	{
		return (impl->path.empty()) ? true : !impl->path[0].empty();
	}

	std::string Uri::getQuery() const
	{
		return impl->query;
	}

	std::string Uri::getFragment() const
	{
		return impl->fragment;
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
} // namespace Uri