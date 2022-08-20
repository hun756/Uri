#include "uri.hpp"

namespace
{
	/**
	 * 	@brief 
	 *  	This function parses the given string as an unsigned 16-bit
	 * 		integer, detecting invalid characters, owerflow, etc.
	 * 
	 * 	@param numberString 
	 * 		This is the string containing the number to parse.
	 * 
	 * 	@param[out] number
	 * 		This is where to store port number parsed.
	 *  
	 * 	@return
	 * 		An indication of whether or not the number was parsed 
	 * 		successfully is returned. 
	**/
	bool parseUint16(const std::string& numberString, uint16_t& number)
	{
		uint32_t numberIn32Bits = 0;
		for(auto c : numberString)
		{
			if(!isdigit(c))
				return false;

			numberIn32Bits *= 10;
			numberIn32Bits += static_cast<uint16_t>(c - '0');

			if((numberIn32Bits & ~((1 << 16) - 1)) != 0)
				return false;
		}

		number = static_cast<uint16_t>(numberIn32Bits);
		return true;
	}
} // namespace

namespace Uri
{
	struct Uri::Impl
	{
		///< Properties

		/**
         *  @brief
         *      This is the "scheme" element of URI.
        **/
		std::string scheme;

		/**
         *  @brief
         *      This is the "User Info" element of URI.
        **/
		std::string userInfo;

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

		///< Methods

		/**
		 * 	@brief
		 * 		This method builds internal path element sequence
		 * 		by parsing it from the given path string
		 *
		 * 	@param[in] pathString
		 * 		This is the string containing the whole path of the URI.
		 * 	@return
		 * 		An indication if the path was parsed correctly or not
		 * 		is returned.
		**/
		bool parsePath(std::string pathString)
		{
			path.clear();
			if(pathString == "/")
			{
				path.emplace_back("");
				pathString.clear();
			}
			else if(!pathString.empty())
			{
				for(;;)
				{
					auto pathDelimiter = pathString.find('/');
					if(pathDelimiter == std::string::npos)
					{
						path.push_back(pathString);
						pathString.clear();
						break;
					}
					else
					{
						path.emplace_back(pathString.begin(), pathString.begin() + pathDelimiter);
						pathString = pathString.substr(pathDelimiter + 1);
					}
				}
			}
			return true;
		}

		/**
		 * 	@brief
		 * 		This method parses the elements that make up the authority
		 * 		composite part of the URI by parsing it from the given string
		 *
		 * 	@param[in] authorityString
		 * 		This is the string containing the whole authority path of the URI.
		 * 	@return
		 * 		An indication if the path was parsed correctly or not
		 * 		is returned.
		**/
		bool parseAuthority(const std::string& authorityString)
		{
			///< Next Check if there is UserInfo, and if so, extract it
			const auto userInfoDelimiter = authorityString.find('@');
			std::string hostPortString;
			if(userInfoDelimiter == std::string::npos)
			{
				userInfo.clear();
				hostPortString = authorityString;
			}
			else
			{

				userInfo = authorityString.substr(0, userInfoDelimiter);
				hostPortString = authorityString.substr(userInfoDelimiter + 1);
			}

			///< Next, parsing host and port from authority and path.
			const auto portDelimiter = hostPortString.find(':');
			if(portDelimiter == std::string::npos)
			{
				host = hostPortString;
				hasPort = false;
			}
			else
			{
				host = hostPortString.substr(0, portDelimiter);
				// const auto portNumStr = authorityAndPathString.substr(portDelimiter + 1, authorityEnd - portDelimiter - 1);

				const auto portString = hostPortString.substr(portDelimiter + 1);
				if(!parseUint16(portString, port))
				{
					return false;
				}
				hasPort = true;
			}

			return true;
			//			hostPortString = authorityAndPathString.substr(authorityEnd);
		}
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

		//> Next parse the authority
		const auto pathEnd = rest.find_first_of("?#");
		auto authorityAndPathString = rest.substr(0, pathEnd);
		const auto queryAndOrFragment = rest.substr(authorityAndPathString.length());
		std::string pathString;
		if(authorityAndPathString.substr(0, 2) == "//")
		{
			///< Strip of the authority marker.
			authorityAndPathString = authorityAndPathString.substr(2);

			///< First seperate the authority from the path.
			auto authorityEnd = authorityAndPathString.find('/', 2);
			if(authorityEnd == std::string::npos)
			{
				authorityEnd = authorityAndPathString.length();
			}

			pathString = authorityAndPathString.substr(authorityEnd);
			auto authorityString = authorityAndPathString.substr(0, authorityEnd);

			///< Parse the elemnts inside authority
			if(!impl->parseAuthority(authorityString))
				return false;
		}
		else
		{
			impl->userInfo.clear();
			impl->host.clear();
			impl->hasPort = false;
			pathString = authorityAndPathString;
		}

		//> Next, parse the path
		if(!impl->parsePath(pathString))
			return false;

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

	std::string Uri::getUserInfo() const
	{
		return impl->userInfo;
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
