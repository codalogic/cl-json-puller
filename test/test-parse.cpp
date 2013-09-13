//----------------------------------------------------------------------------
// Copyright (c) 2013, Codalogic Ltd (http://www.codalogic.com)
// All rights reserved.
//
// The license for this file is based on the BSD-3-Clause license
// (http://www.opensource.org/licenses/BSD-3-Clause).
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// - Neither the name Codalogic Ltd nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------------

#include "clunit.h"

#include "cl-json-puller.h"

#include <string>

struct Harness
{
    std::string json;
    cljp::ReaderString reader;
    cljp::Parser parser;
    cljp::Event event;

    Harness( const std::string & r_json_in )
        :
        json( r_json_in ),
        reader( json ),
        parser( reader )
    {}
};

TFEATURE( "Basic Parser" )
{
	TDOC( "Testing outer parsing handling" );

    {
    Harness h( "{" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_OBJECT_START );
    }

    {
    Harness h( " [" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_START );
    }

    {
    Harness h( "[ ]" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_END );
    }

    {
    Harness h( "[ ]" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_END );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );
    }

    {
    Harness h( "[ ]]" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_END );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_READ_PAST_END_OF_MESSAGE );
    }

    {
    Harness h( "[ }" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_UNEXPECTED_OBJECT_CLOSE );
    }

    {
    Harness h( " {]" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_OBJECT_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_UNEXPECTED_ARRAY_CLOSE );
    }

	TDOC( "Parser::get_in_array();" );
    {
    Harness h( "[{}]" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_OBJECT_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_OBJECT_END );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_END );
    }

    {
    Harness h( "[{},{}]" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_OBJECT_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_OBJECT_END );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_OBJECT_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_OBJECT_END );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type() == cljp::Event::T_ARRAY_END );
    }
    
	TTODO( "Parser::get_in_object();" );
    TTODO( "Parser::get_value();" );
}
