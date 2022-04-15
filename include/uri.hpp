#ifndef URI_HPP
#define URI_HPP

#include <memory>
#include <string>
#include <vector>

namespace Uri
{
	class Uri
	{
	public:
		Uri();

		~Uri();
		Uri(const Uri&) = delete;
		Uri(Uri&&) = delete;
		Uri& operator=(const Uri&) = delete;
		Uri& operator=(Uri&&) = delete;

		//> Methods
	public:
		/**
         *  @brief
         *      This method builds the URI from the elements parsed
         *      from the given string rendering of a URI.
         *
         *  @param[in]
         *      This is the string rendering of the URI to parse.
         *
         *  @return
         *      An indication of whether or not the uri was parsed
         *      successfully is returned.
         *
        **/
		bool parseFromString(const std::string& uriString);

		/**
         *  @brief
         *      This method returns the scheme element of the URI.
         *
         *  @return
         *      The "scheme" element of the URI is returned.
         *
         *  @retval
         *      This is returned if there is no "scheme" element in the URI.
         *
        **/
		std::string getScheme() const;

		/**
         *  @brief
         *      This method returns the "UserInfo" element of the URI.
         *
         *  @return
         *      The "UserInfo" element of the URI is returned.
         *
         *  @retval
         *      This is returned if there is no "UserInfo" element in the URI.
        **/
		std::string getUserInfo() const;

		/**
         *  @brief
         *      This method returns the scheme element of the URI.
         *
         *  @return
         *      The "host" element of the URI is returned.
         *
         *  @retval
         *      This is returned if there is no "host" element in the URI.
         *
        **/
		std::string getHost() const;

		/**
         *  @brief
         *      This method returns the "path" element of the URI.
         *      as a sequence segment
         *
         *  @note
         *      if the first segment of the path is an empty string.
         *      then the URI has an absolute path.
         *
         *  @return
         *      The "path" element of the URI is returned
         *      as a sequence of segments.
        **/
		std::vector<std::string> getPath() const;

		/**
         *  @brief
         *      This methods returns an indication of whether or not
         *      URI includes a port number.
         *
         *  @return
         *      An indication of whether or not the
         *      URI includes port number is returned
        **/
		bool hasPort() const;

		/**
         *  @brief
         *      This method returns the port number element of the URI,
         *      if it has one
         *
         *  @return
         *      The port number element of the URI is returned.
         *
         *  @note
         *      The returned port number is only valid if the
         *      HasPort method returns true
        **/
		uint16_t getPort() const;

		/**
         *  @brief
         *      This method returns an indication of whether or not
         *      the URI is relative reference. 
         * 
         *  @return 
         *      An indicaiton of wether or nt the URI is a
         *      relative reference is returned. 
        **/
		bool isRelativeReference() const;

		/**
         *  @brief
         *      This method returns an indication of whether or not
         *      the URI contains relative path. 
         * 
         *  @return An indicaiton of wether or nt the URI contains a
         *          relative path is returned. 
        **/
		bool containsRelativePath() const;

		/**
         *  @brief 
         *      This method returns the "fragment" element of the URI
         *      if it has one.
         * 
         *  @return std::string 
         *      The "fragment" element of the URI is returned.
         * 
         *  @retval ""
         *      This is returned if there is no "fragment" element int he URI.
        **/
		std::string getFragment() const;

		/**
         *  @brief 
         *      This method returns the "quey" element of the URI
         *      if it has one.
         * 
         *  @return std::string 
         *      The "quey" element of the URI is returned.
         * 
         *  @retval ""
         *      This is returned if there is no "quey" element int he URI.
        **/
		std::string getQuery() const;

	private:
		struct Impl;

		std::unique_ptr<Impl> impl;
	};
} // namespace Uri

#endif /* end of include guard : URI_HPP */
