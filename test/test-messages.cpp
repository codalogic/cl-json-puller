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

#include "cl-json-pull/cl-json-pull.h"   // Put file under test first to verify dependencies

#include "clunit.h"

#include "test-harness.h"

struct ExpectedEvent
{
    cljp::Parser::Status status;
    cljp::Event::Type type;
    std::string name;
    std::string value;
};

void test_message_sequence( const char * message, ExpectedEvent events[] )
{
    Harness h( message );

    for( size_t i=0; ; ++i )
    {
        cljp::Parser::Status status = h.parser.get( &h.event );

        TCRITICALTEST( status == events[i].status );

        if( status == cljp::Parser::PS_END_OF_MESSAGE )
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
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "false";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = " [false] ";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = " [[false]] ";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ [ false ] ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { \"tag1\" : false } ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "tag1", "false"},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { } ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { } , false ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { } , {} ]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[ { } , [] ]  \t\r\n ";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{ \"tag1\" : [ false ] }";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "tag1", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{}";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{  }";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{ \"tag1\" : [ false ], \"tag2\" : 12 }";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "tag1", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_NUMBER, "tag2", "12"},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{ \"tag1\" : [ false ], \"tag2\" : { \"tag3\":1}}";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "tag1", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "tag2", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_NUMBER, "tag3", "1"},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "[{\"tag1\" : [ false ]},{ \"tag2\" : { \"tag3\":1}}]";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_START, "tag1", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_BOOLEAN, "", "false"},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "tag2", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_NUMBER, "tag3", "1"},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_ARRAY_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }

    {
    const char * p_message = "{  } \"Ignored\" ";
    ExpectedEvent events[] = {
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_START, "", ""},
                    {cljp::Parser::PS_OK, cljp::Event::T_OBJECT_END, "", ""},
                    {cljp::Parser::PS_END_OF_MESSAGE, cljp::Event::T_UNKNOWN, "", ""}
                };

    TCALL( test_message_sequence( p_message, events ) );
    }
}

TFEATURE( "Reading multiple messages in single stream" )
{
    {
    Harness h( "\"Message 1\"      \"Message 2\"" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_OK );
    TTEST( h.event.type == cljp::Event::T_STRING );
    TTEST( h.event.value == "Message 1" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );

    // Repeated reads at end of message return 'End of message' without reading more of the stream
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );

    // Read second message
    h.parser.new_message();

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_OK );
    TTEST( h.event.type == cljp::Event::T_STRING );
    TTEST( h.event.value == "Message 2" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );

    // Read non-existent third message - Telling it to start reading a new message that isn't there
    // statuss in "end of message"
    h.parser.new_message();

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );
    }

    {
    Harness h( " { \"rate\": 15 }{ \"rate\" :10}   " );     // Includes variation of whitespace to make life trickier!

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_OK );
    TTEST( h.event.type == cljp::Event::T_OBJECT_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_OK );
    TTEST( h.event.type == cljp::Event::T_NUMBER );
    TTEST( h.event.name == "rate" );
    TTEST( h.event.value == "15" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_OK );
    TTEST( h.event.type == cljp::Event::T_OBJECT_END );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );

    // Repeated reads at end of message return 'End of message' without reading more of the stream
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );

    // Read second message
    h.parser.new_message();

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_OK );
    TTEST( h.event.type == cljp::Event::T_OBJECT_START );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_OK );
    TTEST( h.event.type == cljp::Event::T_NUMBER );
    TTEST( h.event.name == "rate" );
    TTEST( h.event.value == "10" );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_OK );
    TTEST( h.event.type == cljp::Event::T_OBJECT_END );

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );
    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );

    // Read non-existent third message - Telling it to start reading a new message that isn't there
    // statuss in "end of message"
    h.parser.new_message();

    TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_END_OF_MESSAGE );
    }
}

void test_invalid_message( const char * message, cljp::Parser::Status expected_final_status )
{
    Harness h( message );

    for(;;)
    {
        cljp::Parser::Status status = h.parser.get( &h.event );

        TCRITICALTEST( status != cljp::Parser::PS_END_OF_MESSAGE );

        if( status != cljp::Parser::PS_OK )
        {
            TTEST( status == expected_final_status );

            // Further attempts to read will get PS_UNABLE_TO_CONTINUE_DUE_TO_ERRORS
            TTEST( h.parser.get( &h.event ) == cljp::Parser::PS_UNABLE_TO_CONTINUE_DUE_TO_ERRORS );

            return;
        }
    }
}

TFEATURE( "Parser: illegally formed messages" )
{
    // Note that truncated messages are tested elsewhere

    // In this routine, test that we get every error code returned.  Note for the
    // PS_BAD_FORMAT_XXX codes, more extensive testing is included in test-parse.cpp.
    //
    //  PS_UNEXPECTED_END_OF_MESSAGE - Tested in truncated message tests elsewhere
    //  PS_EXPECTED_COLON_NAME_SEPARATOR
    //  PS_UNEXPECTED_OBJECT_CLOSE
    //  PS_UNEXPECTED_ARRAY_CLOSE
    //  PS_EXPECTED_COMMA_OR_END_OF_ARRAY
    //  PS_EXPECTED_COMMA_OR_END_OF_OBJECT
    //  PS_UNRECOGNISED_VALUE_FORMAT
    //  PS_BAD_FORMAT_STRING
    //  PS_BAD_FORMAT_FALSE
    //  PS_BAD_FORMAT_TRUE
    //  PS_BAD_FORMAT_NULL
    //  PS_BAD_FORMAT_NUMBER
    //  PS_BAD_UNICODE_ESCAPE
    //  PS_EXPECTED_MEMBER_NAME

    TCALL( test_invalid_message( "Illegal", cljp::Parser::PS_UNRECOGNISED_VALUE_FORMAT ) );
    TCALL( test_invalid_message( " fals ", cljp::Parser::PS_BAD_FORMAT_FALSE ) );
    TCALL( test_invalid_message( " falsey ", cljp::Parser::PS_BAD_FORMAT_FALSE ) );
    TCALL( test_invalid_message( " truey ", cljp::Parser::PS_BAD_FORMAT_TRUE ) );
    TCALL( test_invalid_message( " nully ", cljp::Parser::PS_BAD_FORMAT_NULL ) );
    TCALL( test_invalid_message( " falsey ", cljp::Parser::PS_BAD_FORMAT_FALSE ) );
    TCALL( test_invalid_message( " 1.0.0 ", cljp::Parser::PS_BAD_FORMAT_NUMBER ) );
    TCALL( test_invalid_message( " \"abc\\u00xxabc\" ", cljp::Parser::PS_BAD_UNICODE_ESCAPE ) );
    TCALL( test_invalid_message( " \"abc\\qabc\" ", cljp::Parser::PS_BAD_FORMAT_STRING ) );

    TCALL( test_invalid_message( " } ", cljp::Parser::PS_UNEXPECTED_OBJECT_CLOSE ) );
    TCALL( test_invalid_message( " ] ", cljp::Parser::PS_UNEXPECTED_ARRAY_CLOSE ) );

    TCALL( test_invalid_message( " [ fals ", cljp::Parser::PS_BAD_FORMAT_FALSE ) );
    TCALL( test_invalid_message( " [[false]} ", cljp::Parser::PS_UNEXPECTED_OBJECT_CLOSE ) );

    TCALL( test_invalid_message( " { \"tag1\" : fals } ", cljp::Parser::PS_BAD_FORMAT_FALSE ) );
    TCALL( test_invalid_message( " { tag1 : false } ", cljp::Parser::PS_EXPECTED_MEMBER_NAME ) );
    TCALL( test_invalid_message( " { 10 : false } ", cljp::Parser::PS_EXPECTED_MEMBER_NAME ) );
    TCALL( test_invalid_message( " { \"tag1\" } ", cljp::Parser::PS_EXPECTED_COLON_NAME_SEPARATOR ) );
    TCALL( test_invalid_message( " { \"tag1\" 10 } ", cljp::Parser::PS_EXPECTED_COLON_NAME_SEPARATOR ) );
    TCALL( test_invalid_message( " { \"tag1\" : false  true } ", cljp::Parser::PS_EXPECTED_COMMA_OR_END_OF_OBJECT ) );
    TCALL( test_invalid_message( " { \"tag1\" : false  \"tag2\" } ", cljp::Parser::PS_EXPECTED_COMMA_OR_END_OF_OBJECT ) );
    TCALL( test_invalid_message( " { \"tag1\" : false : \"tag2\" } ", cljp::Parser::PS_EXPECTED_COMMA_OR_END_OF_OBJECT ) );
    TCALL( test_invalid_message( " { \"tag1\" : false { \"tag2\" } ", cljp::Parser::PS_EXPECTED_COMMA_OR_END_OF_OBJECT ) );
    TCALL( test_invalid_message( " { \"tag1\" : false ] ", cljp::Parser::PS_UNEXPECTED_ARRAY_CLOSE ) );

    TCALL( test_invalid_message( "[ { \"tag1\" = false } ]", cljp::Parser::PS_EXPECTED_COLON_NAME_SEPARATOR ) );
    TCALL( test_invalid_message( "[ {},[],{ ]", cljp::Parser::PS_UNEXPECTED_ARRAY_CLOSE ) );
    TCALL( test_invalid_message( "[ [{}, 12, .],{} ]", cljp::Parser::PS_BAD_FORMAT_NUMBER ) );
    TCALL( test_invalid_message( "[ {},[], ]", cljp::Parser::PS_UNEXPECTED_ARRAY_CLOSE ) );
}

TFEATURE( "Parser truncated input" )
{
    //const char * p_message = "[ { \"tag1\" : false } ]";
    TCALL( test_invalid_message( "[ ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { \"", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { \"ta", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { \"tag1\" ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { \"tag1\" :", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { \"tag1\" : fal", cljp::Parser::PS_BAD_FORMAT_FALSE ) );
    TCALL( test_invalid_message( "[ { \"tag1\" : false", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { \"tag1\" : false }", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );

    //const char * p_message = "[ { } , {} ]";
    TCALL( test_invalid_message( "[ { }", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { } , ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { } , {", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "[ { } , {}", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );

    //const char * p_message = "{ \"tag1\" : [ false ], \"tag2\" : 12 }";
    TCALL( test_invalid_message( "{", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"  ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"ta", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\"", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" :", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ fal", cljp::Parser::PS_BAD_FORMAT_FALSE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ]", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ], ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ], \"", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ], \"ta", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ], \"tag2\"", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ], \"tag2\"  ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ], \"tag2\" : ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ], \"tag2\" : 12", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
    TCALL( test_invalid_message( "{ \"tag1\" : [ false ], \"tag2\" : 12 ", cljp::Parser::PS_UNEXPECTED_END_OF_MESSAGE ) );
}
