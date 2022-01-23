#pragma once

#include <string>

#include "AudioFile.h"

// Simple multi-channel, single amplitude level wrapper to the "AudioFile" class.
template<typename InputT, typename OutputT>
class WavWriter final {
private:
	AudioFile<OutputT> m_audio;

	const std::string m_path;
	const int m_channels;
	const int m_samples;

	const InputT m_lowLevelInput;
	const InputT m_highLevelInput;
	const OutputT m_lowLevelOutput;
	const OutputT m_highLevelOutput;

	bool m_recording = false;

public:
	WavWriter(
		std::string path,
		int channels, int samples,
		InputT lowLevelInput, InputT highLevelInput,
		OutputT lowLevelOutput, OutputT highLevelOutput)
		: m_path(path),
		  m_channels(channels), m_samples(samples),
		  m_lowLevelInput(lowLevelInput), m_highLevelInput(highLevelInput),
		  m_lowLevelOutput(lowLevelOutput), m_highLevelOutput(highLevelOutput)
	{}

	[[nodiscard]] constexpr auto recording() const noexcept { return m_recording; }
	[[nodiscard]] constexpr const auto& path() const noexcept { return m_path; }
	[[nodiscard]] constexpr auto channels() const noexcept { return m_channels; }
	[[nodiscard]] constexpr auto samples() const noexcept { return m_samples; }

	[[nodiscard]] constexpr auto lowLevelInput() const noexcept { return m_lowLevelInput; }
	[[nodiscard]] constexpr auto highLevelInput() const noexcept { return m_highLevelInput; }
	[[nodiscard]] constexpr auto lowLevelOutput() const noexcept { return m_lowLevelOutput; }
	[[nodiscard]] constexpr auto highLevelOutput() const noexcept { return m_highLevelOutput; }

	void startRecording() {
		assert(!recording());
		m_audio.samples.clear();
		m_audio.setNumChannels(channels());
		m_audio.setNumSamplesPerChannel(samples());
		m_recording = true;
	}

	void stopRecording() {
		assert(recording());
		m_audio.save(path(), AudioFileFormat::Wave);
		m_audio.samples.clear();
		m_recording = false;
	}

	auto maybeStartRecording() {
		const auto starting = !recording();
		if (starting)
			startRecording();
		return starting;
	}

	auto maybeStopRecording() {
		const auto stopping = recording();
		if (stopping)
			stopRecording();
		return stopping;
	}

	void recordSample(int channel, OutputT sample) {
		assert(channel < channels());
		m_audio.samples[channel].push_back(sample);
	}

	void recordSample(OutputT sample) {
		for (int channel = 0; channel < channels(); ++channel)
			recordSample(channel, sample);
	}

	template<typename IteratorT>
	void recordSamples(IteratorT begin, IteratorT end) {
		for (auto sample = begin; sample != end; ++sample) {
			const auto low = *sample == lowLevelInput();
			const auto high = *sample == highLevelInput();
			assert(low || high);
			recordSample(low ? lowLevelOutput() : highLevelOutput());
		}
	}

	template<typename IteratorT>
	auto maybeRecordSamples(IteratorT begin, IteratorT end) {
		if (recording())
			recordSamples(begin, end);
		return recording();
	}
};
