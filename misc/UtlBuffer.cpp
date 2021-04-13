//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// $Header: $
// $NoKeywords: $
//
// Serialization buffer
//===========================================================================//

#pragma warning (disable : 4514)

#include "UtlBuffer.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "characterset.hpp"

const char* V_strnchr(const char* pStr, char c, int n)
{
    char const* pLetter = pStr;
    char const* pLast = pStr + n;

    // Check the entire string
    while((pLetter < pLast) && (*pLetter != 0)) {
        if(*pLetter == c)
            return pLetter;
        ++pLetter;
    }
    return NULL;
}
//-----------------------------------------------------------------------------
// Finds a string in another string with a case insensitive test w/ length validation
//-----------------------------------------------------------------------------
char const* V_strnistr(char const* pStr, char const* pSearch, int n)
{
    if(!pStr || !pSearch)
        return 0;

    char const* pLetter = pStr;

    // Check the entire string
    while(*pLetter != 0) {
        if(n <= 0)
            return 0;

        // Skip over non-matches
        if(tolower(*pLetter) == tolower(*pSearch)) {
            int n1 = n - 1;

            // Check for match
            char const* pMatch = pLetter + 1;
            char const* pTest = pSearch + 1;
            while(*pTest != 0) {
                if(n1 <= 0)
                    return 0;

                // We've run off the end; don't bother.
                if(*pMatch == 0)
                    return 0;

                if(tolower(*pMatch) != tolower(*pTest))
                    break;

                ++pMatch;
                ++pTest;
                --n1;
            }

            // Found a match!
            if(*pTest == 0)
                return pLetter;
        }

        ++pLetter;
        --n;
    }

    return 0;
}
//-----------------------------------------------------------------------------
// Character conversions for C strings
//-----------------------------------------------------------------------------
class CUtlCStringConversion : public CUtlCharConversion
{
public:
    CUtlCStringConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray);

    // Finds a conversion for the passed-in string, returns length
    virtual char FindConversion(const char *pString, int *pLength);

private:
    char m_pConversion[255];
};


//-----------------------------------------------------------------------------
// Character conversions for no-escape sequence strings
//-----------------------------------------------------------------------------
class CUtlNoEscConversion : public CUtlCharConversion
{
public:
    CUtlNoEscConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray) :
        CUtlCharConversion(nEscapeChar, pDelimiter, nCount, pArray)
    {
    }

    // Finds a conversion for the passed-in string, returns length
    virtual char FindConversion(const char *pString, int *pLength) { *pLength = 0; return 0; }
};


//-----------------------------------------------------------------------------
// List of character conversions
//-----------------------------------------------------------------------------
BEGIN_CUSTOM_CHAR_CONVERSION(CUtlCStringConversion, s_StringCharConversion, "\"", '\\')
{
    '\n', "n"
},
{ '\t', "t" },
{ '\v', "v" },
{ '\b', "b" },
{ '\r', "r" },
{ '\f', "f" },
{ '\a', "a" },
{ '\\', "\\" },
{ '\?', "\?" },
{ '\'', "\'" },
{ '\"', "\"" },
END_CUSTOM_CHAR_CONVERSION(CUtlCStringConversion, s_StringCharConversion, "\"", '\\');

    CUtlCharConversion *GetCStringCharConversion()
    {
        return &s_StringCharConversion;
    }

    BEGIN_CUSTOM_CHAR_CONVERSION(CUtlNoEscConversion, s_NoEscConversion, "\"", 0x7F)
    {
        0x7F, ""
    },
        END_CUSTOM_CHAR_CONVERSION(CUtlNoEscConversion, s_NoEscConversion, "\"", 0x7F);

        CUtlCharConversion *GetNoEscCharConversion()
        {
            return &s_NoEscConversion;
        }


        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        CUtlCStringConversion::CUtlCStringConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray) :
            CUtlCharConversion(nEscapeChar, pDelimiter, nCount, pArray)
        {
            memset(m_pConversion, 0x0, sizeof(m_pConversion));
            for(int i = 0; i < nCount; ++i) {
                m_pConversion[pArray[i].m_pReplacementString[0]] = pArray[i].m_nActualChar;
            }
        }

        // Finds a conversion for the passed-in string, returns length
        char CUtlCStringConversion::FindConversion(const char *pString, int *pLength)
        {
            char c = m_pConversion[pString[0]];
            *pLength = (c != '\0') ? 1 : 0;
            return c;
        }



        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        CUtlCharConversion::CUtlCharConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray)
        {
            m_nEscapeChar = nEscapeChar;
            m_pDelimiter = pDelimiter;
            m_nCount = nCount;
            m_nDelimiterLength = strlen(pDelimiter);
            m_nMaxConversionLength = 0;

            memset(m_pReplacements, 0, sizeof(m_pReplacements));

            for(int i = 0; i < nCount; ++i) {
                m_pList[i] = pArray[i].m_nActualChar;
                ConversionInfo_t &info = m_pReplacements[m_pList[i]];
                assert(info.m_pReplacementString == 0);
                info.m_pReplacementString = pArray[i].m_pReplacementString;
                info.m_nLength = strlen(info.m_pReplacementString);
                if(info.m_nLength > m_nMaxConversionLength) {
                    m_nMaxConversionLength = info.m_nLength;
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Escape character + delimiter
        //-----------------------------------------------------------------------------
        char CUtlCharConversion::GetEscapeChar() const
        {
            return m_nEscapeChar;
        }

        const char *CUtlCharConversion::GetDelimiter() const
        {
            return m_pDelimiter;
        }

        int CUtlCharConversion::GetDelimiterLength() const
        {
            return m_nDelimiterLength;
        }


        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        const char *CUtlCharConversion::GetConversionString(char c) const
        {
            return m_pReplacements[c].m_pReplacementString;
        }

        int CUtlCharConversion::GetConversionLength(char c) const
        {
            return m_pReplacements[c].m_nLength;
        }

        int CUtlCharConversion::MaxConversionLength() const
        {
            return m_nMaxConversionLength;
        }


        //-----------------------------------------------------------------------------
        // Finds a conversion for the passed-in string, returns length
        //-----------------------------------------------------------------------------
        char CUtlCharConversion::FindConversion(const char *pString, int *pLength)
        {
            for(int i = 0; i < m_nCount; ++i) {
                if(!strcmp(pString, m_pReplacements[m_pList[i]].m_pReplacementString)) {
                    *pLength = m_pReplacements[m_pList[i]].m_nLength;
                    return m_pList[i];
                }
            }

            *pLength = 0;
            return '\0';
        }


        //-----------------------------------------------------------------------------
        // constructors
        //-----------------------------------------------------------------------------
        CUtlBuffer::CUtlBuffer(int growSize, int initSize, int nFlags) :
            m_Memory(growSize, initSize), m_Error(0)
        {
            m_Get = 0;
            m_Put = 0;
            m_nTab = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            if((initSize != 0) && !IsReadOnly()) {
                m_nMaxPut = -1;
                AddNullTermination();
            } else {
                m_nMaxPut = 0;
            }
            SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
        }

        CUtlBuffer::CUtlBuffer(const void *pBuffer, int nSize, int nFlags) :
            m_Memory((unsigned char*)pBuffer, nSize), m_Error(0)
        {
            assert(nSize != 0);

            m_Get = 0;
            m_Put = 0;
            m_nTab = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            if(IsReadOnly()) {
                m_nMaxPut = nSize;
            } else {
                m_nMaxPut = -1;
                AddNullTermination();
            }
            SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
        }


        //-----------------------------------------------------------------------------
        // Modifies the buffer to be binary or text; Blows away the buffer and the CONTAINS_CRLF value. 
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetBufferType(bool bIsText, bool bContainsCRLF)
        {
#ifdef _DEBUG
            // If the buffer is empty, there is no opportunity for this stuff to fail
            if(TellMaxPut() != 0) {
                if(IsText()) {
                    if(bIsText) {
                        assert(ContainsCRLF() == bContainsCRLF);
                    } else {
                        assert(ContainsCRLF());
                    }
                } else {
                    if(bIsText) {
                        assert(bContainsCRLF);
                    }
                }
            }
#endif

            if(bIsText) {
                m_Flags |= TEXT_BUFFER;
            } else {
                m_Flags &= ~TEXT_BUFFER;
            }
            if(bContainsCRLF) {
                m_Flags |= CONTAINS_CRLF;
            } else {
                m_Flags &= ~CONTAINS_CRLF;
            }
        }


        //-----------------------------------------------------------------------------
        // Attaches the buffer to external memory....
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetExternalBuffer(void* pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.SetExternalBuffer((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }

        //-----------------------------------------------------------------------------
        // Assumes an external buffer but manages its deletion
        //-----------------------------------------------------------------------------
        void CUtlBuffer::AssumeMemory(void *pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.AssumeMemory((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }

        //-----------------------------------------------------------------------------
        // Makes sure we've got at least this much memory
        //-----------------------------------------------------------------------------
        void CUtlBuffer::EnsureCapacity(int num)
        {
            // Add one extra for the null termination
            num += 1;
            if(m_Memory.IsExternallyAllocated()) {
                if(IsGrowable() && (m_Memory.NumAllocated() < num)) {
                    m_Memory.ConvertToGrowableMemory(0);
                } else {
                    num -= 1;
                }
            }

            m_Memory.EnsureCapacity(num);
        }


        //-----------------------------------------------------------------------------
        // Base Get method from which all others derive
        //-----------------------------------------------------------------------------
        void CUtlBuffer::Get(void* pMem, int size)
        {
            if(CheckGet(size)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], size);
                m_Get += size;
            }
        }


        //-----------------------------------------------------------------------------
        // This will Get at least 1 uint8_t and up to nSize bytes. 
        // It will return the number of bytes actually read.
        //-----------------------------------------------------------------------------
        int CUtlBuffer::GetUpTo(void *pMem, int nSize)
        {
            if(CheckArbitraryPeekGet(0, nSize)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], nSize);
                m_Get += nSize;
                return nSize;
            }
            return 0;
        }


        //-----------------------------------------------------------------------------
        // Eats whitespace
        //-----------------------------------------------------------------------------
        void CUtlBuffer::EatWhiteSpace()
        {
            if(IsText() && IsValid()) {
                while(CheckGet(sizeof(char))) {
                    if(!isspace(*(const unsigned char*)PeekGet()))
                        break;
                    m_Get += sizeof(char);
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Eats C++ style comments
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::EatCPPComment()
        {
            if(IsText() && IsValid()) {
                // If we don't have a a c++ style comment next, we're done
                const char *pPeek = (const char *)PeekGet(2 * sizeof(char), 0);
                if(!pPeek || (pPeek[0] != '/') || (pPeek[1] != '/'))
                    return false;

                // Deal with c++ style comments
                m_Get += 2;

                // read complete line
                for(char c = GetChar(); IsValid(); c = GetChar()) {
                    if(c == '\n')
                        break;
                }
                return true;
            }
            return false;
        }


        //-----------------------------------------------------------------------------
        // Peeks how much whitespace to eat
        //-----------------------------------------------------------------------------
        int CUtlBuffer::PeekWhiteSpace(int nOffset)
        {
            if(!IsText() || !IsValid())
                return 0;

            while(CheckPeekGet(nOffset, sizeof(char))) {
                if(!isspace(*(unsigned char*)PeekGet(nOffset)))
                    break;
                nOffset += sizeof(char);
            }

            return nOffset;
        }


        //-----------------------------------------------------------------------------
        // Peek size of sting to come, check memory bound
        //-----------------------------------------------------------------------------
        int	CUtlBuffer::PeekStringLength()
        {
            if(!IsValid())
                return 0;

            // Eat preceeding whitespace
            int nOffset = 0;
            if(IsText()) {
                nOffset = PeekWhiteSpace(nOffset);
            }

            int nStartingOffset = nOffset;

            do {
                int nPeekAmount = 128;

                // NOTE: Add 1 for the terminating zero!
                if(!CheckArbitraryPeekGet(nOffset, nPeekAmount)) {
                    if(nOffset == nStartingOffset)
                        return 0;
                    return nOffset - nStartingOffset + 1;
                }

                const char *pTest = (const char *)PeekGet(nOffset);

                if(!IsText()) {
                    for(int i = 0; i < nPeekAmount; ++i) {
                        // The +1 here is so we eat the terminating 0
                        if(pTest[i] == 0)
                            return (i + nOffset - nStartingOffset + 1);
                    }
                } else {
                    for(int i = 0; i < nPeekAmount; ++i) {
                        // The +1 here is so we eat the terminating 0
                        if(isspace((unsigned char)pTest[i]) || (pTest[i] == 0))
                            return (i + nOffset - nStartingOffset + 1);
                    }
                }

                nOffset += nPeekAmount;

            } while(true);
        }


        //-----------------------------------------------------------------------------
        // Peek size of line to come, check memory bound
        //-----------------------------------------------------------------------------
        int	CUtlBuffer::PeekLineLength()
        {
            if(!IsValid())
                return 0;

            int nOffset = 0;
            int nStartingOffset = nOffset;

            do {
                int nPeekAmount = 128;

                // NOTE: Add 1 for the terminating zero!
                if(!CheckArbitraryPeekGet(nOffset, nPeekAmount)) {
                    if(nOffset == nStartingOffset)
                        return 0;
                    return nOffset - nStartingOffset + 1;
                }

                const char *pTest = (const char *)PeekGet(nOffset);

                for(int i = 0; i < nPeekAmount; ++i) {
                    // The +2 here is so we eat the terminating '\n' and 0
                    if(pTest[i] == '\n' || pTest[i] == '\r')
                        return (i + nOffset - nStartingOffset + 2);
                    // The +1 here is so we eat the terminating 0
                    if(pTest[i] == 0)
                        return (i + nOffset - nStartingOffset + 1);
                }

                nOffset += nPeekAmount;

            } while(true);
        }


        //-----------------------------------------------------------------------------
        // Does the next bytes of the buffer match a pattern?
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::PeekStringMatch(int nOffset, const char *pString, int nLen)
        {
            if(!CheckPeekGet(nOffset, nLen))
                return false;
            return !strncmp((const char*)PeekGet(nOffset), pString, nLen);
        }


        //-----------------------------------------------------------------------------
        // This version of PeekStringLength converts \" to \\ and " to \, etc.
        // It also reads a " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        int CUtlBuffer::PeekDelimitedStringLength(CUtlCharConversion *pConv, bool bActualSize)
        {
            if(!IsText() || !pConv)
                return PeekStringLength();

            // Eat preceeding whitespace
            int nOffset = 0;
            if(IsText()) {
                nOffset = PeekWhiteSpace(nOffset);
            }

            if(!PeekStringMatch(nOffset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                return 0;

            // Try to read ending ", but don't accept \"
            int nActualStart = nOffset;
            nOffset += pConv->GetDelimiterLength();
            int nLen = 1;	// Starts at 1 for the '\0' termination

            do {
                if(PeekStringMatch(nOffset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                    break;

                if(!CheckPeekGet(nOffset, 1))
                    break;

                char c = *(const char*)PeekGet(nOffset);
                ++nLen;
                ++nOffset;
                if(c == pConv->GetEscapeChar()) {
                    int nLength = pConv->MaxConversionLength();
                    if(!CheckArbitraryPeekGet(nOffset, nLength))
                        break;

                    pConv->FindConversion((const char*)PeekGet(nOffset), &nLength);
                    nOffset += nLength;
                }
            } while(true);

            return bActualSize ? nLen : nOffset - nActualStart + pConv->GetDelimiterLength() + 1;
        }


        //-----------------------------------------------------------------------------
        // Reads a null-terminated string
        //-----------------------------------------------------------------------------
        void CUtlBuffer::GetString(char* pString, int nMaxChars)
        {
            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekStringLength();

            if(IsText()) {
                EatWhiteSpace();
            }

            if(nLen == 0) {
                *pString = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pString, nLen - 1);
                pString[nLen - 1] = 0;
            } else {
                Get(pString, nMaxChars - 1);
                pString[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }

            // Read the terminating NULL in binary formats
            if(!IsText()) {
                assert(GetChar() == 0);
            }
        }


        //-----------------------------------------------------------------------------
        // Reads up to and including the first \n
        //-----------------------------------------------------------------------------
        void CUtlBuffer::GetLine(char* pLine, int nMaxChars)
        {
            assert(IsText() && !ContainsCRLF());

            if(!IsValid()) {
                *pLine = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekLineLength();
            if(nLen == 0) {
                *pLine = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pLine, nLen - 1);
                pLine[nLen - 1] = 0;
            } else {
                Get(pLine, nMaxChars - 1);
                pLine[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }
        }


        //-----------------------------------------------------------------------------
        // This version of GetString converts \ to \\ and " to \", etc.
        // It also places " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        char CUtlBuffer::GetDelimitedCharInternal(CUtlCharConversion *pConv)
        {
            char c = GetChar();
            if(c == pConv->GetEscapeChar()) {
                int nLength = pConv->MaxConversionLength();
                if(!CheckArbitraryPeekGet(0, nLength))
                    return '\0';

                c = pConv->FindConversion((const char *)PeekGet(), &nLength);
                SeekGet(SEEK_CURRENT, nLength);
            }

            return c;
        }

        char CUtlBuffer::GetDelimitedChar(CUtlCharConversion *pConv)
        {
            if(!IsText() || !pConv)
                return GetChar();
            return GetDelimitedCharInternal(pConv);
        }

        void CUtlBuffer::GetDelimitedString(CUtlCharConversion *pConv, char *pString, int nMaxChars)
        {
            if(!IsText() || !pConv) {
                GetString(pString, nMaxChars);
                return;
            }

            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            EatWhiteSpace();
            if(!PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                return;

            // Pull off the starting delimiter
            SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());

            int nRead = 0;
            while(IsValid()) {
                if(PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength())) {
                    SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());
                    break;
                }

                char c = GetDelimitedCharInternal(pConv);

                if(nRead < nMaxChars) {
                    pString[nRead] = c;
                    ++nRead;
                }
            }

            if(nRead >= nMaxChars) {
                nRead = nMaxChars - 1;
            }
            pString[nRead] = '\0';
        }


        //-----------------------------------------------------------------------------
        // Checks if a Get is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckGet(int nSize)
        {
            if(m_Error & GET_OVERFLOW)
                return false;

            if(TellMaxPut() < m_Get + nSize) {
                m_Error |= GET_OVERFLOW;
                return false;
            }

            if((m_Get < m_nOffset) || (m_Memory.NumAllocated() < m_Get - m_nOffset + nSize)) {
                if(!OnGetOverflow(nSize)) {
                    m_Error |= GET_OVERFLOW;
                    return false;
                }
            }

            return true;
        }


        //-----------------------------------------------------------------------------
        // Checks if a peek Get is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckPeekGet(int nOffset, int nSize)
        {
            if(m_Error & GET_OVERFLOW)
                return false;

            // Checking for peek can't Set the overflow flag
            bool bOk = CheckGet(nOffset + nSize);
            m_Error &= ~GET_OVERFLOW;
            return bOk;
        }


        //-----------------------------------------------------------------------------
        // Call this to peek arbitrarily long into memory. It doesn't fail unless
        // it can't read *anything* new
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckArbitraryPeekGet(int nOffset, int &nIncrement)
        {
            if(TellGet() + nOffset >= TellMaxPut()) {
                nIncrement = 0;
                return false;
            }

            if(TellGet() + nOffset + nIncrement > TellMaxPut()) {
                nIncrement = TellMaxPut() - TellGet() - nOffset;
            }

            // NOTE: CheckPeekGet could modify TellMaxPut for streaming files
            // We have to call TellMaxPut again here
            CheckPeekGet(nOffset, nIncrement);
            int nMaxGet = TellMaxPut() - TellGet();
            if(nMaxGet < nIncrement) {
                nIncrement = nMaxGet;
            }
            return (nIncrement != 0);
        }


        //-----------------------------------------------------------------------------
        // Peek part of the butt
        //-----------------------------------------------------------------------------
        const void* CUtlBuffer::PeekGet(int nMaxSize, int nOffset)
        {
            if(!CheckPeekGet(nOffset, nMaxSize))
                return NULL;
            return &m_Memory[m_Get + nOffset - m_nOffset];
        }


        //-----------------------------------------------------------------------------
        // Change where I'm reading
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SeekGet(SeekType_t type, int offset)
        {
            switch(type) {
                case SEEK_HEAD:
                    m_Get = offset;
                    break;

                case SEEK_CURRENT:
                    m_Get += offset;
                    break;

                case SEEK_TAIL:
                    m_Get = m_nMaxPut - offset;
                    break;
            }

            if(m_Get > m_nMaxPut) {
                m_Error |= GET_OVERFLOW;
            } else {
                m_Error &= ~GET_OVERFLOW;
                if(m_Get < m_nOffset || m_Get >= m_nOffset + Size()) {
                    OnGetOverflow(-1);
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Parse...
        //-----------------------------------------------------------------------------

#pragma warning ( disable : 4706 )

        int CUtlBuffer::VaScanf(const char* pFmt, va_list list)
        {
            assert(pFmt);
            if(m_Error || !IsText())
                return 0;

            int numScanned = 0;
            int nLength;
            char c;
            char* pEnd;
            while(c = *pFmt++) {
                // Stop if we hit the end of the buffer
                if(m_Get >= TellMaxPut()) {
                    m_Error |= GET_OVERFLOW;
                    break;
                }

                switch(c) {
                    case ' ':
                        // eat all whitespace
                        EatWhiteSpace();
                        break;

                    case '%':
                    {
                        // Conversion character... try to convert baby!
                        char type = *pFmt++;
                        if(type == 0)
                            return numScanned;

                        switch(type) {
                            case 'c':
                            {
                                char* ch = va_arg(list, char *);
                                if(CheckPeekGet(0, sizeof(char))) {
                                    *ch = *(const char*)PeekGet();
                                    ++m_Get;
                                } else {
                                    *ch = 0;
                                    return numScanned;
                                }
                            }
                            break;

                            case 'i':
                            case 'd':
                            {
                                int* i = va_arg(list, int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *i = 0;
                                    return numScanned;
                                }

                                *i = strtol((char*)PeekGet(), &pEnd, 10);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'x':
                            {
                                int* i = va_arg(list, int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *i = 0;
                                    return numScanned;
                                }

                                *i = strtol((char*)PeekGet(), &pEnd, 16);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'u':
                            {
                                unsigned int* u = va_arg(list, unsigned int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *u = 0;
                                    return numScanned;
                                }

                                *u = strtoul((char*)PeekGet(), &pEnd, 10);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'f':
                            {
                                float* f = va_arg(list, float *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *f = 0.0f;
                                    return numScanned;
                                }

                                *f = (float)strtod((char*)PeekGet(), &pEnd);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 's':
                            {
                                char* s = va_arg(list, char *);
                                GetString(s);
                            }
                            break;

                            default:
                            {
                                // unimplemented scanf type
                                assert(0);
                                return numScanned;
                            }
                            break;
                        }

                        ++numScanned;
                    }
                    break;

                    default:
                    {
                        // Here we have to match the format string character
                        // against what's in the buffer or we're done.
                        if(!CheckPeekGet(0, sizeof(char)))
                            return numScanned;

                        if(c != *(const char*)PeekGet())
                            return numScanned;

                        ++m_Get;
                    }
                }
            }
            return numScanned;
        }

#pragma warning ( default : 4706 )

        int CUtlBuffer::Scanf(const char* pFmt, ...)
        {
            va_list args;

            va_start(args, pFmt);
            int count = VaScanf(pFmt, args);
            va_end(args);

            return count;
        }


        //-----------------------------------------------------------------------------
        // Advance the Get index until after the particular string is found
        // Do not eat whitespace before starting. Return false if it failed
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::GetToken(const char *pToken)
        {
            assert(pToken);

            // Look for the token
            int nLen = strlen(pToken);

            int nSizeToCheck = Size() - TellGet() - m_nOffset;

            int nGet = TellGet();
            do {
                int nMaxSize = TellMaxPut() - TellGet();
                if(nMaxSize < nSizeToCheck) {
                    nSizeToCheck = nMaxSize;
                }
                if(nLen > nSizeToCheck)
                    break;

                if(!CheckPeekGet(0, nSizeToCheck))
                    break;

                const char *pBufStart = (const char*)PeekGet();
                const char *pFoundEnd = V_strnistr(pBufStart, pToken, nSizeToCheck);
                if(pFoundEnd) {
                    size_t nOffset = (size_t)pFoundEnd - (size_t)pBufStart;
                    SeekGet(CUtlBuffer::SEEK_CURRENT, nOffset + nLen);
                    return true;
                }

                SeekGet(CUtlBuffer::SEEK_CURRENT, nSizeToCheck - nLen - 1);
                nSizeToCheck = Size() - (nLen - 1);

            } while(true);

            SeekGet(CUtlBuffer::SEEK_HEAD, nGet);
            return false;
        }


        //-----------------------------------------------------------------------------
        // (For text buffers only)
        // Parse a token from the buffer:
        // Grab all text that lies between a starting delimiter + ending delimiter
        // (skipping whitespace that leads + trails both delimiters).
        // Note the delimiter checks are case-insensitive.
        // If successful, the Get index is advanced and the function returns true,
        // otherwise the index is not advanced and the function returns false.
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::ParseToken(const char *pStartingDelim, const char *pEndingDelim, char* pString, int nMaxLen)
        {
            int nCharsToCopy = 0;
            int nCurrentGet = 0;

            size_t nEndingDelimLen;

            // Starting delimiter is optional
            char emptyBuf = '\0';
            if(!pStartingDelim) {
                pStartingDelim = &emptyBuf;
            }

            // Ending delimiter is not
            assert(pEndingDelim && pEndingDelim[0]);
            nEndingDelimLen = strlen(pEndingDelim);

            int nStartGet = TellGet();
            char nCurrChar;
            int nTokenStart = -1;
            EatWhiteSpace();
            while(*pStartingDelim) {
                nCurrChar = *pStartingDelim++;
                if(!isspace((unsigned char)nCurrChar)) {
                    if(tolower(GetChar()) != tolower(nCurrChar))
                        goto parseFailed;
                } else {
                    EatWhiteSpace();
                }
            }

            EatWhiteSpace();
            nTokenStart = TellGet();
            if(!GetToken(pEndingDelim))
                goto parseFailed;

            nCurrentGet = TellGet();
            nCharsToCopy = (nCurrentGet - nEndingDelimLen) - nTokenStart;
            if(nCharsToCopy >= nMaxLen) {
                nCharsToCopy = nMaxLen - 1;
            }

            if(nCharsToCopy > 0) {
                SeekGet(CUtlBuffer::SEEK_HEAD, nTokenStart);
                Get(pString, nCharsToCopy);
                if(!IsValid())
                    goto parseFailed;

                // Eat trailing whitespace
                for(; nCharsToCopy > 0; --nCharsToCopy) {
                    if(!isspace((unsigned char)pString[nCharsToCopy - 1]))
                        break;
                }
            }
            pString[nCharsToCopy] = '\0';

            // Advance the Get index
            SeekGet(CUtlBuffer::SEEK_HEAD, nCurrentGet);
            return true;

        parseFailed:
            // Revert the Get index
            SeekGet(SEEK_HEAD, nStartGet);
            pString[0] = '\0';
            return false;
        }


        //-----------------------------------------------------------------------------
        // Parses the next token, given a Set of character breaks to stop at
        //-----------------------------------------------------------------------------
        int CUtlBuffer::ParseToken(characterset_t *pBreaks, char *pTokenBuf, int nMaxLen, bool bParseComments)
        {
            assert(nMaxLen > 0);
            pTokenBuf[0] = 0;

            // skip whitespace + comments
            while(true) {
                if(!IsValid())
                    return -1;
                EatWhiteSpace();
                if(bParseComments) {
                    if(!EatCPPComment())
                        break;
                } else {
                    break;
                }
            }

            char c = GetChar();

            // End of buffer
            if(c == 0)
                return -1;

            // handle quoted strings specially
            if(c == '\"') {
                int nLen = 0;
                while(IsValid()) {
                    c = GetChar();
                    if(c == '\"' || !c) {
                        pTokenBuf[nLen] = 0;
                        return nLen;
                    }
                    pTokenBuf[nLen] = c;
                    if(++nLen == nMaxLen) {
                        pTokenBuf[nLen - 1] = 0;
                        return nMaxLen;
                    }
                }

                // In this case, we hit the end of the buffer before hitting the end qoute
                pTokenBuf[nLen] = 0;
                return nLen;
            }

            // parse single characters
            if(IN_CHARACTERSET(*pBreaks, c)) {
                pTokenBuf[0] = c;
                pTokenBuf[1] = 0;
                return 1;
            }

            // parse a regular word
            int nLen = 0;
            while(true) {
                pTokenBuf[nLen] = c;
                if(++nLen == nMaxLen) {
                    pTokenBuf[nLen - 1] = 0;
                    return nMaxLen;
                }
                c = GetChar();
                if(!IsValid())
                    break;

                if(IN_CHARACTERSET(*pBreaks, c) || c == '\"' || c <= ' ') {
                    SeekGet(SEEK_CURRENT, -1);
                    break;
                }
            }

            pTokenBuf[nLen] = 0;
            return nLen;
        }



        //-----------------------------------------------------------------------------
        // Serialization
        //-----------------------------------------------------------------------------
        void CUtlBuffer::Put(const void *pMem, int size)
        {
            if(size && CheckPut(size)) {
                memcpy(&m_Memory[m_Put - m_nOffset], pMem, size);
                m_Put += size;

                AddNullTermination();
            }
        }


        //-----------------------------------------------------------------------------
        // Writes a null-terminated string
        //-----------------------------------------------------------------------------
        void CUtlBuffer::PutString(const char* pString)
        {
            if(!IsText()) {
                if(pString) {
                    // Not text? append a null at the end.
                    size_t nLen = strlen(pString) + 1;
                    Put(pString, nLen * sizeof(char));
                    return;
                } else {
                    PutTypeBin<char>(0);
                }
            } else if(pString) {
                int nTabCount = (m_Flags & AUTO_TABS_DISABLED) ? 0 : m_nTab;
                if(nTabCount > 0) {
                    if(WasLastCharacterCR()) {
                        PutTabs();
                    }

                    const char* pEndl = strchr(pString, '\n');
                    while(pEndl) {
                        size_t nSize = (size_t)pEndl - (size_t)pString + sizeof(char);
                        Put(pString, nSize);
                        pString = pEndl + 1;
                        if(*pString) {
                            PutTabs();
                            pEndl = strchr(pString, '\n');
                        } else {
                            pEndl = NULL;
                        }
                    }
                }
                size_t nLen = strlen(pString);
                if(nLen) {
                    Put(pString, nLen * sizeof(char));
                }
            }
        }


        //-----------------------------------------------------------------------------
        // This version of PutString converts \ to \\ and " to \", etc.
        // It also places " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        inline void CUtlBuffer::PutDelimitedCharInternal(CUtlCharConversion *pConv, char c)
        {
            int l = pConv->GetConversionLength(c);
            if(l == 0) {
                PutChar(c);
            } else {
                PutChar(pConv->GetEscapeChar());
                Put(pConv->GetConversionString(c), l);
            }
        }

        void CUtlBuffer::PutDelimitedChar(CUtlCharConversion *pConv, char c)
        {
            if(!IsText() || !pConv) {
                PutChar(c);
                return;
            }

            PutDelimitedCharInternal(pConv, c);
        }

        void CUtlBuffer::PutDelimitedString(CUtlCharConversion *pConv, const char *pString)
        {
            if(!IsText() || !pConv) {
                PutString(pString);
                return;
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());

            int nLen = pString ? strlen(pString) : 0;
            for(int i = 0; i < nLen; ++i) {
                PutDelimitedCharInternal(pConv, pString[i]);
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());
        }


        void CUtlBuffer::VaPrintf(const char* pFmt, va_list list)
        {
            char temp[2048];
            int nLen = vsnprintf(temp, sizeof(temp), pFmt, list);
            assert(nLen < 2048);
            PutString(temp);
        }

        void CUtlBuffer::Printf(const char* pFmt, ...)
        {
            va_list args;

            va_start(args, pFmt);
            VaPrintf(pFmt, args);
            va_end(args);
        }


        //-----------------------------------------------------------------------------
        // Calls the overflow functions
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetOverflowFuncs(UtlBufferOverflowFunc_t getFunc, UtlBufferOverflowFunc_t putFunc)
        {
            m_GetOverflowFunc = getFunc;
            m_PutOverflowFunc = putFunc;
        }


        //-----------------------------------------------------------------------------
        // Calls the overflow functions
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::OnPutOverflow(int nSize)
        {
            return (this->*m_PutOverflowFunc)(nSize);
        }

        bool CUtlBuffer::OnGetOverflow(int nSize)
        {
            return (this->*m_GetOverflowFunc)(nSize);
        }


        //-----------------------------------------------------------------------------
        // Checks if a put is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::PutOverflow(int nSize)
        {
            if(m_Memory.IsExternallyAllocated()) {
                if(!IsGrowable())
                    return false;

                m_Memory.ConvertToGrowableMemory(0);
            }

            while(Size() < m_Put - m_nOffset + nSize) {
                m_Memory.Grow();
            }

            return true;
        }

        bool CUtlBuffer::GetOverflow(int nSize)
        {
            return false;
        }


        //-----------------------------------------------------------------------------
        // Checks if a put is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckPut(int nSize)
        {
            if((m_Error & PUT_OVERFLOW) || IsReadOnly())
                return false;

            if((m_Put < m_nOffset) || (m_Memory.NumAllocated() < m_Put - m_nOffset + nSize)) {
                if(!OnPutOverflow(nSize)) {
                    m_Error |= PUT_OVERFLOW;
                    return false;
                }
            }
            return true;
        }

        void CUtlBuffer::SeekPut(SeekType_t type, int offset)
        {
            int nNextPut = m_Put;
            switch(type) {
                case SEEK_HEAD:
                    nNextPut = offset;
                    break;

                case SEEK_CURRENT:
                    nNextPut += offset;
                    break;

                case SEEK_TAIL:
                    nNextPut = m_nMaxPut - offset;
                    break;
            }

            // Force a write of the data
            // FIXME: We could make this more optimal potentially by writing out
            // the entire buffer if you seek outside the current range

            // NOTE: This call will write and will also seek the file to nNextPut.
            OnPutOverflow(-nNextPut - 1);
            m_Put = nNextPut;

            AddNullTermination();
        }


        void CUtlBuffer::ActivateByteSwapping(bool bActivate)
        {
            m_Byteswap.ActivateByteSwapping(bActivate);
        }

        void CUtlBuffer::SetBigEndian(bool bigEndian)
        {
            m_Byteswap.SetTargetBigEndian(bigEndian);
        }

        bool CUtlBuffer::IsBigEndian(void)
        {
            return m_Byteswap.IsTargetBigEndian();
        }


        //-----------------------------------------------------------------------------
        // null terminate the buffer
        //-----------------------------------------------------------------------------
        void CUtlBuffer::AddNullTermination(void)
        {
            if(m_Put > m_nMaxPut) {
                if(!IsReadOnly() && ((m_Error & PUT_OVERFLOW) == 0)) {
                    // Add null termination value
                    if(CheckPut(1)) {
                        m_Memory[m_Put - m_nOffset] = 0;
                    } else {
                        // Restore the overflow state, it was valid before...
                        m_Error &= ~PUT_OVERFLOW;
                    }
                }
                m_nMaxPut = m_Put;
            }
        }


        //-----------------------------------------------------------------------------
        // Converts a buffer from a CRLF buffer to a CR buffer (and back)
        // Returns false if no conversion was necessary (and outBuf is left untouched)
        // If the conversion occurs, outBuf will be cleared.
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::ConvertCRLF(CUtlBuffer &outBuf)
        {
            if(!IsText() || !outBuf.IsText())
                return false;

            if(ContainsCRLF() == outBuf.ContainsCRLF())
                return false;

            int nInCount = TellMaxPut();

            outBuf.Purge();
            outBuf.EnsureCapacity(nInCount);

            bool bFromCRLF = ContainsCRLF();

            // Start reading from the beginning
            int nGet = TellGet();
            int nPut = TellPut();
            int nGetDelta = 0;
            int nPutDelta = 0;

            const char *pBase = (const char*)Base();
            int nCurrGet = 0;
            while(nCurrGet < nInCount) {
                const char *pCurr = &pBase[nCurrGet];
                if(bFromCRLF) {
                    const char *pNext = V_strnistr(pCurr, "\r\n", nInCount - nCurrGet);
                    if(!pNext) {
                        outBuf.Put(pCurr, nInCount - nCurrGet);
                        break;
                    }

                    int nBytes = (size_t)pNext - (size_t)pCurr;
                    outBuf.Put(pCurr, nBytes);
                    outBuf.PutChar('\n');
                    nCurrGet += nBytes + 2;
                    if(nGet >= nCurrGet - 1) {
                        --nGetDelta;
                    }
                    if(nPut >= nCurrGet - 1) {
                        --nPutDelta;
                    }
                } else {
                    const char *pNext = V_strnchr(pCurr, '\n', nInCount - nCurrGet);
                    if(!pNext) {
                        outBuf.Put(pCurr, nInCount - nCurrGet);
                        break;
                    }

                    int nBytes = (size_t)pNext - (size_t)pCurr;
                    outBuf.Put(pCurr, nBytes);
                    outBuf.PutChar('\r');
                    outBuf.PutChar('\n');
                    nCurrGet += nBytes + 1;
                    if(nGet >= nCurrGet) {
                        ++nGetDelta;
                    }
                    if(nPut >= nCurrGet) {
                        ++nPutDelta;
                    }
                }
            }

            assert(nPut + nPutDelta <= outBuf.TellMaxPut());

            outBuf.SeekGet(SEEK_HEAD, nGet + nGetDelta);
            outBuf.SeekPut(SEEK_HEAD, nPut + nPutDelta);

            return true;
        }


        //---------------------------------------------------------------------------
        // Implementation of CUtlInplaceBuffer
        //---------------------------------------------------------------------------

        CUtlInplaceBuffer::CUtlInplaceBuffer(int growSize /* = 0 */, int initSize /* = 0 */, int nFlags /* = 0 */) :
            CUtlBuffer(growSize, initSize, nFlags)
        {
            NULL;
        }

        bool CUtlInplaceBuffer::InplaceGetLinePtr(char **ppszInBufferPtr, int *pnLineLength)
        {
            assert(IsText() && !ContainsCRLF());

            int nLineLen = PeekLineLength();
            if(nLineLen <= 1) {
                SeekGet(SEEK_TAIL, 0);
                return false;
            }

            --nLineLen; // because it accounts for putting a terminating null-character

            char *pszLine = (char *) const_cast< void * >(PeekGet());
            SeekGet(SEEK_CURRENT, nLineLen);

            // Set the out args
            if(ppszInBufferPtr)
                *ppszInBufferPtr = pszLine;

            if(pnLineLength)
                *pnLineLength = nLineLen;

            return true;
        }

        char * CUtlInplaceBuffer::InplaceGetLinePtr(void)
        {
            char *pszLine = NULL;
            int nLineLen = 0;

            if(InplaceGetLinePtr(&pszLine, &nLineLen)) {
                assert(nLineLen >= 1);

                switch(pszLine[nLineLen - 1]) {
                    case '\n':
                    case '\r':
                        pszLine[nLineLen - 1] = 0;
                        if(--nLineLen) {
                            switch(pszLine[nLineLen - 1]) {
                                case '\n':
                                case '\r':
                                    pszLine[nLineLen - 1] = 0;
                                    break;
                            }
                        }
                        break;

                    default:
                        assert(pszLine[nLineLen] == 0);
                        break;
                }
            }
            return pszLine;
        }








































































































// Junk Code By Troll Face & Thaisen's Gen
void bttWgGBhiBIWGFEsnPYxTCaOgkZwbTAmUPPYJtfy31921352() {     int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU34865405 = -337659724;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU37461344 = -267340500;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU16675722 = -449896793;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU18806866 = -143459688;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU2130854 = -162678621;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU5948618 = -600219482;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU18412414 = -981062797;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU78708733 = 71798793;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU7481691 = -118569231;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU83321103 = -986489845;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU24842523 = -812622421;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU94999689 = -522167622;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU50270050 = -845019663;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU63406128 = -340527184;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU12140590 = 20170031;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU10395199 = -802448577;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU38104406 = -634358407;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU80509842 = -813464409;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU96938073 = -279253423;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU19703544 = -268144426;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU44940109 = 54678490;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU8721395 = -437348795;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU58248755 = -99812101;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU62735850 = -468703818;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU92646035 = -981944727;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU74277784 = -442022767;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15389833 = -300038853;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU40162132 = -159525137;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU45730672 = 55822598;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU8045581 = -717431075;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU41521066 = -345015500;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU38790247 = -901725045;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU28775162 = -393923676;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU77708014 = -966802826;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU70625243 = -934043966;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU94855895 = -246445766;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU56804934 = -751507992;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU316202 = -261428811;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15978053 = -561098289;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU45756493 = 73270112;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU86132870 = -190992369;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU59187063 = 20097747;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU99611708 = -615612170;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU22475472 = -15515664;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU22885101 = -698084029;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26437280 = -205756014;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU39167445 = -23464655;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU42790267 = -843763723;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU95151995 = 49204785;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU29710391 = -590376641;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU74131887 = -178933112;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU66253353 = -898236582;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU48617834 = -618814992;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU13700744 = -844425792;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU86981242 = -549517415;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26144011 = -900310929;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU79212589 = -67528400;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU53939872 = -981192975;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26160830 = -161514961;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU27853070 = -720655854;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU90558785 = -200180630;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU78250281 = -721537661;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU32978062 = -984023806;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU99436109 = -401138157;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU41800037 = -541474345;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU86052275 = -910897377;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU66224528 = -28243946;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU72562035 = -878216837;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU92780884 = -406483218;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU17284695 = -733384203;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU53590265 = 49059415;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU37788204 = -272929596;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU64531789 = -152366121;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU51181580 = -252523536;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU33570674 = 22847942;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU85753046 = -965419257;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU9109686 = -821736626;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU35773283 = 15703562;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU39850750 = -770619789;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU66208755 = -676188714;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU35110340 = -318558112;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU72599565 = -456275130;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU45010137 = -108729922;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU16020281 = -353800762;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU33913694 = -438497963;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU75267712 = -446778919;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU90172412 = -182910054;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15074419 = -549497885;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU90726772 = -317285412;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU44481232 = 66266962;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15643306 = -78917367;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU2865063 = -770315017;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU74155372 = 86150;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU88124983 = -840442436;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU55197707 = -726549259;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU7882590 = -469454708;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26209001 = 4121552;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU175599 = -114474014;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU80675435 = -474041319;    int swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU36832825 = -337659724;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU34865405 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU37461344;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU37461344 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU16675722;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU16675722 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU18806866;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU18806866 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU2130854;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU2130854 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU5948618;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU5948618 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU18412414;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU18412414 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU78708733;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU78708733 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU7481691;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU7481691 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU83321103;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU83321103 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU24842523;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU24842523 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU94999689;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU94999689 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU50270050;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU50270050 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU63406128;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU63406128 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU12140590;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU12140590 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU10395199;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU10395199 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU38104406;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU38104406 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU80509842;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU80509842 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU96938073;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU96938073 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU19703544;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU19703544 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU44940109;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU44940109 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU8721395;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU8721395 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU58248755;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU58248755 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU62735850;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU62735850 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU92646035;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU92646035 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU74277784;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU74277784 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15389833;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15389833 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU40162132;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU40162132 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU45730672;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU45730672 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU8045581;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU8045581 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU41521066;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU41521066 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU38790247;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU38790247 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU28775162;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU28775162 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU77708014;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU77708014 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU70625243;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU70625243 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU94855895;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU94855895 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU56804934;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU56804934 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU316202;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU316202 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15978053;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15978053 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU45756493;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU45756493 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU86132870;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU86132870 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU59187063;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU59187063 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU99611708;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU99611708 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU22475472;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU22475472 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU22885101;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU22885101 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26437280;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26437280 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU39167445;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU39167445 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU42790267;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU42790267 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU95151995;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU95151995 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU29710391;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU29710391 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU74131887;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU74131887 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU66253353;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU66253353 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU48617834;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU48617834 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU13700744;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU13700744 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU86981242;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU86981242 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26144011;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26144011 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU79212589;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU79212589 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU53939872;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU53939872 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26160830;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26160830 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU27853070;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU27853070 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU90558785;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU90558785 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU78250281;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU78250281 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU32978062;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU32978062 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU99436109;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU99436109 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU41800037;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU41800037 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU86052275;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU86052275 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU66224528;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU66224528 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU72562035;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU72562035 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU92780884;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU92780884 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU17284695;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU17284695 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU53590265;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU53590265 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU37788204;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU37788204 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU64531789;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU64531789 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU51181580;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU51181580 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU33570674;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU33570674 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU85753046;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU85753046 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU9109686;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU9109686 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU35773283;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU35773283 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU39850750;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU39850750 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU66208755;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU66208755 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU35110340;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU35110340 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU72599565;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU72599565 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU45010137;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU45010137 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU16020281;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU16020281 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU33913694;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU33913694 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU75267712;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU75267712 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU90172412;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU90172412 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15074419;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15074419 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU90726772;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU90726772 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU44481232;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU44481232 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15643306;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU15643306 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU2865063;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU2865063 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU74155372;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU74155372 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU88124983;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU88124983 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU55197707;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU55197707 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU7882590;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU7882590 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26209001;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU26209001 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU175599;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU175599 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU80675435;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU80675435 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU36832825;     swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU36832825 = swFCqyHgOiGAOurZTeqvhEFOBVCqqfitjZEXwXYidsisNzXaXqEeDkRnqrBUwQqizMQopU34865405;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void OrucnFnZRUwFnLtqrmfrcEsIWTZMiPuLRjjbqBnJEcPuicxAzBs8396751() {     float YOvAJylGUfaqBZloEbRApyVmzoyZ36352697 = -961033321;    float YOvAJylGUfaqBZloEbRApyVmzoyZ27302689 = -882930430;    float YOvAJylGUfaqBZloEbRApyVmzoyZ24942624 = -978217082;    float YOvAJylGUfaqBZloEbRApyVmzoyZ61327131 = -840857057;    float YOvAJylGUfaqBZloEbRApyVmzoyZ81279090 = -848013971;    float YOvAJylGUfaqBZloEbRApyVmzoyZ22493955 = -595978478;    float YOvAJylGUfaqBZloEbRApyVmzoyZ63540396 = -76680318;    float YOvAJylGUfaqBZloEbRApyVmzoyZ24806437 = 88517653;    float YOvAJylGUfaqBZloEbRApyVmzoyZ40579788 = -869510548;    float YOvAJylGUfaqBZloEbRApyVmzoyZ10873608 = -502758406;    float YOvAJylGUfaqBZloEbRApyVmzoyZ52558957 = -802505993;    float YOvAJylGUfaqBZloEbRApyVmzoyZ90980391 = -711136081;    float YOvAJylGUfaqBZloEbRApyVmzoyZ37417218 = -352646303;    float YOvAJylGUfaqBZloEbRApyVmzoyZ70016788 = -517329471;    float YOvAJylGUfaqBZloEbRApyVmzoyZ20172029 = 54227039;    float YOvAJylGUfaqBZloEbRApyVmzoyZ21416138 = -221058633;    float YOvAJylGUfaqBZloEbRApyVmzoyZ58588191 = 79930499;    float YOvAJylGUfaqBZloEbRApyVmzoyZ72797322 = -830612815;    float YOvAJylGUfaqBZloEbRApyVmzoyZ8258617 = -86181417;    float YOvAJylGUfaqBZloEbRApyVmzoyZ90313212 = -35842283;    float YOvAJylGUfaqBZloEbRApyVmzoyZ13053362 = -13474099;    float YOvAJylGUfaqBZloEbRApyVmzoyZ93910226 = -875249673;    float YOvAJylGUfaqBZloEbRApyVmzoyZ69449464 = -905058509;    float YOvAJylGUfaqBZloEbRApyVmzoyZ79408881 = -104179272;    float YOvAJylGUfaqBZloEbRApyVmzoyZ48286912 = -112629921;    float YOvAJylGUfaqBZloEbRApyVmzoyZ10717697 = -82967818;    float YOvAJylGUfaqBZloEbRApyVmzoyZ54083825 = -167917432;    float YOvAJylGUfaqBZloEbRApyVmzoyZ75663421 = -151131724;    float YOvAJylGUfaqBZloEbRApyVmzoyZ1424496 = 30543683;    float YOvAJylGUfaqBZloEbRApyVmzoyZ2415262 = -162378187;    float YOvAJylGUfaqBZloEbRApyVmzoyZ23431973 = -883314251;    float YOvAJylGUfaqBZloEbRApyVmzoyZ64299036 = 25543644;    float YOvAJylGUfaqBZloEbRApyVmzoyZ15396984 = -107043022;    float YOvAJylGUfaqBZloEbRApyVmzoyZ12064782 = -95603611;    float YOvAJylGUfaqBZloEbRApyVmzoyZ15510506 = -278194807;    float YOvAJylGUfaqBZloEbRApyVmzoyZ48677623 = -295121281;    float YOvAJylGUfaqBZloEbRApyVmzoyZ41791660 = 51787134;    float YOvAJylGUfaqBZloEbRApyVmzoyZ47992270 = -172426719;    float YOvAJylGUfaqBZloEbRApyVmzoyZ33094437 = 35828133;    float YOvAJylGUfaqBZloEbRApyVmzoyZ41491478 = -798030308;    float YOvAJylGUfaqBZloEbRApyVmzoyZ51021375 = -825619414;    float YOvAJylGUfaqBZloEbRApyVmzoyZ62913753 = 95815569;    float YOvAJylGUfaqBZloEbRApyVmzoyZ69421756 = -423822297;    float YOvAJylGUfaqBZloEbRApyVmzoyZ37856596 = -213306283;    float YOvAJylGUfaqBZloEbRApyVmzoyZ92725473 = -393745628;    float YOvAJylGUfaqBZloEbRApyVmzoyZ79125833 = -289795537;    float YOvAJylGUfaqBZloEbRApyVmzoyZ35995125 = -588516620;    float YOvAJylGUfaqBZloEbRApyVmzoyZ56486548 = 1802037;    float YOvAJylGUfaqBZloEbRApyVmzoyZ85961946 = -266363569;    float YOvAJylGUfaqBZloEbRApyVmzoyZ93441390 = -975046529;    float YOvAJylGUfaqBZloEbRApyVmzoyZ47325151 = -285342101;    float YOvAJylGUfaqBZloEbRApyVmzoyZ23641039 = -734537246;    float YOvAJylGUfaqBZloEbRApyVmzoyZ65723289 = -469989059;    float YOvAJylGUfaqBZloEbRApyVmzoyZ51154515 = -309593145;    float YOvAJylGUfaqBZloEbRApyVmzoyZ95343389 = -837404813;    float YOvAJylGUfaqBZloEbRApyVmzoyZ42442471 = 14216352;    float YOvAJylGUfaqBZloEbRApyVmzoyZ57853225 = -977871922;    float YOvAJylGUfaqBZloEbRApyVmzoyZ45533743 = -774037811;    float YOvAJylGUfaqBZloEbRApyVmzoyZ13040219 = -628227137;    float YOvAJylGUfaqBZloEbRApyVmzoyZ70561393 = -665046154;    float YOvAJylGUfaqBZloEbRApyVmzoyZ68410129 = -328061046;    float YOvAJylGUfaqBZloEbRApyVmzoyZ87876975 = -925548594;    float YOvAJylGUfaqBZloEbRApyVmzoyZ23381941 = -942026030;    float YOvAJylGUfaqBZloEbRApyVmzoyZ38164527 = -607132361;    float YOvAJylGUfaqBZloEbRApyVmzoyZ87441635 = -619444155;    float YOvAJylGUfaqBZloEbRApyVmzoyZ88259920 = -728049637;    float YOvAJylGUfaqBZloEbRApyVmzoyZ75583407 = -504093059;    float YOvAJylGUfaqBZloEbRApyVmzoyZ25352437 = -157042692;    float YOvAJylGUfaqBZloEbRApyVmzoyZ54506283 = -139134665;    float YOvAJylGUfaqBZloEbRApyVmzoyZ71494405 = -650651680;    float YOvAJylGUfaqBZloEbRApyVmzoyZ79624478 = -172845768;    float YOvAJylGUfaqBZloEbRApyVmzoyZ10595922 = -747642782;    float YOvAJylGUfaqBZloEbRApyVmzoyZ39702886 = -766440949;    float YOvAJylGUfaqBZloEbRApyVmzoyZ66767138 = -288151109;    float YOvAJylGUfaqBZloEbRApyVmzoyZ39291838 = -210222869;    float YOvAJylGUfaqBZloEbRApyVmzoyZ50139608 = -9289668;    float YOvAJylGUfaqBZloEbRApyVmzoyZ24488470 = -351427377;    float YOvAJylGUfaqBZloEbRApyVmzoyZ31592868 = -591752226;    float YOvAJylGUfaqBZloEbRApyVmzoyZ86683407 = -710433644;    float YOvAJylGUfaqBZloEbRApyVmzoyZ69161079 = -822834385;    float YOvAJylGUfaqBZloEbRApyVmzoyZ74722572 = -494451198;    float YOvAJylGUfaqBZloEbRApyVmzoyZ97597277 = -69719470;    float YOvAJylGUfaqBZloEbRApyVmzoyZ89701475 = -884768156;    float YOvAJylGUfaqBZloEbRApyVmzoyZ7983106 = 5590211;    float YOvAJylGUfaqBZloEbRApyVmzoyZ55090110 = -877036086;    float YOvAJylGUfaqBZloEbRApyVmzoyZ99790933 = -48777006;    float YOvAJylGUfaqBZloEbRApyVmzoyZ98575747 = -504467297;    float YOvAJylGUfaqBZloEbRApyVmzoyZ64242469 = -797449877;    float YOvAJylGUfaqBZloEbRApyVmzoyZ16721392 = -258198798;    float YOvAJylGUfaqBZloEbRApyVmzoyZ73068034 = -192411159;    float YOvAJylGUfaqBZloEbRApyVmzoyZ90824398 = -317249360;    float YOvAJylGUfaqBZloEbRApyVmzoyZ96257916 = -174175055;    float YOvAJylGUfaqBZloEbRApyVmzoyZ34952052 = -544199583;    float YOvAJylGUfaqBZloEbRApyVmzoyZ62533043 = -299125713;    float YOvAJylGUfaqBZloEbRApyVmzoyZ73081349 = -369969263;    float YOvAJylGUfaqBZloEbRApyVmzoyZ63144400 = -900070820;    float YOvAJylGUfaqBZloEbRApyVmzoyZ39531813 = 37841598;    float YOvAJylGUfaqBZloEbRApyVmzoyZ31257230 = -816689936;    float YOvAJylGUfaqBZloEbRApyVmzoyZ50414961 = -593862128;    float YOvAJylGUfaqBZloEbRApyVmzoyZ4465554 = -961033321;     YOvAJylGUfaqBZloEbRApyVmzoyZ36352697 = YOvAJylGUfaqBZloEbRApyVmzoyZ27302689;     YOvAJylGUfaqBZloEbRApyVmzoyZ27302689 = YOvAJylGUfaqBZloEbRApyVmzoyZ24942624;     YOvAJylGUfaqBZloEbRApyVmzoyZ24942624 = YOvAJylGUfaqBZloEbRApyVmzoyZ61327131;     YOvAJylGUfaqBZloEbRApyVmzoyZ61327131 = YOvAJylGUfaqBZloEbRApyVmzoyZ81279090;     YOvAJylGUfaqBZloEbRApyVmzoyZ81279090 = YOvAJylGUfaqBZloEbRApyVmzoyZ22493955;     YOvAJylGUfaqBZloEbRApyVmzoyZ22493955 = YOvAJylGUfaqBZloEbRApyVmzoyZ63540396;     YOvAJylGUfaqBZloEbRApyVmzoyZ63540396 = YOvAJylGUfaqBZloEbRApyVmzoyZ24806437;     YOvAJylGUfaqBZloEbRApyVmzoyZ24806437 = YOvAJylGUfaqBZloEbRApyVmzoyZ40579788;     YOvAJylGUfaqBZloEbRApyVmzoyZ40579788 = YOvAJylGUfaqBZloEbRApyVmzoyZ10873608;     YOvAJylGUfaqBZloEbRApyVmzoyZ10873608 = YOvAJylGUfaqBZloEbRApyVmzoyZ52558957;     YOvAJylGUfaqBZloEbRApyVmzoyZ52558957 = YOvAJylGUfaqBZloEbRApyVmzoyZ90980391;     YOvAJylGUfaqBZloEbRApyVmzoyZ90980391 = YOvAJylGUfaqBZloEbRApyVmzoyZ37417218;     YOvAJylGUfaqBZloEbRApyVmzoyZ37417218 = YOvAJylGUfaqBZloEbRApyVmzoyZ70016788;     YOvAJylGUfaqBZloEbRApyVmzoyZ70016788 = YOvAJylGUfaqBZloEbRApyVmzoyZ20172029;     YOvAJylGUfaqBZloEbRApyVmzoyZ20172029 = YOvAJylGUfaqBZloEbRApyVmzoyZ21416138;     YOvAJylGUfaqBZloEbRApyVmzoyZ21416138 = YOvAJylGUfaqBZloEbRApyVmzoyZ58588191;     YOvAJylGUfaqBZloEbRApyVmzoyZ58588191 = YOvAJylGUfaqBZloEbRApyVmzoyZ72797322;     YOvAJylGUfaqBZloEbRApyVmzoyZ72797322 = YOvAJylGUfaqBZloEbRApyVmzoyZ8258617;     YOvAJylGUfaqBZloEbRApyVmzoyZ8258617 = YOvAJylGUfaqBZloEbRApyVmzoyZ90313212;     YOvAJylGUfaqBZloEbRApyVmzoyZ90313212 = YOvAJylGUfaqBZloEbRApyVmzoyZ13053362;     YOvAJylGUfaqBZloEbRApyVmzoyZ13053362 = YOvAJylGUfaqBZloEbRApyVmzoyZ93910226;     YOvAJylGUfaqBZloEbRApyVmzoyZ93910226 = YOvAJylGUfaqBZloEbRApyVmzoyZ69449464;     YOvAJylGUfaqBZloEbRApyVmzoyZ69449464 = YOvAJylGUfaqBZloEbRApyVmzoyZ79408881;     YOvAJylGUfaqBZloEbRApyVmzoyZ79408881 = YOvAJylGUfaqBZloEbRApyVmzoyZ48286912;     YOvAJylGUfaqBZloEbRApyVmzoyZ48286912 = YOvAJylGUfaqBZloEbRApyVmzoyZ10717697;     YOvAJylGUfaqBZloEbRApyVmzoyZ10717697 = YOvAJylGUfaqBZloEbRApyVmzoyZ54083825;     YOvAJylGUfaqBZloEbRApyVmzoyZ54083825 = YOvAJylGUfaqBZloEbRApyVmzoyZ75663421;     YOvAJylGUfaqBZloEbRApyVmzoyZ75663421 = YOvAJylGUfaqBZloEbRApyVmzoyZ1424496;     YOvAJylGUfaqBZloEbRApyVmzoyZ1424496 = YOvAJylGUfaqBZloEbRApyVmzoyZ2415262;     YOvAJylGUfaqBZloEbRApyVmzoyZ2415262 = YOvAJylGUfaqBZloEbRApyVmzoyZ23431973;     YOvAJylGUfaqBZloEbRApyVmzoyZ23431973 = YOvAJylGUfaqBZloEbRApyVmzoyZ64299036;     YOvAJylGUfaqBZloEbRApyVmzoyZ64299036 = YOvAJylGUfaqBZloEbRApyVmzoyZ15396984;     YOvAJylGUfaqBZloEbRApyVmzoyZ15396984 = YOvAJylGUfaqBZloEbRApyVmzoyZ12064782;     YOvAJylGUfaqBZloEbRApyVmzoyZ12064782 = YOvAJylGUfaqBZloEbRApyVmzoyZ15510506;     YOvAJylGUfaqBZloEbRApyVmzoyZ15510506 = YOvAJylGUfaqBZloEbRApyVmzoyZ48677623;     YOvAJylGUfaqBZloEbRApyVmzoyZ48677623 = YOvAJylGUfaqBZloEbRApyVmzoyZ41791660;     YOvAJylGUfaqBZloEbRApyVmzoyZ41791660 = YOvAJylGUfaqBZloEbRApyVmzoyZ47992270;     YOvAJylGUfaqBZloEbRApyVmzoyZ47992270 = YOvAJylGUfaqBZloEbRApyVmzoyZ33094437;     YOvAJylGUfaqBZloEbRApyVmzoyZ33094437 = YOvAJylGUfaqBZloEbRApyVmzoyZ41491478;     YOvAJylGUfaqBZloEbRApyVmzoyZ41491478 = YOvAJylGUfaqBZloEbRApyVmzoyZ51021375;     YOvAJylGUfaqBZloEbRApyVmzoyZ51021375 = YOvAJylGUfaqBZloEbRApyVmzoyZ62913753;     YOvAJylGUfaqBZloEbRApyVmzoyZ62913753 = YOvAJylGUfaqBZloEbRApyVmzoyZ69421756;     YOvAJylGUfaqBZloEbRApyVmzoyZ69421756 = YOvAJylGUfaqBZloEbRApyVmzoyZ37856596;     YOvAJylGUfaqBZloEbRApyVmzoyZ37856596 = YOvAJylGUfaqBZloEbRApyVmzoyZ92725473;     YOvAJylGUfaqBZloEbRApyVmzoyZ92725473 = YOvAJylGUfaqBZloEbRApyVmzoyZ79125833;     YOvAJylGUfaqBZloEbRApyVmzoyZ79125833 = YOvAJylGUfaqBZloEbRApyVmzoyZ35995125;     YOvAJylGUfaqBZloEbRApyVmzoyZ35995125 = YOvAJylGUfaqBZloEbRApyVmzoyZ56486548;     YOvAJylGUfaqBZloEbRApyVmzoyZ56486548 = YOvAJylGUfaqBZloEbRApyVmzoyZ85961946;     YOvAJylGUfaqBZloEbRApyVmzoyZ85961946 = YOvAJylGUfaqBZloEbRApyVmzoyZ93441390;     YOvAJylGUfaqBZloEbRApyVmzoyZ93441390 = YOvAJylGUfaqBZloEbRApyVmzoyZ47325151;     YOvAJylGUfaqBZloEbRApyVmzoyZ47325151 = YOvAJylGUfaqBZloEbRApyVmzoyZ23641039;     YOvAJylGUfaqBZloEbRApyVmzoyZ23641039 = YOvAJylGUfaqBZloEbRApyVmzoyZ65723289;     YOvAJylGUfaqBZloEbRApyVmzoyZ65723289 = YOvAJylGUfaqBZloEbRApyVmzoyZ51154515;     YOvAJylGUfaqBZloEbRApyVmzoyZ51154515 = YOvAJylGUfaqBZloEbRApyVmzoyZ95343389;     YOvAJylGUfaqBZloEbRApyVmzoyZ95343389 = YOvAJylGUfaqBZloEbRApyVmzoyZ42442471;     YOvAJylGUfaqBZloEbRApyVmzoyZ42442471 = YOvAJylGUfaqBZloEbRApyVmzoyZ57853225;     YOvAJylGUfaqBZloEbRApyVmzoyZ57853225 = YOvAJylGUfaqBZloEbRApyVmzoyZ45533743;     YOvAJylGUfaqBZloEbRApyVmzoyZ45533743 = YOvAJylGUfaqBZloEbRApyVmzoyZ13040219;     YOvAJylGUfaqBZloEbRApyVmzoyZ13040219 = YOvAJylGUfaqBZloEbRApyVmzoyZ70561393;     YOvAJylGUfaqBZloEbRApyVmzoyZ70561393 = YOvAJylGUfaqBZloEbRApyVmzoyZ68410129;     YOvAJylGUfaqBZloEbRApyVmzoyZ68410129 = YOvAJylGUfaqBZloEbRApyVmzoyZ87876975;     YOvAJylGUfaqBZloEbRApyVmzoyZ87876975 = YOvAJylGUfaqBZloEbRApyVmzoyZ23381941;     YOvAJylGUfaqBZloEbRApyVmzoyZ23381941 = YOvAJylGUfaqBZloEbRApyVmzoyZ38164527;     YOvAJylGUfaqBZloEbRApyVmzoyZ38164527 = YOvAJylGUfaqBZloEbRApyVmzoyZ87441635;     YOvAJylGUfaqBZloEbRApyVmzoyZ87441635 = YOvAJylGUfaqBZloEbRApyVmzoyZ88259920;     YOvAJylGUfaqBZloEbRApyVmzoyZ88259920 = YOvAJylGUfaqBZloEbRApyVmzoyZ75583407;     YOvAJylGUfaqBZloEbRApyVmzoyZ75583407 = YOvAJylGUfaqBZloEbRApyVmzoyZ25352437;     YOvAJylGUfaqBZloEbRApyVmzoyZ25352437 = YOvAJylGUfaqBZloEbRApyVmzoyZ54506283;     YOvAJylGUfaqBZloEbRApyVmzoyZ54506283 = YOvAJylGUfaqBZloEbRApyVmzoyZ71494405;     YOvAJylGUfaqBZloEbRApyVmzoyZ71494405 = YOvAJylGUfaqBZloEbRApyVmzoyZ79624478;     YOvAJylGUfaqBZloEbRApyVmzoyZ79624478 = YOvAJylGUfaqBZloEbRApyVmzoyZ10595922;     YOvAJylGUfaqBZloEbRApyVmzoyZ10595922 = YOvAJylGUfaqBZloEbRApyVmzoyZ39702886;     YOvAJylGUfaqBZloEbRApyVmzoyZ39702886 = YOvAJylGUfaqBZloEbRApyVmzoyZ66767138;     YOvAJylGUfaqBZloEbRApyVmzoyZ66767138 = YOvAJylGUfaqBZloEbRApyVmzoyZ39291838;     YOvAJylGUfaqBZloEbRApyVmzoyZ39291838 = YOvAJylGUfaqBZloEbRApyVmzoyZ50139608;     YOvAJylGUfaqBZloEbRApyVmzoyZ50139608 = YOvAJylGUfaqBZloEbRApyVmzoyZ24488470;     YOvAJylGUfaqBZloEbRApyVmzoyZ24488470 = YOvAJylGUfaqBZloEbRApyVmzoyZ31592868;     YOvAJylGUfaqBZloEbRApyVmzoyZ31592868 = YOvAJylGUfaqBZloEbRApyVmzoyZ86683407;     YOvAJylGUfaqBZloEbRApyVmzoyZ86683407 = YOvAJylGUfaqBZloEbRApyVmzoyZ69161079;     YOvAJylGUfaqBZloEbRApyVmzoyZ69161079 = YOvAJylGUfaqBZloEbRApyVmzoyZ74722572;     YOvAJylGUfaqBZloEbRApyVmzoyZ74722572 = YOvAJylGUfaqBZloEbRApyVmzoyZ97597277;     YOvAJylGUfaqBZloEbRApyVmzoyZ97597277 = YOvAJylGUfaqBZloEbRApyVmzoyZ89701475;     YOvAJylGUfaqBZloEbRApyVmzoyZ89701475 = YOvAJylGUfaqBZloEbRApyVmzoyZ7983106;     YOvAJylGUfaqBZloEbRApyVmzoyZ7983106 = YOvAJylGUfaqBZloEbRApyVmzoyZ55090110;     YOvAJylGUfaqBZloEbRApyVmzoyZ55090110 = YOvAJylGUfaqBZloEbRApyVmzoyZ99790933;     YOvAJylGUfaqBZloEbRApyVmzoyZ99790933 = YOvAJylGUfaqBZloEbRApyVmzoyZ98575747;     YOvAJylGUfaqBZloEbRApyVmzoyZ98575747 = YOvAJylGUfaqBZloEbRApyVmzoyZ64242469;     YOvAJylGUfaqBZloEbRApyVmzoyZ64242469 = YOvAJylGUfaqBZloEbRApyVmzoyZ16721392;     YOvAJylGUfaqBZloEbRApyVmzoyZ16721392 = YOvAJylGUfaqBZloEbRApyVmzoyZ73068034;     YOvAJylGUfaqBZloEbRApyVmzoyZ73068034 = YOvAJylGUfaqBZloEbRApyVmzoyZ90824398;     YOvAJylGUfaqBZloEbRApyVmzoyZ90824398 = YOvAJylGUfaqBZloEbRApyVmzoyZ96257916;     YOvAJylGUfaqBZloEbRApyVmzoyZ96257916 = YOvAJylGUfaqBZloEbRApyVmzoyZ34952052;     YOvAJylGUfaqBZloEbRApyVmzoyZ34952052 = YOvAJylGUfaqBZloEbRApyVmzoyZ62533043;     YOvAJylGUfaqBZloEbRApyVmzoyZ62533043 = YOvAJylGUfaqBZloEbRApyVmzoyZ73081349;     YOvAJylGUfaqBZloEbRApyVmzoyZ73081349 = YOvAJylGUfaqBZloEbRApyVmzoyZ63144400;     YOvAJylGUfaqBZloEbRApyVmzoyZ63144400 = YOvAJylGUfaqBZloEbRApyVmzoyZ39531813;     YOvAJylGUfaqBZloEbRApyVmzoyZ39531813 = YOvAJylGUfaqBZloEbRApyVmzoyZ31257230;     YOvAJylGUfaqBZloEbRApyVmzoyZ31257230 = YOvAJylGUfaqBZloEbRApyVmzoyZ50414961;     YOvAJylGUfaqBZloEbRApyVmzoyZ50414961 = YOvAJylGUfaqBZloEbRApyVmzoyZ4465554;     YOvAJylGUfaqBZloEbRApyVmzoyZ4465554 = YOvAJylGUfaqBZloEbRApyVmzoyZ36352697;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void iKEXEhSnAxYQlMsOmAFHwgrnr10838906() {     long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE19534284 = -136997084;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE30129853 = -416841823;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE58772401 = -31242597;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE57993933 = -687001988;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE9714948 = 63634367;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE90972560 = -635505726;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE85162358 = -349204607;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE72191718 = -742540925;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE17074880 = -701766162;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE94613546 = 46427443;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE80119991 = -379595128;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE71966388 = -907567601;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE77170425 = -916485490;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE93636117 = 17664067;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE86679918 = -444115141;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE49869197 = -739854274;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE42522046 = -935163714;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE87004526 = -150900751;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE43409060 = -204330198;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE42726716 = -349912998;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE70315853 = -586896892;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE89993521 = -903303051;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE66102760 = -699611811;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE86350963 = -221959346;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE13083941 = 7625393;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE47086018 = -702276144;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE36402483 = -978270990;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE32425042 = -649538234;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE96975846 = -985999680;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE88835406 = -27994946;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE61397912 = 52552486;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE5513202 = -70207518;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE89732335 = -138027146;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE95192730 = -802943298;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE76222955 = -186657069;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE29552352 = -350124802;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE47612415 = -528632277;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE42728337 = -797723811;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE58886648 = -985359033;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE98353190 = -749529878;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE53549968 = -835510416;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE24922568 = -849887494;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE16853203 = -490356207;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE10478613 = -345060993;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE21521754 = -6900750;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE60688863 = -371654553;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE34992074 = -256413761;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE5482017 = -941662277;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE85581932 = -497049965;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE98950815 = -778531486;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE32929553 = 44102877;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE87281096 = -202325595;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE68461240 = -820918773;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE16288915 = -800194239;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE40523053 = -897038509;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE29540762 = -233694033;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE64027092 = -717230013;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE72421437 = -809283251;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE44909992 = -594627382;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE62628930 = -234089489;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE54570078 = -657234737;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE52737317 = -699666374;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE75215871 = -756541245;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE28239473 = -573771217;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE33215635 = 93874957;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE74606790 = -209387610;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE82234053 = -669540456;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE81977694 = -13542193;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE17413162 = -795678864;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE57127567 = 6009661;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE2256783 = -111221997;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE99793709 = -37439904;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE28117879 = -165541718;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE45055869 = -454800321;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE89176747 = -514402583;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE45393286 = -737009398;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE73140319 = -312946845;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE55624147 = -254550818;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE64829209 = -115058597;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE52395077 = -620720055;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE12093944 = -345862384;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE30920466 = 63391286;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE46843109 = -52488270;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE98025030 = -107468195;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE55905854 = 27902177;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE74116816 = -745121920;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE37051962 = -249288746;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE73443421 = -337832907;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE54669677 = -905904790;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE46682193 = -952963036;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE65525259 = -632894790;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE75190977 = -719349027;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE97818344 = -103096429;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE96257717 = -651269545;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE43783113 = 7704858;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE812652 = -35844043;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE49706696 = 6653750;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE88613730 = -916584990;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE77262978 = -338935951;    long uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE46914964 = -136997084;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE19534284 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE30129853;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE30129853 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE58772401;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE58772401 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE57993933;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE57993933 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE9714948;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE9714948 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE90972560;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE90972560 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE85162358;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE85162358 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE72191718;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE72191718 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE17074880;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE17074880 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE94613546;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE94613546 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE80119991;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE80119991 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE71966388;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE71966388 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE77170425;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE77170425 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE93636117;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE93636117 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE86679918;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE86679918 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE49869197;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE49869197 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE42522046;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE42522046 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE87004526;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE87004526 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE43409060;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE43409060 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE42726716;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE42726716 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE70315853;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE70315853 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE89993521;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE89993521 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE66102760;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE66102760 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE86350963;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE86350963 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE13083941;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE13083941 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE47086018;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE47086018 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE36402483;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE36402483 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE32425042;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE32425042 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE96975846;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE96975846 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE88835406;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE88835406 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE61397912;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE61397912 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE5513202;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE5513202 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE89732335;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE89732335 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE95192730;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE95192730 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE76222955;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE76222955 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE29552352;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE29552352 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE47612415;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE47612415 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE42728337;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE42728337 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE58886648;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE58886648 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE98353190;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE98353190 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE53549968;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE53549968 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE24922568;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE24922568 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE16853203;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE16853203 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE10478613;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE10478613 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE21521754;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE21521754 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE60688863;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE60688863 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE34992074;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE34992074 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE5482017;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE5482017 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE85581932;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE85581932 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE98950815;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE98950815 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE32929553;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE32929553 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE87281096;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE87281096 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE68461240;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE68461240 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE16288915;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE16288915 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE40523053;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE40523053 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE29540762;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE29540762 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE64027092;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE64027092 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE72421437;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE72421437 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE44909992;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE44909992 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE62628930;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE62628930 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE54570078;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE54570078 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE52737317;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE52737317 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE75215871;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE75215871 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE28239473;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE28239473 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE33215635;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE33215635 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE74606790;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE74606790 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE82234053;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE82234053 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE81977694;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE81977694 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE17413162;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE17413162 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE57127567;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE57127567 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE2256783;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE2256783 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE99793709;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE99793709 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE28117879;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE28117879 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE45055869;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE45055869 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE89176747;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE89176747 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE45393286;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE45393286 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE73140319;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE73140319 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE55624147;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE55624147 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE64829209;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE64829209 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE52395077;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE52395077 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE12093944;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE12093944 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE30920466;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE30920466 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE46843109;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE46843109 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE98025030;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE98025030 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE55905854;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE55905854 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE74116816;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE74116816 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE37051962;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE37051962 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE73443421;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE73443421 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE54669677;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE54669677 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE46682193;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE46682193 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE65525259;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE65525259 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE75190977;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE75190977 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE97818344;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE97818344 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE96257717;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE96257717 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE43783113;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE43783113 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE812652;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE812652 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE49706696;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE49706696 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE88613730;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE88613730 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE77262978;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE77262978 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE46914964;     uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE46914964 = uzJinHrPxdocnupIDsMsPsREKfJohsBSQfJhJEMqhAnjE19534284;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void NfqPzJShdlEkJAJMbluVboCwyaNGbWHGuPDLX2160739() {     double CVsvZlfoYXkRsaVutYYdu26843292 = -534226537;    double CVsvZlfoYXkRsaVutYYdu29049262 = 55750317;    double CVsvZlfoYXkRsaVutYYdu44186907 = 21565121;    double CVsvZlfoYXkRsaVutYYdu55932929 = -765417971;    double CVsvZlfoYXkRsaVutYYdu6418916 = -622158458;    double CVsvZlfoYXkRsaVutYYdu73113563 = -920093970;    double CVsvZlfoYXkRsaVutYYdu46306402 = -213525449;    double CVsvZlfoYXkRsaVutYYdu12091798 = -739842714;    double CVsvZlfoYXkRsaVutYYdu29976055 = -698831974;    double CVsvZlfoYXkRsaVutYYdu41434559 = -786168870;    double CVsvZlfoYXkRsaVutYYdu25275132 = -495427081;    double CVsvZlfoYXkRsaVutYYdu8492954 = -938117144;    double CVsvZlfoYXkRsaVutYYdu97222920 = -316978402;    double CVsvZlfoYXkRsaVutYYdu53254752 = -992866911;    double CVsvZlfoYXkRsaVutYYdu60059261 = -75628106;    double CVsvZlfoYXkRsaVutYYdu53382031 = -277404674;    double CVsvZlfoYXkRsaVutYYdu90360208 = 19078491;    double CVsvZlfoYXkRsaVutYYdu50620669 = -912704156;    double CVsvZlfoYXkRsaVutYYdu39093781 = 27991412;    double CVsvZlfoYXkRsaVutYYdu89330754 = -717867978;    double CVsvZlfoYXkRsaVutYYdu14456736 = -901765381;    double CVsvZlfoYXkRsaVutYYdu18589290 = -952109837;    double CVsvZlfoYXkRsaVutYYdu39740096 = -759264771;    double CVsvZlfoYXkRsaVutYYdu29705502 = -314633837;    double CVsvZlfoYXkRsaVutYYdu38505057 = -494312465;    double CVsvZlfoYXkRsaVutYYdu42032189 = -529934027;    double CVsvZlfoYXkRsaVutYYdu26973009 = -683019973;    double CVsvZlfoYXkRsaVutYYdu66674693 = -313004642;    double CVsvZlfoYXkRsaVutYYdu14660996 = -537146329;    double CVsvZlfoYXkRsaVutYYdu72330413 = -519234837;    double CVsvZlfoYXkRsaVutYYdu55813956 = -785459470;    double CVsvZlfoYXkRsaVutYYdu28755395 = -297937906;    double CVsvZlfoYXkRsaVutYYdu29110350 = -854278488;    double CVsvZlfoYXkRsaVutYYdu31520951 = -212495701;    double CVsvZlfoYXkRsaVutYYdu99601369 = -181199954;    double CVsvZlfoYXkRsaVutYYdu1698417 = -71868219;    double CVsvZlfoYXkRsaVutYYdu62565083 = -36614350;    double CVsvZlfoYXkRsaVutYYdu89485857 = -277711729;    double CVsvZlfoYXkRsaVutYYdu26276636 = -393398564;    double CVsvZlfoYXkRsaVutYYdu21578365 = -373795379;    double CVsvZlfoYXkRsaVutYYdu62628269 = -26779948;    double CVsvZlfoYXkRsaVutYYdu46191927 = -296674808;    double CVsvZlfoYXkRsaVutYYdu56205553 = -475849541;    double CVsvZlfoYXkRsaVutYYdu56056644 = -731844698;    double CVsvZlfoYXkRsaVutYYdu77316969 = -499751881;    double CVsvZlfoYXkRsaVutYYdu21725647 = -925601101;    double CVsvZlfoYXkRsaVutYYdu61679271 = -54101141;    double CVsvZlfoYXkRsaVutYYdu46522939 = -214495260;    double CVsvZlfoYXkRsaVutYYdu79686484 = -143582892;    double CVsvZlfoYXkRsaVutYYdu25166180 = -892779417;    double CVsvZlfoYXkRsaVutYYdu8138536 = -6116323;    double CVsvZlfoYXkRsaVutYYdu95841719 = -550923476;    double CVsvZlfoYXkRsaVutYYdu41166079 = -943832399;    double CVsvZlfoYXkRsaVutYYdu54081396 = -507335566;    double CVsvZlfoYXkRsaVutYYdu87704002 = -209465061;    double CVsvZlfoYXkRsaVutYYdu8254003 = -582116700;    double CVsvZlfoYXkRsaVutYYdu89309166 = -184984913;    double CVsvZlfoYXkRsaVutYYdu14481405 = -663801043;    double CVsvZlfoYXkRsaVutYYdu17427873 = -171105506;    double CVsvZlfoYXkRsaVutYYdu64386727 = 7775569;    double CVsvZlfoYXkRsaVutYYdu46140554 = -137073997;    double CVsvZlfoYXkRsaVutYYdu79631708 = -900520808;    double CVsvZlfoYXkRsaVutYYdu97430802 = -102696386;    double CVsvZlfoYXkRsaVutYYdu57645642 = -79597138;    double CVsvZlfoYXkRsaVutYYdu85620602 = 99290599;    double CVsvZlfoYXkRsaVutYYdu96519736 = -97489175;    double CVsvZlfoYXkRsaVutYYdu79382604 = 16161344;    double CVsvZlfoYXkRsaVutYYdu65701969 = -4482701;    double CVsvZlfoYXkRsaVutYYdu53653382 = -711666957;    double CVsvZlfoYXkRsaVutYYdu58360845 = 96240112;    double CVsvZlfoYXkRsaVutYYdu90816948 = -140790325;    double CVsvZlfoYXkRsaVutYYdu874351 = -703209781;    double CVsvZlfoYXkRsaVutYYdu24344033 = -419305592;    double CVsvZlfoYXkRsaVutYYdu17515417 = -598213209;    double CVsvZlfoYXkRsaVutYYdu26702485 = -591088030;    double CVsvZlfoYXkRsaVutYYdu68264809 = -505090573;    double CVsvZlfoYXkRsaVutYYdu62383737 = -376260297;    double CVsvZlfoYXkRsaVutYYdu83683451 = 72579927;    double CVsvZlfoYXkRsaVutYYdu52388533 = -814881956;    double CVsvZlfoYXkRsaVutYYdu16779410 = -568711364;    double CVsvZlfoYXkRsaVutYYdu80352918 = -375832886;    double CVsvZlfoYXkRsaVutYYdu80450070 = -368524714;    double CVsvZlfoYXkRsaVutYYdu86988209 = -69421750;    double CVsvZlfoYXkRsaVutYYdu89494816 = -644366912;    double CVsvZlfoYXkRsaVutYYdu64191877 = -413118515;    double CVsvZlfoYXkRsaVutYYdu59972237 = -134535995;    double CVsvZlfoYXkRsaVutYYdu87589316 = -354105508;    double CVsvZlfoYXkRsaVutYYdu75028953 = -246942923;    double CVsvZlfoYXkRsaVutYYdu43816949 = 96969360;    double CVsvZlfoYXkRsaVutYYdu91347367 = -599083254;    double CVsvZlfoYXkRsaVutYYdu12389251 = -886883307;    double CVsvZlfoYXkRsaVutYYdu48083678 = -372813308;    double CVsvZlfoYXkRsaVutYYdu72057984 = -6606223;    double CVsvZlfoYXkRsaVutYYdu61889909 = -301174133;    double CVsvZlfoYXkRsaVutYYdu75437810 = -136721383;    double CVsvZlfoYXkRsaVutYYdu82996560 = -126259141;    double CVsvZlfoYXkRsaVutYYdu48761125 = -93978423;    double CVsvZlfoYXkRsaVutYYdu98559910 = -296252404;    double CVsvZlfoYXkRsaVutYYdu70436041 = -731135297;    double CVsvZlfoYXkRsaVutYYdu80797232 = -534226537;     CVsvZlfoYXkRsaVutYYdu26843292 = CVsvZlfoYXkRsaVutYYdu29049262;     CVsvZlfoYXkRsaVutYYdu29049262 = CVsvZlfoYXkRsaVutYYdu44186907;     CVsvZlfoYXkRsaVutYYdu44186907 = CVsvZlfoYXkRsaVutYYdu55932929;     CVsvZlfoYXkRsaVutYYdu55932929 = CVsvZlfoYXkRsaVutYYdu6418916;     CVsvZlfoYXkRsaVutYYdu6418916 = CVsvZlfoYXkRsaVutYYdu73113563;     CVsvZlfoYXkRsaVutYYdu73113563 = CVsvZlfoYXkRsaVutYYdu46306402;     CVsvZlfoYXkRsaVutYYdu46306402 = CVsvZlfoYXkRsaVutYYdu12091798;     CVsvZlfoYXkRsaVutYYdu12091798 = CVsvZlfoYXkRsaVutYYdu29976055;     CVsvZlfoYXkRsaVutYYdu29976055 = CVsvZlfoYXkRsaVutYYdu41434559;     CVsvZlfoYXkRsaVutYYdu41434559 = CVsvZlfoYXkRsaVutYYdu25275132;     CVsvZlfoYXkRsaVutYYdu25275132 = CVsvZlfoYXkRsaVutYYdu8492954;     CVsvZlfoYXkRsaVutYYdu8492954 = CVsvZlfoYXkRsaVutYYdu97222920;     CVsvZlfoYXkRsaVutYYdu97222920 = CVsvZlfoYXkRsaVutYYdu53254752;     CVsvZlfoYXkRsaVutYYdu53254752 = CVsvZlfoYXkRsaVutYYdu60059261;     CVsvZlfoYXkRsaVutYYdu60059261 = CVsvZlfoYXkRsaVutYYdu53382031;     CVsvZlfoYXkRsaVutYYdu53382031 = CVsvZlfoYXkRsaVutYYdu90360208;     CVsvZlfoYXkRsaVutYYdu90360208 = CVsvZlfoYXkRsaVutYYdu50620669;     CVsvZlfoYXkRsaVutYYdu50620669 = CVsvZlfoYXkRsaVutYYdu39093781;     CVsvZlfoYXkRsaVutYYdu39093781 = CVsvZlfoYXkRsaVutYYdu89330754;     CVsvZlfoYXkRsaVutYYdu89330754 = CVsvZlfoYXkRsaVutYYdu14456736;     CVsvZlfoYXkRsaVutYYdu14456736 = CVsvZlfoYXkRsaVutYYdu18589290;     CVsvZlfoYXkRsaVutYYdu18589290 = CVsvZlfoYXkRsaVutYYdu39740096;     CVsvZlfoYXkRsaVutYYdu39740096 = CVsvZlfoYXkRsaVutYYdu29705502;     CVsvZlfoYXkRsaVutYYdu29705502 = CVsvZlfoYXkRsaVutYYdu38505057;     CVsvZlfoYXkRsaVutYYdu38505057 = CVsvZlfoYXkRsaVutYYdu42032189;     CVsvZlfoYXkRsaVutYYdu42032189 = CVsvZlfoYXkRsaVutYYdu26973009;     CVsvZlfoYXkRsaVutYYdu26973009 = CVsvZlfoYXkRsaVutYYdu66674693;     CVsvZlfoYXkRsaVutYYdu66674693 = CVsvZlfoYXkRsaVutYYdu14660996;     CVsvZlfoYXkRsaVutYYdu14660996 = CVsvZlfoYXkRsaVutYYdu72330413;     CVsvZlfoYXkRsaVutYYdu72330413 = CVsvZlfoYXkRsaVutYYdu55813956;     CVsvZlfoYXkRsaVutYYdu55813956 = CVsvZlfoYXkRsaVutYYdu28755395;     CVsvZlfoYXkRsaVutYYdu28755395 = CVsvZlfoYXkRsaVutYYdu29110350;     CVsvZlfoYXkRsaVutYYdu29110350 = CVsvZlfoYXkRsaVutYYdu31520951;     CVsvZlfoYXkRsaVutYYdu31520951 = CVsvZlfoYXkRsaVutYYdu99601369;     CVsvZlfoYXkRsaVutYYdu99601369 = CVsvZlfoYXkRsaVutYYdu1698417;     CVsvZlfoYXkRsaVutYYdu1698417 = CVsvZlfoYXkRsaVutYYdu62565083;     CVsvZlfoYXkRsaVutYYdu62565083 = CVsvZlfoYXkRsaVutYYdu89485857;     CVsvZlfoYXkRsaVutYYdu89485857 = CVsvZlfoYXkRsaVutYYdu26276636;     CVsvZlfoYXkRsaVutYYdu26276636 = CVsvZlfoYXkRsaVutYYdu21578365;     CVsvZlfoYXkRsaVutYYdu21578365 = CVsvZlfoYXkRsaVutYYdu62628269;     CVsvZlfoYXkRsaVutYYdu62628269 = CVsvZlfoYXkRsaVutYYdu46191927;     CVsvZlfoYXkRsaVutYYdu46191927 = CVsvZlfoYXkRsaVutYYdu56205553;     CVsvZlfoYXkRsaVutYYdu56205553 = CVsvZlfoYXkRsaVutYYdu56056644;     CVsvZlfoYXkRsaVutYYdu56056644 = CVsvZlfoYXkRsaVutYYdu77316969;     CVsvZlfoYXkRsaVutYYdu77316969 = CVsvZlfoYXkRsaVutYYdu21725647;     CVsvZlfoYXkRsaVutYYdu21725647 = CVsvZlfoYXkRsaVutYYdu61679271;     CVsvZlfoYXkRsaVutYYdu61679271 = CVsvZlfoYXkRsaVutYYdu46522939;     CVsvZlfoYXkRsaVutYYdu46522939 = CVsvZlfoYXkRsaVutYYdu79686484;     CVsvZlfoYXkRsaVutYYdu79686484 = CVsvZlfoYXkRsaVutYYdu25166180;     CVsvZlfoYXkRsaVutYYdu25166180 = CVsvZlfoYXkRsaVutYYdu8138536;     CVsvZlfoYXkRsaVutYYdu8138536 = CVsvZlfoYXkRsaVutYYdu95841719;     CVsvZlfoYXkRsaVutYYdu95841719 = CVsvZlfoYXkRsaVutYYdu41166079;     CVsvZlfoYXkRsaVutYYdu41166079 = CVsvZlfoYXkRsaVutYYdu54081396;     CVsvZlfoYXkRsaVutYYdu54081396 = CVsvZlfoYXkRsaVutYYdu87704002;     CVsvZlfoYXkRsaVutYYdu87704002 = CVsvZlfoYXkRsaVutYYdu8254003;     CVsvZlfoYXkRsaVutYYdu8254003 = CVsvZlfoYXkRsaVutYYdu89309166;     CVsvZlfoYXkRsaVutYYdu89309166 = CVsvZlfoYXkRsaVutYYdu14481405;     CVsvZlfoYXkRsaVutYYdu14481405 = CVsvZlfoYXkRsaVutYYdu17427873;     CVsvZlfoYXkRsaVutYYdu17427873 = CVsvZlfoYXkRsaVutYYdu64386727;     CVsvZlfoYXkRsaVutYYdu64386727 = CVsvZlfoYXkRsaVutYYdu46140554;     CVsvZlfoYXkRsaVutYYdu46140554 = CVsvZlfoYXkRsaVutYYdu79631708;     CVsvZlfoYXkRsaVutYYdu79631708 = CVsvZlfoYXkRsaVutYYdu97430802;     CVsvZlfoYXkRsaVutYYdu97430802 = CVsvZlfoYXkRsaVutYYdu57645642;     CVsvZlfoYXkRsaVutYYdu57645642 = CVsvZlfoYXkRsaVutYYdu85620602;     CVsvZlfoYXkRsaVutYYdu85620602 = CVsvZlfoYXkRsaVutYYdu96519736;     CVsvZlfoYXkRsaVutYYdu96519736 = CVsvZlfoYXkRsaVutYYdu79382604;     CVsvZlfoYXkRsaVutYYdu79382604 = CVsvZlfoYXkRsaVutYYdu65701969;     CVsvZlfoYXkRsaVutYYdu65701969 = CVsvZlfoYXkRsaVutYYdu53653382;     CVsvZlfoYXkRsaVutYYdu53653382 = CVsvZlfoYXkRsaVutYYdu58360845;     CVsvZlfoYXkRsaVutYYdu58360845 = CVsvZlfoYXkRsaVutYYdu90816948;     CVsvZlfoYXkRsaVutYYdu90816948 = CVsvZlfoYXkRsaVutYYdu874351;     CVsvZlfoYXkRsaVutYYdu874351 = CVsvZlfoYXkRsaVutYYdu24344033;     CVsvZlfoYXkRsaVutYYdu24344033 = CVsvZlfoYXkRsaVutYYdu17515417;     CVsvZlfoYXkRsaVutYYdu17515417 = CVsvZlfoYXkRsaVutYYdu26702485;     CVsvZlfoYXkRsaVutYYdu26702485 = CVsvZlfoYXkRsaVutYYdu68264809;     CVsvZlfoYXkRsaVutYYdu68264809 = CVsvZlfoYXkRsaVutYYdu62383737;     CVsvZlfoYXkRsaVutYYdu62383737 = CVsvZlfoYXkRsaVutYYdu83683451;     CVsvZlfoYXkRsaVutYYdu83683451 = CVsvZlfoYXkRsaVutYYdu52388533;     CVsvZlfoYXkRsaVutYYdu52388533 = CVsvZlfoYXkRsaVutYYdu16779410;     CVsvZlfoYXkRsaVutYYdu16779410 = CVsvZlfoYXkRsaVutYYdu80352918;     CVsvZlfoYXkRsaVutYYdu80352918 = CVsvZlfoYXkRsaVutYYdu80450070;     CVsvZlfoYXkRsaVutYYdu80450070 = CVsvZlfoYXkRsaVutYYdu86988209;     CVsvZlfoYXkRsaVutYYdu86988209 = CVsvZlfoYXkRsaVutYYdu89494816;     CVsvZlfoYXkRsaVutYYdu89494816 = CVsvZlfoYXkRsaVutYYdu64191877;     CVsvZlfoYXkRsaVutYYdu64191877 = CVsvZlfoYXkRsaVutYYdu59972237;     CVsvZlfoYXkRsaVutYYdu59972237 = CVsvZlfoYXkRsaVutYYdu87589316;     CVsvZlfoYXkRsaVutYYdu87589316 = CVsvZlfoYXkRsaVutYYdu75028953;     CVsvZlfoYXkRsaVutYYdu75028953 = CVsvZlfoYXkRsaVutYYdu43816949;     CVsvZlfoYXkRsaVutYYdu43816949 = CVsvZlfoYXkRsaVutYYdu91347367;     CVsvZlfoYXkRsaVutYYdu91347367 = CVsvZlfoYXkRsaVutYYdu12389251;     CVsvZlfoYXkRsaVutYYdu12389251 = CVsvZlfoYXkRsaVutYYdu48083678;     CVsvZlfoYXkRsaVutYYdu48083678 = CVsvZlfoYXkRsaVutYYdu72057984;     CVsvZlfoYXkRsaVutYYdu72057984 = CVsvZlfoYXkRsaVutYYdu61889909;     CVsvZlfoYXkRsaVutYYdu61889909 = CVsvZlfoYXkRsaVutYYdu75437810;     CVsvZlfoYXkRsaVutYYdu75437810 = CVsvZlfoYXkRsaVutYYdu82996560;     CVsvZlfoYXkRsaVutYYdu82996560 = CVsvZlfoYXkRsaVutYYdu48761125;     CVsvZlfoYXkRsaVutYYdu48761125 = CVsvZlfoYXkRsaVutYYdu98559910;     CVsvZlfoYXkRsaVutYYdu98559910 = CVsvZlfoYXkRsaVutYYdu70436041;     CVsvZlfoYXkRsaVutYYdu70436041 = CVsvZlfoYXkRsaVutYYdu80797232;     CVsvZlfoYXkRsaVutYYdu80797232 = CVsvZlfoYXkRsaVutYYdu26843292;}
// Junk Finished
