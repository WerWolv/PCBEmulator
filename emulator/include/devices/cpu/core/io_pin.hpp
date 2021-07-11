#pragma once

namespace vc::dev::cpu {

    class IOPin {
    public:
        [[nodiscard]]
        auto getValue() {
            auto copy = this->value;
            this->value.reset();
            return copy;
        }

        void setValue(u8 value) {
            this->value = value;
        }

        [[nodiscard]]
        bool hasValue() {
            return this->value.has_value();
        }
    private:
        std::optional<u8> value;
    };

}