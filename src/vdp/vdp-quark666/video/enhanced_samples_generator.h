#ifndef ENHANCED_SAMPLES_GENERATOR_H
#define ENHANCED_SAMPLES_GENERATOR_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <fabgl.h>

#include "audio_sample.h"
#include "types.h"

// Enhanced samples generator
//
class EnhancedSamplesGenerator : public WaveformGenerator {
	public:
		EnhancedSamplesGenerator(std::shared_ptr<audio_sample> sample);

		void setFrequency(int value);
		int getSample();

		int getDuration();
	
	private:
		std::weak_ptr<audio_sample> _sample;
};

EnhancedSamplesGenerator::EnhancedSamplesGenerator(std::shared_ptr<audio_sample> sample)
	: _sample(sample)
{}

void EnhancedSamplesGenerator::setFrequency(int value) {
	// usually this will do nothing...
	// but we'll hijack this method to allow us to reset the sample index
	// ideally we'd override the enable method, but C++ doesn't let us do that
	if (value < 0) {
		// rewind our sample if it's still valid
		if (!_sample.expired()) {
			auto samplePtr = _sample.lock();
			samplePtr->rewind();
		}
	}
}

int EnhancedSamplesGenerator::getSample() {
	if (duration() == 0 || _sample.expired()) {
		return 0;
	}

	auto samplePtr = _sample.lock();
	int sample = samplePtr->getSample();

	// process volume
	sample = sample * volume() / 127;

	decDuration();

	return sample;
}

int EnhancedSamplesGenerator::getDuration() {
	// NB this is hard-coded for a 16khz sample rate
	return _sample.expired() ? 0 : _sample.lock()->getDuration();
}

// FM generator
//
class FMGenerator : public WaveformGenerator {
	public:
		FMGenerator(std::unique_ptr<WaveformGenerator> nest, std::shared_ptr<audio_channel> dest);

		void setFrequency(int value) { frequency = 2 * value; nest->setFrequency(value); }
		void setVolume(int value) { nest->setVolume(value); }
		int volume() { return 0; } // null volume
		bool enabled() { return nest->enabled(); }
		void enable(bool value) { nest->enable(value); }
		uint32_t duration() { return nest->duration(); }
		
		int getSample();
	
	private:
		std::unique_ptr<WaveformGenerator> nest;
		std::shared_ptr<audio_channel> dest;
		int sample = 0;
		int frequency = 0;
};

FMGenerator::FMGenerator(std::unique_ptr<WaveformGenerator> nest, std::shared_ptr<audio_channel> dest) {
	this->nest = std::move(nest);
	this->dest = dest;
}

int FMGenerator::getSample() {
	sample = nest->getSample();
	int freq = (sample * frequency) >> 8;// gain right
	dest->setFrequency(freq);// double range at max volume
	return 0;//return modulator quiet
}

#endif // ENHANCED_SAMPLES_GENERATOR_Hstd::shared_ptr<audio_channel>
