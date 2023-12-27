#include <iostream>

#include "LeptonThread.h"

#include "Palettes.h"
#include "SPI.h"
#include "Lepton_I2C.h"

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27;

LeptonThread::LeptonThread() : QThread()
{
	//
	loglevel = 0;

	//
	typeColormap = 3; // 1:colormap_rainbow  /  2:colormap_grayscale  /  3:colormap_ironblack(default)
	selectedColormap = colormap_ironblack;
	selectedColormapSize = get_size_colormap_ironblack();

	//
	typeLepton = 2; // 2:Lepton 2.x  / 3:Lepton 3.x
	myImageWidth = 80;
	myImageHeight = 60;

	//
	spiSpeed = 20 * 1000 * 1000; // SPI bus speed 20MHz

	// min/max value for scaling
	autoRangeMin = true;
	autoRangeMax = true;
	rangeMin = 30000;
	rangeMax = 32000;

	//ROI
	startCol = 40;
	startRow = 20;
	endCol = 120;
	endRow = 100;

	midpoint = 0;	
	gain = 0;
	agcSelect = 0;
	dampen = 0;
	clipHigh = 0;
	clipLow = 0;
	binExtension = 0;
	emptyCounts = 0;
	normalizationFactor = 0;

	// VID Focus
}

LeptonThread::~LeptonThread() {
}

void LeptonThread::setLogLevel(uint16_t newLoglevel)
{
	loglevel = newLoglevel;
}

void LeptonThread::useColormap(int newTypeColormap)
{
	switch (newTypeColormap) {
	case 1:
		typeColormap = 1;
		selectedColormap = colormap_rainbow;
		selectedColormapSize = get_size_colormap_rainbow();
		break;
	case 2:
		typeColormap = 2;
		selectedColormap = colormap_grayscale;
		selectedColormapSize = get_size_colormap_grayscale();
		break;
	default:
		typeColormap = 3;
		selectedColormap = colormap_ironblack;
		selectedColormapSize = get_size_colormap_ironblack();
		break;
	}
}

void LeptonThread::useLepton(int newTypeLepton)
{
	switch (newTypeLepton) {
	case 3:
		typeLepton = 3;
		myImageWidth = 160;
		myImageHeight = 120;
		break;
	default:
		typeLepton = 2;
		myImageWidth = 80;
		myImageHeight = 60;
	}
}

void LeptonThread::useSpiSpeedMhz(unsigned int newSpiSpeed)
{
	spiSpeed = newSpiSpeed * 1000 * 1000;
}

void LeptonThread::setAutomaticScalingRange()
{
	autoRangeMin = true;
	autoRangeMax = true;
}

void LeptonThread::useRangeMinValue(uint16_t newMinValue)
{
	autoRangeMin = false;
	rangeMin = newMinValue;
}

void LeptonThread::useRangeMaxValue(uint16_t newMaxValue)
{
	autoRangeMax = false;
	rangeMax = newMaxValue;
}

void LeptonThread::useMidpoint(uint16_t newMidpoint)
{
	midpoint = newMidpoint;
}

void LeptonThread::useGain(uint16_t newGain)
{
	gain = newGain;
}

void LeptonThread::selectAGC(int newAGCSelect)
{
	agcSelect = newAGCSelect;
}

void LeptonThread::HeqVariables(uint16_t newDampen, uint16_t newClipHigh, uint16_t newClipLow, uint16_t newBinExtension, uint16_t newEmptyCounts, uint16_t newNormalizationFactor)
{
	dampen = newDampen;
	clipHigh = newClipHigh;
	clipLow = newClipLow;
	binExtension = newBinExtension;
	emptyCounts = newEmptyCounts;
	normalizationFactor = newNormalizationFactor;
}


/* Sliders and Buttons for AGC */
void LeptonThread::clipLowSlider(int adjClipLow) {
    SetHeqClipLow(adjClipLow);
	GetHeqClipLow();
}
void LeptonThread::midpointSlider(int adjMidpoint) {
	lepton_set_heq_midpoint(adjMidpoint);
	lepton_get_agc_heq_midpoint();
}
void LeptonThread::dampenSlider(int adjDampen) {
	SetHeqDampening(adjDampen);
	GetHeqDampening();
}

// VID Focus
int FRZ = 0;
void LeptonThread::freezeToggle() {
	if(FRZ == 0) {
		FRZ = 1;
		SetFreezeState();
	}
	else if (FRZ == 1) {
		FRZ = 3;
		SetEndFreezeState();
		FRZ = 0;
	}
}

void LeptonThread::run()
{
	//create the initial image
	myImage = QImage(myImageWidth, myImageHeight, QImage::Format_RGB888);

	const int *colormap = selectedColormap;
	const int colormapSize = selectedColormapSize;
	uint16_t minValue = rangeMin;
	uint16_t maxValue = rangeMax;
	float diff = maxValue - minValue;
	float scale = 255/diff;
	uint16_t n_wrong_segment = 0;
	uint16_t n_zero_value_drop_frame = 0;

	//open spi port
	SpiOpenPort(0, spiSpeed);

	if(agcSelect == 0){
		lepton_disable_agc();
	}
	else if(agcSelect == 1){
		lepton_enable_agc();
		lepton_set_policy_heq();
		lepton_set_heq_8bit_scale_factor();
	}
	else if(agcSelect == 2){
		lepton_enable_agc();
		lepton_set_policy_heq();
		lepton_set_heq_14bit_scale_factor();
	}
	else if(agcSelect == 3){
		lepton_enable_agc();
		lepton_set_policy_linear();
	}
	usleep(5000);
	check_agc_state();

	if(agcSelect != 0){
		lepton_set_agc_roi(60, 40, 100, 80);
		if(gain != 0){
			lepton_max_gain(gain);
		}
		if (midpoint != 0){
			lepton_set_heq_midpoint(midpoint);
		}
	}
	if(dampen != 0){
		SetHeqDampening(dampen);
	}
	if(clipHigh != 0){
		SetHeqClipHigh(clipHigh);
	}
	if(clipLow != 0){
		SetHeqClipLow(clipLow);
	}
	if(binExtension != 0){
		SetBinExtension(binExtension);
	}
	if(emptyCounts != 0){
		SetEmptyCounts(emptyCounts);
	}
	/*
	if(normalizationFactor != 0){
		SetNormalizationFactor(normalizationFactor);
	}
	*/

	lepton_get_agc_max_gain();
	lepton_get_agc_heq_midpoint();
	lepton_get_agc_roi();
	lepton_get_histogram_statistics();
	GetBinExtension();
	GetEmptyCounts();	
	GetHeqClipHigh();
	GetHeqClipLow();
	GetHeqDampening();
	//GetNormalizationFactor();	//what does this do?
	GetAgcCalculationState();
	lepton_get_aux_temp_kelvin();
	lepton_get_scene_statistics();

	/* FOCUS METRIC */

	if(agcSelect == 0) {
		//enbale focus metric
		SetFocusCalculationEnableState();
		GetFocusCalculationEnableState();
		GetFocusRoi();
		GetFocusMetric();
		GetFocusMetricThreshold();	
		GetSceneBasedNucEnableState();
	}


	int statLoop = 0;
	while(true) {
		statLoop ++;
		//read data packets from lepton over SPI
		int resets = 0;
		int segmentNumber = -1;
		for(int j=0;j<PACKETS_PER_FRAME;j++) {
			//if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
			read(spi_cs0_fd, result+sizeof(uint8_t)*PACKET_SIZE*j, sizeof(uint8_t)*PACKET_SIZE);
			int packetNumber = result[j*PACKET_SIZE+1];
			if(packetNumber != j) {
				log_message(4, "[ERROR] Packet number mismatch. PacketNumber: " + std::to_string(packetNumber) + " j: " + std::to_string(j));
				j = -1;
				resets += 1;
				usleep(1000);
				//Note: we've selected 750 resets as an arbitrary limit, since there should never be 750 "null" packets between two valid transmissions at the current poll rate
				//By polling faster, developers may easily exceed this count, and the down period between frames may then be flagged as a loss of sync
				if(resets == 750) {
					SpiClosePort(0);
					lepton_reboot();
					n_wrong_segment = 0;
					n_zero_value_drop_frame = 0;
					usleep(750000);
					SpiOpenPort(0, spiSpeed);
				}
				continue;
			}
			if ((typeLepton == 3) && (packetNumber == 20)) {
				segmentNumber = (result[j*PACKET_SIZE] >> 4) & 0x0f;
				if ((segmentNumber < 1) || (4 < segmentNumber)) {
					log_message(10, "[ERROR] Wrong segment number " + std::to_string(segmentNumber));
					break;
				}
			}
		}
		if(resets >= 30) {
			log_message(3, "done reading, resets: " + std::to_string(resets));
		}


		//
		int iSegmentStart = 1;
		int iSegmentStop;
		if (typeLepton == 3) {
			if ((segmentNumber < 1) || (4 < segmentNumber)) {
				n_wrong_segment++;
				if ((n_wrong_segment % 12) == 0) {
					log_message(5, "[WARNING] Got wrong segment number continuously " + std::to_string(n_wrong_segment) + " times");
				}
				continue;
			}
			if (n_wrong_segment != 0) {
				log_message(8, "[WARNING] Got wrong segment number continuously " + std::to_string(n_wrong_segment) + " times [RECOVERED] : " + std::to_string(segmentNumber));
				n_wrong_segment = 0;
			}

			//
			memcpy(shelf[segmentNumber - 1], result, sizeof(uint8_t) * PACKET_SIZE*PACKETS_PER_FRAME);
			if (segmentNumber != 4) {
				continue;
			}
			iSegmentStop = 4;
		}
		else {
			memcpy(shelf[0], result, sizeof(uint8_t) * PACKET_SIZE*PACKETS_PER_FRAME);
			iSegmentStop = 1;
		}

		if ((autoRangeMin == true) || (autoRangeMax == true)) {
			if (autoRangeMin == true) {
				minValue = 65535;
			}
			if (autoRangeMax == true) {
				maxValue = 0;
			}
			for(int iSegment = iSegmentStart; iSegment <= iSegmentStop; iSegment++) {
				for(int i=0;i<FRAME_SIZE_UINT16;i++) {
					//skip the first 2 uint16_t's of every packet, they're 4 header bytes
					if(i % PACKET_SIZE_UINT16 < 2) {
						continue;
					}

					//flip the MSB and LSB at the last second
					uint16_t value = (shelf[iSegment - 1][i*2] << 8) + shelf[iSegment - 1][i*2+1];
					if (value == 0) {
						// Why this value is 0?
						continue;
					}
					if ((autoRangeMax == true) && (value > maxValue)) {
						maxValue = value;
					}
					if ((autoRangeMin == true) && (value < minValue)) {
						minValue = value;
					}
				}
			}
			diff = maxValue - minValue;
			scale = 255/diff;
		}

		int row, column;
		uint8_t value8;
		uint8_t valueFrameBuffer8;
		uint16_t value;
		uint16_t valueFrameBuffer;
		QRgb color;
		for(int iSegment = iSegmentStart; iSegment <= iSegmentStop; iSegment++) {
			int ofsRow = 30 * (iSegment - 1);
			for(int i=0;i<FRAME_SIZE_UINT16;i++) {
				//skip the first 2 uint16_t's of every packet, they're 4 header bytes
				if(i % PACKET_SIZE_UINT16 < 2) {
					continue;
				}

				//flip the MSB and LSB at the last second
				if(agcSelect == 1){
					valueFrameBuffer8 = shelf[iSegment - 1][i * 2 + 1];
				}
				else{
					valueFrameBuffer = (shelf[iSegment - 1][i*2] << 8) + shelf[iSegment - 1][i*2+1];
				}
				
				if (valueFrameBuffer == 0 && agcSelect != 1) {
					// Why this value is 0?
					n_zero_value_drop_frame++;
					if ((n_zero_value_drop_frame % 12) == 0) {
						log_message(5, "[WARNING] Found zero-value. Drop the frame continuously " + std::to_string(n_zero_value_drop_frame) + " times");
					}
					break;
				}

				//declare ofs
				int ofs_r, ofs_g, ofs_b;
				if(agcSelect == 1){
					value8 = valueFrameBuffer8;
					log_message(2, "value: " + std::to_string(value8));
					ofs_r = 3 * value8 + 0; if (colormapSize <= ofs_r) ofs_r = colormapSize - 1;
					ofs_g = 3 * value8 + 1; if (colormapSize <= ofs_g) ofs_g = colormapSize - 1;
					ofs_b = 3 * value8 + 2; if (colormapSize <= ofs_b) ofs_b = colormapSize - 1;
				}
				else{
					value = (valueFrameBuffer - minValue) * scale;
					log_message(2, "value: " + std::to_string(value));
					ofs_r = 3 * value + 0; if (colormapSize <= ofs_r) ofs_r = colormapSize - 1;
					ofs_g = 3 * value + 1; if (colormapSize <= ofs_g) ofs_g = colormapSize - 1;
					ofs_b = 3 * value + 2; if (colormapSize <= ofs_b) ofs_b = colormapSize - 1;
				}				
				color = qRgb(colormap[ofs_r], colormap[ofs_g], colormap[ofs_b]);
				if (typeLepton == 3) {
					column = (i % PACKET_SIZE_UINT16) - 2 + (myImageWidth / 2) * ((i % (PACKET_SIZE_UINT16 * 2)) / PACKET_SIZE_UINT16);
					row = i / PACKET_SIZE_UINT16 / 2 + ofsRow;
				}
				else {
					column = (i % PACKET_SIZE_UINT16) - 2;
					row = i / PACKET_SIZE_UINT16;
				}
				myImage.setPixel(column, row, color);
			}
		}

		if (n_zero_value_drop_frame != 0) {
			log_message(8, "[WARNING] Found zero-value. Drop the frame continuously " + std::to_string(n_zero_value_drop_frame) + " times [RECOVERED]");
			n_zero_value_drop_frame = 0;
		}

		//update statistics periodically
		if (statLoop % 20 == 0) {
			lepton_get_scene_statistics();
			GetAgcCalculationState();
			GetFocusMetric();

		}
		//lets emit the signal for update
		emit updateImage(myImage);
	}
	
	//finally, close SPI port just bcuz
	SpiClosePort(0);
}

void LeptonThread::performFFC() {
	//perform FFC
	lepton_perform_ffc();
}

void LeptonThread::log_message(uint16_t level, std::string msg)
{
	if (level <= loglevel) {
		std::cerr << msg << std::endl;
	}
}

void LeptonThread::enableAGC() {
    lepton_enable_agc();
}

void LeptonThread::setHeqPolicy() {
    lepton_set_policy_heq();
}

// HEQ 8-bit Scale Factor
void LeptonThread::setHeqScaleFactor8() {
	lepton_set_heq_8bit_scale_factor();
}

// HEQ 14-bit Scale Factor
void LeptonThread::setHeqScaleFactor14() {
	lepton_set_heq_14bit_scale_factor();
}
