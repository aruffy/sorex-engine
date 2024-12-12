/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022 Aleksandr Ershov (Ruffy).                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "Types.h"
#include "RuntimeClass.h"
#include "Platform.h"
#include "Status.h"

#define SRX_UNKNOWN_SIZE (static_cast<Sorex::ssize_t>(-1))
#define SRX_READ_ERROR SRX_UNKNOWN_SIZE
#define SRX_WRITE_ERROR SRX_UNKNOWN_SIZE

namespace Sorex
{
  enum class EAccessMode
  {
    None      = 0,
    Read      = (1 << 0),
    Write     = (1 << 1),
    ReadWrite = Read | Write
  };

  /**
   * The different ways you can seek within a stream.
   */
  enum class ESeekMode : uint8
  {
    Begin = 0,
    Current,
    End
  };

  class SRX_API Stream
  {
    SRX_RTTI_BASE(Stream)

public:
    virtual ~Stream() = default;

    /**
     * @brief Check if the stream is ready to perform the specified mode
     * operation.
     *
     * @param mode The access mode to check
     *
     * @return Returns true if the stream is ready for the specified mode, false
     * otherwise.
     */
    virtual bool Check(const EAccessMode mode) const SRX_NOEXCEPT
    {
      return false;
    }

    /**
     * @brief Check if the stream is in a valid state.
     *
     * @return True if the stream is ready to work; otherwise, false.
     */
    virtual bool IsOpen() const SRX_NOEXCEPT { return false; }

    /**
     * @brief Check if there is no data to read.
     *
     * @return True if stream reached end of data.
     */
    virtual bool EndOfFile() SRX_NOEXCEPT { return true; }

    /**
     * @brief Return name of stream.
     *
     * @return name of the stream.
     */
    virtual StringView GetName() const SRX_NOEXCEPT { return {}; }

    /**
     * @brief Retrieve the current length of the stream data.
     *
     * This function calculates the current length of the data in the stream.
     *
     * @return length of data in bytes. If the length of the stream cannot be
     * calculated or an error has occurred, it returns SRX_UNKNOWN_SIZE.
     */
    virtual ssize_t GetLength() const SRX_NOEXCEPT { return SRX_UNKNOWN_SIZE; };

    /**
     * @brief Retrieve current position of the stream data.
     *
     * @return the index of position, or SRX_UNKNOWN_SIZE if the stream has
     * invalid state.
     */
    virtual ssize_t GetPosition() const SRX_NOEXCEPT
    {
      return SRX_UNKNOWN_SIZE;
    }

    /**
     * @brief Change the position of the stream.
     *
     * @param pos New position to seek to.
     * @param mode The mode of seeking (e.g., from the beginning, current
     * position, or end).
     * @return True if the seek operation was successful, false otherwise.
     */
    virtual bool Seek(int32 pos, ESeekMode mode) SRX_NOEXCEPT { return false; };

    /**
     * @brief Read a byte from the stream without changing the position.
     *
     * @param value Reference to a byte variable where the read value will be
     * stored.
     * @return Returns true if the byte was successfully read, false otherwise.
     */
    virtual bool Peek(byte& value) SRX_NOEXCEPT { return false; }

    /**
     * @brief Try to read `length` bytes to the `buffer`. If length is not
     * specified, it tries to read bytes according to the buffer size.
     *
     * @param buffer Array to read.
     * @param length Number of bytes to read, or SRX_UNKNOWN_SIZE to read the
     * entire buffer.
     * @return Number of bytes read, or SRX_READ_ERROR if an error occurred.
     */
    virtual ssize_t Read(TSpan<byte> buffer,
                         ssize_t length = SRX_UNKNOWN_SIZE) SRX_NOEXCEPT = 0;
    virtual bool    Next(byte& value) SRX_NOEXCEPT;
    virtual ssize_t ReadAll(TVector<byte>& buffer) SRX_NOEXCEPT;

    /**
     * @brief Writes data to the output.
     *
     * @param buffer A constant span of bytes to be written.
     * @return The number of bytes written, or a SRX_WRITE_ERROR value on error.
     */
    virtual ssize_t Write(TSpan<const byte> buffer) SRX_NOEXCEPT = 0;
    virtual bool    Write(const byte value) SRX_NOEXCEPT;
    virtual ssize_t WriteAll(TSpan<const byte> buffer) SRX_NOEXCEPT;

    /**
     * @brief Reset the state of the stream.
     *
     * This function resets the stream to its initial state, including resetting
     * the position and clearing any errors.
     *
     * @return true if the reset was successful, false otherwise.
     */
    virtual bool Reset() SRX_NOEXCEPT
    {
      mStatus = std::nullopt;
      return true;
    }

    /**
     * @brief Retrieves the status of the stream. The status holds error of the
     * last failed operation or SRX_OK.
     *
     * @return stream status
     */
    SRX_INLINE Status GetStatus() const SRX_NOEXCEPT
    {
      return mStatus.value_or(SRX_OK);
    }

protected:
    TOptional<Status> mStatus;
  };
}
