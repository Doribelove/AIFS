#pragma once

#include <string>

namespace aifs::common {

class Status {
public:
    enum class Code {
        kOk,
        kNotFound,
        kInvalidArgument,
        kResourceExhausted,
        kAlreadyExists,
        kInternal,
    };

    Status();
    Status(Code code, std::string message);

    static Status Ok();
    static Status NotFound(const std::string& message);
    static Status InvalidArgument(const std::string& message);
    static Status ResourceExhausted(const std::string& message);
    static Status AlreadyExists(const std::string& message);
    static Status Internal(const std::string& message);

    bool ok() const;
    Code code() const;
    const std::string& message() const;
    std::string ToString() const;

private:
    Code code_;
    std::string message_;
};

}  // namespace aifs::common

