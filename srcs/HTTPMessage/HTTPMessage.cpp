# include "../../includes/HTTPMessage/HTTPMessage.hpp"

using WebServer::Utils;

/**
 * Default constructor for the HTTPMessage class.
 * initialises an empty HTTPMessage object.
 */
HTTPMessage::HTTPMessage() {}

/**
 * Destructor for the HTTPMessage class.
 * Clears all headers and releases resources.
 */
HTTPMessage::~HTTPMessage()
{
	this->_headers.clear();
}

/**
 * Copy constructor for the HTTPMessage class.
 *
 * @param src The HTTPMessage object to copy from.
 */
HTTPMessage::HTTPMessage(const HTTPMessage& src) { *this = src; }

/**
 * Assignment operator for the HTTPMessage class.
 *
 * @param src The HTTPMessage object to assign from.
 * @returns A reference to the updated HTTPMessage object.
 */
HTTPMessage& HTTPMessage::operator=(const HTTPMessage& src)
{
	if (this != &src) {
		this->_headers = src._headers;
		this->_body = src._body;
	}
	return *this;
}

/**
 * Sets a header field in the HTTP message.
 *
 * @param name The name of the header field.
 * @param value The value of the header field.
 */
void HTTPMessage::setHeader(const string& name, const string& value)
{
	this->_headers[name] = value;
}

/**
 * Sets the body of the HTTP message.
 *
 * @param body The body content as a string.
 */
void HTTPMessage::setBody(string body)
{
	this->_body = body;
}

/**
 * Retrieves the value of a header field by name.
 *
 * @param name The name of the header field to retrieve.
 * @returns The value of the header field, or NULL if not found.
 */
string HTTPMessage::getFieldName(const string& name) const
{
	try
	{
		return this->_headers.at(name);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return NULL;
}

/**
 * Retrieves all header fields in the HTTP message.
 *
 * @returns A vector of KeyValue objects representing the headers.
 */
const std::map<string, string> HTTPMessage::getHeaders() const
{
	return this->_headers;
}

/**
 * Retrieves the body of the HTTP message.
 *
 * @returns The body content as a string.
 */
string HTTPMessage::getBody() const
{
	return this->_body;
}

/**
 * Constructs the full HTTP message as a string, including headers and body.
 *
 * @returns The HTTP message as a string.
 */
string HTTPMessage::getMessage() const
{
	string message;

	for (std::map<string, string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); it++) {
		message += it->first + ": " + it->second + CRLF;
	}
	message += CRLF + this->_body;
	return message;
}

/**
 * Retrieves the start line of the HTTP message.
 *
 * @returns The start line as a string.
 */
string HTTPMessage::getStarline() const { return this->_start_line; }

/**
 * Overloaded stream operator to output a vector of KeyValue objects (headers).
 *
 * @param os The output stream to write to.
 * @param headers The vector of KeyValue objects to output.
 * @returns A reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const std::map<string, string>& headers)
{
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        os << it->first << ": " << it->second << "\n";
    }
    return os;
}
