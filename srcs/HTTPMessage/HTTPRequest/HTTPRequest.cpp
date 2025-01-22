# include "../../../includes/HTTPMessage/HTTPRequest/HTTPRequest.hpp"

/**
 * @brief Default constructor for HTTPRequest.
 * initialises an empty HTTPRequest object.
 */
HTTPRequest::HTTPRequest() {}

/**
 * @brief Destructor for HTTPRequest.
 * Cleans up resources used by the HTTPRequest object.
 */
HTTPRequest::~HTTPRequest() {}

/**
 * @brief Copy constructor for HTTPRequest.
 *
 * @param src The HTTPRequest object to copy from.
 */
HTTPRequest::HTTPRequest(const HTTPRequest& src): HTTPMessage() { *this = src; }

/**
 * @brief Assignment operator for HTTPRequest.
 *
 * Copies the contents of one HTTPRequest object to another.
 *
 * @param src The HTTPRequest object to assign from.
 * @return HTTPRequest& A reference to the updated HTTPRequest object.
 */
HTTPRequest& HTTPRequest::operator=(const HTTPRequest& src)
{
	if (this != &src) {
		this->_method = src._method;
        this->_request_target = src._request_target;
        this->_http_version = src._http_version;
	}
	return *this;
}

/**
 * @brief Constructs an HTTPRequest object from a raw HTTP message string.
 *
 * Parses the start line of the message to extract the HTTP method, request target,
 * and HTTP version.
 *
 * @param message The raw HTTP request message.
 */
HTTPRequest::HTTPRequest(const string& message)
{
	this->_parseMessage(message);
	this->checker();
}


// Private Parsers
/**
 * @brief Parses the raw HTTP message string into components.
 *
 * Splits the message into the start-line, headers, and body. Validates the structure
 * of each component and ensures compliance with HTTP grammar.
 *
 * @param message The raw HTTP message as a string.
 * @throws std::runtime_error If critical components like the start-line or headers are missing.
 * @throws std::out_of_range If the header length exceeds the message size.
 */
void HTTPRequest::_parseMessage(const std::string& message) {
    // Check if the message contains a CRLF to separate the start line from headers
    size_t startLineEnd = message.find(CRLF);
    if (startLineEnd == std::string::npos) {
        throw HTTPRequest::HeadersDoNotExist();
    }

    // Parse the start line (e.g., request/response line)
    this->_parseStartline(message.substr(0, startLineEnd));

    // Find the position of the field line separator (empty line between headers and body)
    size_t fieldLinePos = message.find(FIELD_LINE_SEPARATOR, startLineEnd + string(CRLF).size());
    if (fieldLinePos == std::string::npos) {
        throw std::runtime_error("FIELD_LINE_SEPARATOR not found after headers");
    }

    // Calculate the length of the headers section
    size_t headersStart = startLineEnd + string(CRLF).size();
    size_t headerLength = fieldLinePos - headersStart;

    // Ensure header length is within bounds
    if (headersStart + headerLength > message.size()) {
        throw std::out_of_range("Header length exceeds message size");
    }

    // Parse headers
    this->_parseHeaders(message.substr(headersStart, headerLength));

    // Calculate the position where the body starts
    size_t bodyStart = fieldLinePos + string(FIELD_LINE_SEPARATOR).size();

    // Parse the body if it exists
    if (bodyStart < message.size()) {
        this->_parseBody(message.substr(bodyStart));
    }

    // Split the start line into components
    std::vector<string> split_string = WebServer::Utils::splitString(this->_start_line);
    if (split_string.size() != 3)
        throw RequestLineError();

    // Set method, request target, and HTTP version
    this->_setMethod(split_string[0]);
    this->_setRequestTarget(split_string[1]);
    this->_setHttpVersion(split_string[2]);

    // Clear the split string vector
    split_string.clear();
}

/**
 * @brief Parses the start-line of the HTTP request.
 *
 * Extracts and validates the HTTP method, request target, and HTTP version
 * from the start-line. Ensures compliance with HTTP/1.1 specifications.
 *
 * @param start_line The start-line string (e.g., "GET /index.html HTTP/1.1").
 * @throws std::runtime_error If the start-line format is invalid or contains unsupported components.
 */
void HTTPRequest::_parseStartline(const string& start_line)
{
	this->_start_line = start_line;
}

/**
 * @brief Parses the headers section of the HTTP message.
 *
 * Splits the headers into key-value pairs and validates each pair. Skips malformed
 * or whitespace-preceded lines to prevent security vulnerabilities like request smuggling.
 *
 * @param headers The headers section as a string.
 * @throws std::runtime_error If a header line is malformed.
 */
void HTTPRequest::_parseHeaders(const string& headers)
{
	std::vector<string> key_value;
	std::vector<string> tmp = WebServer::Utils::splitString(headers, CRLF);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		key_value = WebServer::Utils::splitString(tmp[i], ": ");
		this->_headers[key_value[0]] = key_value[1];
	}
}

/**
 * @brief Parses the body of the HTTP message.
 *
 * Validates the body length against the `Content-Length` header. If the body length
 * does not match the declared length, an error is thrown.
 *
 * @param body The body content as a string.
 * @throws std::runtime_error If the body length does not match the expected length.
 */

void HTTPRequest::_parseBody(const string& body)
{
	this->_body = body;
}

/**
 * @brief Sets the HTTP method of the request.
 *
 * @param method The HTTP method (e.g., GET, POST).
 */
void HTTPRequest::_setMethod(const string& method) { this->_method = method; }

/**
 * @brief Sets the request target of the HTTP request.
 *
 * @param request_target The target resource of the HTTP request (e.g., "/index.html").
 */
void HTTPRequest::_setRequestTarget(const string& request_target) { this->_request_target = request_target; }

/**
 * @brief Sets the HTTP version of the request.
 *
 * @param http_version The HTTP version (e.g., "HTTP/1.1").
 */
void HTTPRequest::_setHttpVersion(const string& http_version) { this->_http_version = http_version; }

// Getters

/**
 * @brief Retrieves the HTTP method of the request.
 *
 * @return string The HTTP method (e.g., GET, POST).
 */
string HTTPRequest::getRequestMethod() const { return this->_method; }

/**
 * @brief Retrieves the request target of the HTTP request.
 *
 * @return string The target resource (e.g., "/index.html").
 */
string HTTPRequest::getRequestTarget() const { return this->_request_target; }

/**
 * @brief Retrieves the HTTP version of the request.
 *
 * @return string The HTTP version (e.g., "HTTP/1.1").
 */
string HTTPRequest::getHttpVersion() const { return this->_http_version; }

/**
 * @brief Abstract method for additional validation or checks.
 *
 * This method is a placeholder to be implemented in derived classes.
 */
void HTTPRequest::checker()
{
	std::map<string, string> headers = this->getHeaders();
}
