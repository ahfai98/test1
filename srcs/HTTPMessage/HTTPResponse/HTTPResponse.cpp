/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaligno <amaligno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 17:25:41 by amaligno          #+#    #+#             */
/*   Updated: 2025/01/17 18:30:05 by amaligno         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse()
{
}

HTTPResponse::HTTPResponse(HTTPResponse &copy): HTTPMessage()
{
	*this = copy;
}

HTTPResponse::~HTTPResponse()
{
}

HTTPResponse	&HTTPResponse::operator=(HTTPResponse &copy)
{
	HTTPMessage::operator=(copy);
	return *this;
}
