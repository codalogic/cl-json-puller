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

#include "cl-json-puller.h"

namespace cljp {	// Codalogic JSON Puller

//----------------------------------------------------------------------------
//                             class Reader
//----------------------------------------------------------------------------

const int Reader::EOM = -1;

int Reader::get()
{
	if( ! m.unget_buffer.empty() )
	{
		int result = m.unget_buffer.back();
		m.unget_buffer.pop_back();
		return result;
	}
	
	return get_new();
}

void Reader::unget( int c )
{
	m.unget_buffer.push_back( c );
}

//----------------------------------------------------------------------------
//                             class ReaderMemory
//----------------------------------------------------------------------------

ReaderMemory::ReaderMemory( const char * p_start_in, const char * p_end_in )
	: m( p_start_in, p_end_in )
{
}

int ReaderMemory::get_new()
{
	if( m.p_now < m.p_end )
		return static_cast< unsigned char >( *m.p_now++ );
	return EOM;
}

void ReaderMemory::rewind()
{
	m.p_start = m.p_now;
}

}	// End of namespace cljp
