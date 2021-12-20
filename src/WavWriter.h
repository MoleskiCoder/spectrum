#pragma once

// lifted from https://github.com/philippegorley/wavfile
// wavfile was GPLv3
// TODO sort out spectrum project licensing!!

#include <fstream>
#include <locale>
#include <stdexcept>
#include <type_traits>

template<typename SampleFormat>
class WavWriter final {
private:
    std::ofstream m_os;
    size_t m_fact_position = 0;
    size_t m_data_position = 0;

    const std::string m_filename;
    const int m_channels = 0;
    const int m_sampling_rate = 0;

    bool m_started = false;

    [[nodiscard]] constexpr const auto& filename() const noexcept { return m_filename; }
    [[nodiscard]] constexpr const auto& channels() const noexcept { return m_channels; }
    [[nodiscard]] constexpr const auto& sampling_rate() const noexcept { return m_sampling_rate; }

    [[nodiscard]] constexpr const auto& os() const noexcept { return m_os; }
    [[nodiscard]] constexpr auto& os() noexcept { return m_os; }

    void writeHeader() {
        
        os() << "RIFF----WAVEfmt ";

        if (std::is_integral<SampleFormat>::value) {
            write<int32_t>(16);
            write<int16_t>(1);
        } else if (std::is_floating_point<SampleFormat>::value) {
            write<int32_t>(18);
            write<int16_t>(3);
        }

        write<int16_t>(channels());
        write<int32_t>(sampling_rate());
        write<int32_t>(sampling_rate() * sizeof(SampleFormat) * channels());
        write<int16_t>(sizeof(SampleFormat) * channels());
        write<int16_t>(8 * sizeof(SampleFormat));

        if (std::is_floating_point<SampleFormat>::value) {
            write<int16_t>(0);
            os() << "fact";
            write<int32_t>(4);
            m_fact_position = os().tellp();
            os() << "----"; // fact holder
        }

        os() << "data";
        m_data_position = os().tellp();
        os() << "----"; // data holder
    }

    void writeTrailer() {
        const int64_t length = os().tellp();
        os().seekp(m_data_position);
        write<int32_t>(length - m_data_position + 4); // bytes_per_sample * channels * nb_samples
        os().seekp(4);
        write<int32_t>(length - 8);
        if (m_fact_position > 0) {
            os().seekp(m_fact_position);
            write<int32_t>((length - m_data_position + 4) / sizeof(SampleFormat)); // channels * nb_samples
        }
    }

public:
    WavWriter(std::string filename, int channels, int sampling_rate)
    : m_filename(filename),
      m_channels(channels),
      m_sampling_rate(sampling_rate) {
        static_assert(std::is_arithmetic<SampleFormat>::value, "Sample type must be integral or floating point");
    }

    [[nodiscard]] constexpr auto started() const noexcept { return m_started; }
    [[nodiscard]] constexpr auto& started() noexcept { return m_started; }

    auto maybeOpen() {
        auto opening = !started();
        if (opening)
            open();
        return opening;
    }

    auto maybeClose() {
        auto closing = started();
        if (closing)
            close();
        return closing;
    }

    void open() {
        os() = std::ofstream(filename(), std::ios_base::out | std::ios_base::binary);
        os().imbue(std::locale::classic());
        writeHeader();
        started() = true;
    }

    void close() {
        writeTrailer();
        started() = false;
    }

    template<typename Word>
    void write(Word datum) {
        auto p = reinterpret_cast<const uint8_t*>(&datum);
        auto size = sizeof(Word);
        for (int i = 0; size != 0; --size, ++i)
            os().put(p[i]);
    }

    template <typename Iter>
    void write(Iter it, Iter end) {
        for (; it != end; ++it)
            write(*it);
    }
};
