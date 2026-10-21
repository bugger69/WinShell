#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <iostream>
#include <streambuf>
#include <string>
#include <windows.h>

/* Stream Buffers */
class PipeOutStreamBuf final : public std::streambuf {
private:
    HANDLE outhandle;
    DWORD lastErr = ERROR_SUCCESS;

protected:
    int_type overflow(int_type ch) override;
    std::streamsize xsputn(const char* data, std::streamsize) override;
    int sync() override;

public:
    explicit PipeOutStreamBuf(HANDLE ownedWriteHandle) noexcept;
    ~PipeOutStreamBuf() override;

    PipeOutStreamBuf(const PipeOutStreamBuf&) = delete;
    PipeOutStreamBuf& operator=(const PipeOutStreamBuf&) = delete;

    bool is_open() const noexcept;
    bool close_out_handle() noexcept;
    DWORD last_error() const noexcept;
};

class PipeInStreamBuf final : public std::streambuf {
private:
    HANDLE inhandle;
    std::string buffer;
    DWORD lastErr = ERROR_SUCCESS;

protected:
    int_type underflow() override;
    std::streamsize xsgetn(char* data, std::streamsize count) override;
    int sync() override;

public:
    explicit PipeInStreamBuf(HANDLE ownedReadHandle) noexcept;
    ~PipeInStreamBuf() override;

    PipeInStreamBuf(const PipeInStreamBuf&) = delete;
    PipeInStreamBuf& operator=(const PipeInStreamBuf&) = delete;

    bool is_open() const noexcept;
    bool close_in_handle() noexcept;
    DWORD last_error() const noexcept;
};

/* Stream Definitons */
class PipeOutStream final : public std::ostream {
private:
    PipeOutStreamBuf buf;
public:
    explicit PipeOutStream(HANDLE ownedWriteHandle);
    ~PipeOutStream() override;

    PipeOutStream(const PipeOutStream&) = delete;
    PipeOutStream& operator=(const PipeOutStream&) = delete;

    bool close() noexcept;
    DWORD last_error() const noexcept;
};

class PipeInStream final : public std::istream {
private:
    PipeInStreamBuf buf;
public:
    explicit PipeInStream(HANDLE ownedReadHandle);
    ~PipeInStream() override;

    PipeInStream(const PipeInStream&) = delete;
    PipeInStream& operator=(const PipeInStream&) = delete;

    bool close() noexcept;
    DWORD last_error() const noexcept;
};