#include <cstdint>
#include <limits>

class PCGRNG {
public:
    PCGRNG(uint64_t seed = 0x853c49e6748fea9bULL, uint64_t seq = 0xda3e39cb94b95bdbULL) {
        state = 0U;
        inc = (seq << 1u) | 1u;
        next();
        state += seed;
        next();
    }

    uint32_t next() {
        uint64_t oldstate = state;
        state = oldstate * 6364136223846793005ULL + inc;
        uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
        uint32_t rot = static_cast<uint32_t>(oldstate >> 59u);
        return (xorshifted >> rot) | (xorshifted << ((32 - rot) & 31));
    }

    double Double(double min, double max) {
        return min + (max - min) * (next() / static_cast<double>(std::numeric_limits<uint32_t>::max() + 1.0));
    }

    float Float(float min, float max) {
        return min + (max - min) * (next() / static_cast<float>(std::numeric_limits<uint32_t>::max() + 1.0));
    }

    int Int(int min, int max) {
        return min + next() % (max - min + 1);
    }

private:
    uint64_t state;
    uint64_t inc;
};