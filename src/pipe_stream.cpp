#include "pipe_stream.hpp"

#include <algorithm>
#include <cstring>
#include <limits>

namespace {
    bool valid_handle(HANDLE handle) noexcept {
        return handle != nullptr && handle != INVALID_HANDLE_VALUE;
    }
}

/* Pipe Out Stream */
PipeOutStreamBuf::PipeOutStreamBuf(HANDLE ownedWriteHandle) noexcept 
: outhandle(ownedWriteHandle) {
    if(!valid_handle(outhandle)) {
        outhandle = INVALID_HANDLE_VALUE;
        lastErr = ERROR_INVALID_HANDLE;
    }
}

PipeOutStreamBuf::~PipeOutStreamBuf() {
    close_out_handle();
}

bool PipeOutStreamBuf::is_open() const noexcept {
    return valid_handle(outhandle);
}

DWORD PipeOutStreamBuf::last_error() const noexcept {
    return lastErr;
}

PipeOutStreamBuf::int_type PipeOutStreamBuf::overflow(int_type ch) {
    if(traits_type::eq_int_type(ch, traits_type::eof())) {
        return is_open() ? traits_type::not_eof(ch) : traits_type::eof();
    }
    const char byte = traits_type::to_char_type(ch);
    return xsputn(&byte, 1) == 1 ? ch : traits_type::eof();
}

std::streamsize PipeOutStreamBuf::xsputn(const char* data, std::streamsize size) {
    if(size <= 0) return 0;

    if(!is_open() || lastErr != ERROR_SUCCESS) {
        if(lastErr == ERROR_SUCCESS) {
            lastErr = ERROR_INVALID_HANDLE;
        }
        return 0;
    }

    std::streamsize totalWritten = 0;

    while (totalWritten < size) {
        const auto remaining = static_cast<unsigned long long>(size - totalWritten);
        const DWORD chunk = static_cast<DWORD>(
            (std::min)(
                remaining,
                static_cast<unsigned long long>(
                    (std::numeric_limits<DWORD>::max)()
                )
            )
        );
        DWORD bytesWritten = 0;
        if(!WriteFile(outhandle, data + totalWritten, chunk, &bytesWritten, nullptr)) {
            lastErr = GetLastError();
            break;
        }

        if(bytesWritten == 0) {
            lastErr = ERROR_WRITE_FAULT;
            break;
        }

        totalWritten += static_cast<std::streamsize>(bytesWritten);
    }

    return totalWritten;
}

int PipeOutStreamBuf::sync() {
    if(!is_open()) {
        if(lastErr == ERROR_SUCCESS) {
            lastErr = ERROR_INVALID_HANDLE;
        }
        return -1;
    }
    return lastErr == ERROR_SUCCESS ? 0 : -1;
}

bool PipeOutStreamBuf::close_out_handle() noexcept {
    if(!is_open()) {
        return true;
    }

    HANDLE handle = outhandle;
    outhandle = INVALID_HANDLE_VALUE;

    if(!CloseHandle(handle)) {
        lastErr = GetLastError();
        return false;
    }

    return true;
}

PipeOutStream::PipeOutStream(HANDLE ownedWriteHandle) : std::ostream(nullptr), buf(ownedWriteHandle) {
    rdbuf(&buf);
    if(!buf.is_open()) {
        setstate(std::ios::badbit);
    }
}

PipeOutStream::~PipeOutStream() {
    close();
}

bool PipeOutStream::close() noexcept {
    bool flushOk = !fail();

    if(buf.is_open()) {
        try {
            flush();
            flushOk = flushOk && !fail();
        } catch (...) {
            flushOk = false;
        }
    }

    return buf.close_out_handle() && flushOk;
}

DWORD PipeOutStream::last_error() const noexcept {
    return buf.last_error();
}

/* Pipe In Stream */
PipeInStreamBuf::PipeInStreamBuf(HANDLE ownedReadHandle) noexcept : inhandle(ownedReadHandle) {
    if(!valid_handle(inhandle)) {
        inhandle = INVALID_HANDLE_VALUE;
        lastErr = ERROR_INVALID_HANDLE;
    }
}

PipeInStreamBuf::~PipeInStreamBuf() {
    close_in_handle();
}

bool PipeInStreamBuf::is_open() const noexcept {
    return valid_handle(inhandle);
}

DWORD PipeInStreamBuf::last_error() const noexcept {
    return lastErr;
}

int PipeInStreamBuf::underflow() {
    if(gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    buffer.resize(4096);

    const std::streamsize bytesRead = xsgetn(buffer.data(), buffer.size());
    
    if(bytesRead <= 0) {
        buffer.clear();
        setg(nullptr, nullptr, nullptr);
        return traits_type::eof();
    }

    buffer.resize(static_cast<std::size_t>(bytesRead));

    setg(buffer.data(), buffer.data(), buffer.data() + buffer.size());

    return traits_type::to_int_type(*gptr());
}

std::streamsize PipeInStreamBuf::xsgetn(char* data, std::streamsize size) {
    if(size <= 0) {
        return 0;
    }

    if(!is_open() || lastErr != ERROR_SUCCESS) {
        if(lastErr == ERROR_SUCCESS) {
            lastErr = ERROR_INVALID_HANDLE;
        }
        return 0;
    }

    std::streamsize totalRead = 0;
    if(gptr() && gptr() < egptr()) {
        const auto available = egptr() - gptr();
        const auto count = (std::min)(size, static_cast<std::streamsize>(available));

        std::memcpy(data, gptr(), static_cast<size_t>(count));
        gbump(static_cast<int>(count));
        totalRead += count;
    }

    while(totalRead < size) {
        const auto remaining = static_cast<unsigned long long>(size - totalRead);

        const DWORD chunk = static_cast<DWORD>((std::min)(remaining, static_cast<unsigned long long>((std::numeric_limits<DWORD>::max)())));
        
        DWORD bytesRead = 0;

        if(!ReadFile(inhandle, data + totalRead, chunk, &bytesRead, nullptr)) {
            const DWORD error = GetLastError();
            if(error != ERROR_BROKEN_PIPE) {
                lastErr = error;
            }
            break;
        }

        if(bytesRead == 0) {
            break;
        }

        totalRead += static_cast<std::streamsize>(bytesRead);
    }

    return totalRead;
}

int PipeInStreamBuf::sync() {
    if(!is_open()) {
        if(lastErr == ERROR_SUCCESS) {
            lastErr = ERROR_INVALID_HANDLE;
        }
        return -1;
    }
    return lastErr == ERROR_SUCCESS ? 0 : -1;
}

bool PipeInStreamBuf::close_in_handle() noexcept {
    if(!is_open()) return true;

    HANDLE curr = inhandle;
    inhandle = INVALID_HANDLE_VALUE;

    if(!CloseHandle(curr)) {
        lastErr = GetLastError();
        return false;
    }
    return true;
}

PipeInStream::PipeInStream(HANDLE ownedReadHandle) : std::istream(nullptr), buf(ownedReadHandle) {
    rdbuf(&buf);
    if(!buf.is_open()) {
        setstate(std::ios::badbit);
    }
}

PipeInStream::~PipeInStream() {
    close();
}

bool PipeInStream::close() noexcept {
    return buf.close_in_handle() && !fail();
}

DWORD PipeInStream::last_error() const noexcept {
    return buf.last_error();
}