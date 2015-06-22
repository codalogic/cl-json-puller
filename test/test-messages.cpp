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

#include "cl-json-pull.h"   // Put file under test first to verify dependencies

#include "clunit.h"

#include "test-harness.h"

struct ExpectedEvent
{
    cljp::Parser::ParserResult result;
    cljp::Event::Type type;
    std::string name;
    std::string value;
};

void test_message_sequence( const char * message, ExpectedEvent events[] )
{
    Harness h( message );

    for( size_t i=0; ; ++i )
    {
        cljp::Parser::ParserResult result = h.parser.get( &h.event );

        TCRITICALTEST( result == events[i].result );

        if( result == cljp::Parser::PR_END_OF_MESSAGE )
            return;

        TCRITICALTEST( h.event.type == events[i].type );
        TCRITICALTEST( h.event.name == events[i].name );
        TCRITICALTEST( h.event.value == events[i].value );
    }
}

TFEATURE( "Reading whole messages" )
{
    {
    const char * p_message = "";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "false";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = " [false] ";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = " [[false]] ";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ [ false ] ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { \"tag1\" : false } ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_BOOLEAN, "tag1", "false"},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { } ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { } , false ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { } , {} ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { } , [] ]  \t\r\n ";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{ \"tag1\" : [ false ] }";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "tag1", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{}";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{  }";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{ \"tag1\" : [ false ], \"tag2\" : 12 }";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "tag1", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_NUMBER, "tag2", "12"},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{ \"tag1\" : [ false ], \"tag2\" : { \"tag3\":1}}";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_START, "tag1", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PR_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "tag2", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_NUMBER, "tag3", "1"},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{  } \"Ignored\" ";
    ExpectedEvent events[] = {
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PR_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PR_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }
}

TFEATURE( "Reading multiple messages in single stream" )
{
    {
    Harness h( "\"Message 1\"      \"Message 2\"" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type == cljp::Event::T_STRING );
    TTEST( h.event.value == "Message 1" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );

    // Repeated reads at end of message return 'End of message' without reading more of the stream
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );

    // Read second message
    h.parser.new_message();

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type == cljp::Event::T_STRING );
    TTEST( h.event.value == "Message 2" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );

    // Read non-existent third message - Telling it to start reading a new message that isn't there
    // results in "end of message"
    h.parser.new_message();

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );
    }

    {
    Harness h( " { \"rate\": 15 }{ \"rate\" :10}   " );     // Includes variation of whitespace to make life trickier!

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type == cljp::Event::T_OBJECT_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type == cljp::Event::T_NUMBER );
    TTEST( h.event.name == "rate" );
    TTEST( h.event.value == "15" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type == cljp::Event::T_OBJECT_END );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );

    // Repeated reads at end of message return 'End of message' without reading more of the stream
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );

    // Read second message
    h.parser.new_message();

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type == cljp::Event::T_OBJECT_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type == cljp::Event::T_NUMBER );
    TTEST( h.event.name == "rate" );
    TTEST( h.event.value == "10" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_OK );
    TTEST( h.event.type == cljp::Event::T_OBJECT_END );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );

    // Read non-existent third message - Telling it to start reading a new message that isn't there
    // results in "end of message"
    h.parser.new_message();

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_END_OF_MESSAGE );
    }
}

void test_invalid_message( const char * message, cljp::Parser::ParserResult expected_final_result )
{
    Harness h( message );

    for(;;)
    {
        cljp::Parser::ParserResult result = h.parser.get( &h.event );
        
		TCRITICALTEST( result != cljp::Parser::PR_END_OF_MESSAGE );

        if( result != cljp::Parser::PR_OK )
        {
			TTEST( result == expected_final_result );

			// Further attempts to read will get PR_UNABLE_TO_CONTINUE_DUE_TO_ERRORS
			TTEST( h.parser.get( &h.event ) == cljp::Parser::PR_UNABLE_TO_CONTINUE_DUE_TO_ERRORS );

			return;
        }
    }
}

TFEATURE( "Parser: illegally formed messages" )
{
	// Note that truncated messages are tested elsewhere

	TCALL( test_invalid_message( "Illegal", cljp::Parser::PR_UNRECOGNISED_VALUE_FORMAT ) );
	TCALL( test_invalid_message( " fals ", cljp::Parser::PR_BAD_FORMAT_FALSE ) );
	TCALL( test_invalid_message( " falsey ", cljp::Parser::PR_BAD_FORMAT_FALSE ) );
	TCALL( test_invalid_message( " } ", cljp::Parser::PR_UNEXPECTED_OBJECT_CLOSE ) );
	TCALL( test_invalid_message( " ] ", cljp::Parser::PR_UNEXPECTED_ARRAY_CLOSE ) );

	TCALL( test_invalid_message( " [  ", cljp::Parser::PR_UNEXPECTED_END_OF_MESSAGE ) );
	TCALL( test_invalid_message( " [ fals ", cljp::Parser::PR_BAD_FORMAT_FALSE ) );
	TCALL( test_invalid_message( " [[false]} ", cljp::Parser::PR_UNEXPECTED_OBJECT_CLOSE ) );

	TCALL( test_invalid_message( " { \"tag1\" : fals } ", cljp::Parser::PR_BAD_FORMAT_FALSE ) );
	TCALL( test_invalid_message( " { tag1 : false } ", cljp::Parser::PR_EXPECTED_MEMBER_NAME ) );
	TCALL( test_invalid_message( " { 10 : false } ", cljp::Parser::PR_EXPECTED_MEMBER_NAME ) );
	TCALL( test_invalid_message( " { \"tag1\" } ", cljp::Parser::PR_EXPECTED_COLON_NAME_SEPARATOR ) );
	TCALL( test_invalid_message( " { \"tag1\" 10 } ", cljp::Parser::PR_EXPECTED_COLON_NAME_SEPARATOR ) );
	TCALL( test_invalid_message( " { \"tag1\" : false  true } ", cljp::Parser::PR_EXPECTED_COMMA_OR_END_OF_OBJECT ) );
	TCALL( test_invalid_message( " { \"tag1\" : false  \"tag2\" } ", cljp::Parser::PR_EXPECTED_COMMA_OR_END_OF_OBJECT ) );
	TCALL( test_invalid_message( " { \"tag1\" : false : \"tag2\" } ", cljp::Parser::PR_EXPECTED_COMMA_OR_END_OF_OBJECT ) );
	TCALL( test_invalid_message( " { \"tag1\" : false { \"tag2\" } ", cljp::Parser::PR_EXPECTED_COMMA_OR_END_OF_OBJECT ) );
	TCALL( test_invalid_message( " { \"tag1\" : false ] ", cljp::Parser::PR_UNEXPECTED_ARRAY_CLOSE ) );

    //const char * p_message = "[ { \"tag1\" : false } ]";
    //const char * p_message = "[ { } ]";
    //const char * p_message = "[ { } , false ]";
    //const char * p_message = "[ { } , {} ]";
    //const char * p_message = "[ { } , [] ]  \t\r\n ";
    //const char * p_message = "{ \"tag1\" : [ false ] }";
    //const char * p_message = "{}";
    //const char * p_message = "{  }";
    //const char * p_message = "{ \"tag1\" : [ false ], \"tag2\" : 12 }";
    //const char * p_message = "{ \"tag1\" : [ false ], \"tag2\" : { \"tag3\":1}}";

	// Ensure we get the following ocdes at some point:
    //PR_BAD_FORMAT_STRING,
    //PR_BAD_FORMAT_TRUE,
    //PR_BAD_FORMAT_NULL,
    //PR_BAD_FORMAT_NUMBER,
    //PR_BAD_UNICODE_ESCAPE,

    TTODO( "More Test illegally formed messages" );
}

TFEATURE( "Parser truncated input" )
{
	TCALL( test_invalid_message( "{", cljp::Parser::PR_UNEXPECTED_END_OF_MESSAGE ) );

    TTODO( "More Parser truncated input" );
}
