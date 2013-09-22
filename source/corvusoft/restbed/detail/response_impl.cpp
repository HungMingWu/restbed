/*
 * Site: restbed.corvusoft.co.uk
 * Author: Ben Crowhurst
 *
 * Copyright (c) 2013 Restbed Core Development Team and Community Contributors
 *
 * This file is part of Restbed.
 *
 * Restbed is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Restbed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with Restbed.  If not, see <http://www.gnu.org/licenses/>.
 */

//System Includes
#include <ctime>
#include <chrono>

//Project Includes
#include "restbed/status_code.h"
#include "restbed/detail/response_impl.h"
#include "restbed/detail/helpers/map.h"
#include "restbed/detail/helpers/string.h"

//External Includes

//System Namespaces
using std::map;
using std::string;
using std::time_t;
using std::to_string;
using std::chrono::system_clock;

//Project Namespaces
using restbed::detail::helpers::Map;
using restbed::detail::helpers::String;

//External Namespaces

namespace restbed
{
    namespace detail
    {
        ResponseImpl::ResponseImpl( void ) : m_body( ),
                                             m_status_code( StatusCode::OK ),
                                             m_headers( )
        {
            //n/a
        }
        
        ResponseImpl::ResponseImpl( const ResponseImpl& original ) : m_body( original.m_body ),
                                                                     m_status_code( original.m_status_code ),
                                                                     m_headers( original.m_headers )
        {
            //n/a
        }
        
        ResponseImpl::~ResponseImpl( void )
        {
            //n/a
        }

        Bytes ResponseImpl::to_bytes( void ) const
        {
            string sections = generate_status_section( );
            sections += generate_header_section( );
            sections += "\r\n";

            Bytes bytes( sections.begin( ), sections.end( ) );
            bytes.insert( bytes.end( ), m_body.begin( ), m_body.end( ) );

            return bytes;
        }

        Bytes ResponseImpl::get_body( void ) const
        {
            return m_body;
        }
         
        int ResponseImpl::get_status_code( void ) const
        {
            return m_status_code;
        }

        string ResponseImpl::get_header( const string& name ) const
        {
            string value = String::empty;
            
            if ( has_header( name ) )
            {
                const auto iterator = Map::find_key_ignoring_case( name, m_headers );

                value = iterator->second;
            }

            return value;
        }

        map< string, string > ResponseImpl::get_headers( void ) const
        {
            return m_headers;
        }
        
        void ResponseImpl::set_body( const Bytes& value )
        {
            m_body = value;
        }

        void ResponseImpl::set_body( const string& value )
        {
            m_body = Bytes( value.begin( ), value.end( ) );
        }

        void ResponseImpl::set_status_code( const int value )
        {
            m_status_code = value;
        }

        void ResponseImpl::set_header( const string& name, const string& value )
        {
            m_headers[ name ] = value;
        }

        void ResponseImpl::set_headers( const map< std::string, string >& values )
        {
            m_headers = values;
        }

        bool ResponseImpl::operator <( const ResponseImpl& rhs ) const
        {
            return m_body < rhs.m_body;
        }
        
        bool ResponseImpl::operator >( const ResponseImpl& rhs ) const
        {
            return m_body > rhs.m_body;
        }
        
        bool ResponseImpl::operator ==( const ResponseImpl& rhs ) const
        {
            return m_body == rhs.m_body and m_headers == rhs.m_headers;
        }
        
        bool ResponseImpl::operator !=( const ResponseImpl& rhs ) const
        {
            return m_body not_eq rhs.m_body or m_headers not_eq rhs.m_headers;
        }
        
        ResponseImpl& ResponseImpl::operator =( const ResponseImpl& rhs )
        {
            m_body = rhs.m_body;

            m_headers = rhs.m_headers;

            return *this;
        }

        bool ResponseImpl::has_header( const string& name ) const
        {
            return ( Map::find_key_ignoring_case( name, m_headers ) not_eq m_headers.end( ) );
        }

        string ResponseImpl::generate_status_section( void ) const
        {
            string section = "HTTP/1.1 ";
            section += to_string( m_status_code );
            section += " ";
            section += StatusCode::to_string( m_status_code );
            section += "\r\n";

            return section;
        }

        string ResponseImpl::generate_header_section( void ) const
        {
            string section = generate_default_date_header( );
            section += generate_default_connection_header( );
            section += generate_default_server_header( );
            section += generate_default_content_type_header( );
            section += generate_default_content_length_header( );

            for ( auto header : m_headers )
            {
                section += header.first + ": " + header.second + "\r\n";
            }

            return section;
        }

        string ResponseImpl::generate_default_date_header( void ) const
        {
            string section = String::empty;

            if ( not has_header( "Date" ) )
            {
                time_t time = system_clock::to_time_t( system_clock::now( ) );

                section = "Date: " + string( ctime( &time ) );
                section.erase( section.length( ) - 1 );

                section += "\r\n";
            }

            return section;
        }

        string ResponseImpl::generate_default_server_header( void ) const
        {
            string section = String::empty;

            if ( not has_header( "Server" ) )
            {
                section = "Server: Corvusoft - restbed/1.0\r\n";
            }

            return section;
        }
        
        string ResponseImpl::generate_default_connection_header( void ) const
        {
            string section = String::empty;

            if ( not has_header( "Connection" ) )
            {
                section = "Connection: close\r\n";
            }

            return section;
        }

        string ResponseImpl::generate_default_content_type_header( void ) const
        {
            string section = String::empty;

            if ( not has_header( "Content-Type" ) )
            {
                section = "Content-Type: application/json; charset=utf-8\r\n";
            }

            return section;
        }

        string ResponseImpl::generate_default_content_length_header( void ) const
        {
            string section = String::empty;

            if ( not has_header( "Content-Length" ) )
            {
                section = "Content-Length: " + to_string( m_body.size( ) ) + "\r\n";
            }

            return section;
        }
    }
}
