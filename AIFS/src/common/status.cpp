#include "aifs/common/status.h"

#include <sstream>

namespace aifs::common {

Status::Status() : code_(Code::kOk), message_("OK") {}

Status::Status(Code code, std::string message)
    : code_(code), message_(std::move(message)) {}

Status Status::Ok() {
    return Status{};
}

Status Status::NotFound(const std::string& message) {
    return Status(Code::kNotFound, message);
}

Status Status::InvalidArgument(const std::string& message) {
    return Status(Code::kInvalidArgument, message);
}

Status Status::ResourceExhausted(const std::string& message) {
    return Status(Code::kResourceExhausted, message);
}

Status Status::AlreadyExists(const std::string& message) {
    return Status(Code::kAlreadyExists, message);
}

Status Status::Internal(const std::string& message) {
    return Status(Code::kInternal, message);
}

bool Status::ok() const {
    return code_ == Code::kOk;
}

Status::Code Status::code() const {
    return code_;
}

const std::string& Status::message() const {
    return message_;
}

std::string Status::ToString() const {
    if (ok()) {
        return "OK";
    }
    std::ostringstream os;
    os << "error(" << static_cast<int>(code_) << "): " << message_;
    return os.str();
}

}  // namespace aifs::common
