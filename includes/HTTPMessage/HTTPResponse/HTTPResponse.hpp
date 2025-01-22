/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaligno <amaligno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:25:31 by amaligno          #+#    #+#             */
/*   Updated: 2025/01/17 18:09:06 by amaligno         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "HTTPMessage.hpp"

#include <string>

class HTTPResponse : public HTTPMessage
{
	public:
	//Constructors
		HTTPResponse();
		HTTPResponse(HTTPResponse &copy);
	//Destructor
		~HTTPResponse();
	//Operator Overloads
		HTTPResponse	&operator=(HTTPResponse &copy);
		
	private:
};

#endif