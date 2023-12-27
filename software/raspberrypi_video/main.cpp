#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QMessageBox>

#include <QColor>
#include <QLabel>
#include <QtDebug>
#include <QString>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QSize>

#include "LeptonThread.h"
#include "MyLabel.h"

void printUsage(char *cmd) {
        char *cmdname = basename(cmd);
	printf("Usage: %s [OPTION]...\n"
               " -h      display this help and exit\n"
               " -cm x   select colormap\n"
               "           1 : rainbow\n"
               "           2 : grayscale\n"
               "           3 : ironblack [default]\n"
               " -tl x   select type of Lepton\n"
               "           2 : Lepton 2.x [default]\n"
               "           3 : Lepton 3.x\n"
               "               [for your reference] Please use nice command\n"
               "                 e.g. sudo nice -n 0 ./%s -tl 3\n"
               " -ss x   SPI bus speed [MHz] (10 - 30)\n"
               "           20 : 20MHz [default]\n"
               " -min x  override minimum value for scaling (0 - 65535)\n"
               "           [default] automatic scaling range adjustment\n"
               "           e.g. -min 30000\n"
               " -max x  override maximum value for scaling (0 - 65535)\n"
               "           [default] automatic scaling range adjustment\n"
               "           e.g. -max 32000\n"
               " -d x      log level (0-255)\n"
			   " -mid x    set AGC midpoint (127-65534)\n"
			   " -gain x   set AGC max gain (1-4)\n" 
			   " -AGC      set AGC policy (0,1,2)"
               "           [default] 0: off"
			   "	   	   1: Histogram Equalization 8-bit"
			   "		   2: Histogram Equalization 14-bit\n"
			   "           3: Linear Histogram Equalization\n"	
			   " -dampen x set HEQ dampening factor (0-100)\n"	
			   " -clipH x  set HEQ clip limit high (0-19200)\n"	   			
			   " -clipL x  set HEQ clip limit low (0-19200)\n"
			   " -binExt x set HEQ bin extension (0-16)\n"
			   " -empty x  set HEQ empty counts (0-65535)\n"
			   " -norm x   set HEQ normalization factor (0-65535)\n"
               "", cmdname, cmdname);
	return;
}

int main( int argc, char **argv )
{
	int typeColormap = 3; // colormap_ironblack
	int typeLepton = 2; // Lepton 2.x
	int spiSpeed = 20; // SPI bus speed 20MHz
	int rangeMin = -1; //
	int rangeMax = -1; //
	int loglevel = 0;

	int midpoint = 0;
	int gain = 0;
	int agcSelect = 0;

	uint16_t dampen = 0;
	uint16_t clipHigh = 0;
	uint16_t clipLow = 0;
	uint16_t binExtension = 0;
	uint16_t emptyCounts = 0;
	uint16_t normalizationFactor = 0;

	// VID Focus



	for(int i=1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0) {
			printUsage(argv[0]);
			exit(0);
		}
		else if (strcmp(argv[i], "-d") == 0) {
			int val = 3;
			if ((i + 1 != argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
				val = std::atoi(argv[i + 1]);
				i++;
			}
			if (0 <= val) {
				loglevel = val & 0xFF;
			}
		}
		else if ((strcmp(argv[i], "-cm") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((val == 1) || (val == 2)) {
				typeColormap = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-tl") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if (val == 3) {
				typeLepton = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-ss") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((10 <= val) && (val <= 30)) {
				spiSpeed = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-min") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((0 <= val) && (val <= 65535)) {
				rangeMin = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-max") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((0 <= val) && (val <= 65535)) {
				rangeMax = val;
				i++;
			}
		}

		else if ((strcmp(argv[i], "-mid") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((127 <= val) && (val <= 65534)) {
				midpoint = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-gain") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((1 <= val) && (val <= 4)) {
				gain = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-AGC") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((0 <= val) && (val <= 2)) {
				agcSelect = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-dampen") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((0 <= val) && (val <= 100)) {
				dampen = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-clipH") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((0 <= val) && (val <= 19200)) {
				clipHigh = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-clipL") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((0 <= val) && (val <= 19200)) {
				clipLow = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-binExt") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((0 <= val) && (val <= 16)) {
				binExtension = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-empty") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((0 <= val) && (val <= 65535)) {
				emptyCounts = val;
				i++;
			}
		}
		else if ((strcmp(argv[i], "-norm") == 0) && (i + 1 != argc)) {
			int val = std::atoi(argv[i + 1]);
			if ((0 <= val) && (val <= 65535)) {
				normalizationFactor = val;
				i++;
			}
		}
	}
	//create the app
    QApplication a(argc, argv);

    // Set the main window
    QWidget *myWidget = new QWidget;
    myWidget->setGeometry(20, 20, 850, 450); // Reduced size by 50 pixels for both width and height

    // Create an image placeholder
    QImage myImage(160, 120, QImage::Format_RGB888); // Original size
    QRgb red = qRgb(255, 0, 0);
    for(int i = 0; i < 80; i++) {
        for(int j = 0; j < 60; j++) {
            myImage.setPixel(i, j, red);
        }
    }

	// Create Boot Screen

    // Enlarge Image
	int IWidth = 550; // Adjusted width
	int IHeight = 408; // Adjusted height
    QSize maxSize(IWidth, IHeight); // Adjusted width and reduced height for the space at the top

    // Scale the image
    QImage scaledImage = myImage.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Create a label and set its image
    MyLabel myLabel(myWidget);
    myLabel.setGeometry(0, 0, scaledImage.width(), scaledImage.height()); // Start the image in the top left corner
    myLabel.setPixmap(QPixmap::fromImage(scaledImage));

    // Create a FFC button and adjust its position
    QPushButton *button1 = new QPushButton("Perform FFC", myWidget);
    button1->setGeometry(100, IHeight+5, 100, 30); // Adjusted to be centered at the bottom

	// Create a Freeze/Unfreeze button and adjust its position
	QPushButton *button2 = new QPushButton("Freeze/Unfreeze", myWidget);
	button2->setGeometry(250, IHeight+5, 100, 30); // Adjusted to be centered at the bottom

	/* Create Sliders to Adjust AGC Parameters */
	// Clip Low slider
	QSlider *slider1 = new QSlider(Qt::Vertical, myWidget);
	slider1->setGeometry(IWidth+20, 0, 20, IHeight); // Adjusted to be centered at the bottom
	slider1->setRange(0, 1024);
	slider1->setValue(clipLow);
	slider1->setTickPosition(QSlider::TicksAbove);
    slider1->setTickInterval(250);
	// Midpoint slider
	QSlider *slider2 = new QSlider(Qt::Vertical, myWidget);
	slider2->setGeometry(IWidth+50, 0, 20, IHeight); // Adjusted to be centered at the bottom
	slider2->setRange(28000, 33000);
	slider2->setValue(midpoint);
	slider2->setTickPosition(QSlider::TicksAbove);
    slider2->setTickInterval(1000);
	// Dampening slider
	QSlider *slider3 = new QSlider(Qt::Vertical, myWidget);
	slider3->setGeometry(IWidth+70, 0, 20, IHeight); // Adjusted to be centered at the bottom
	slider3->setRange(0, 100);
	slider3->setValue(dampen);
	slider3->setTickPosition(QSlider::TicksAbove);
    slider3->setTickInterval(10);




	//create a thread to gather SPI data
	//when the thread emits updateImage, the label should update its image accordingly
	LeptonThread *thread = new LeptonThread();
	thread->setLogLevel(loglevel);
	thread->useColormap(typeColormap);
	thread->useLepton(typeLepton);
	thread->useSpiSpeedMhz(spiSpeed);
	thread->setAutomaticScalingRange();
	//AGC Commands
	thread->selectAGC(agcSelect);
	thread->useMidpoint(midpoint);
	thread->useGain(gain);
	thread->HeqVariables(dampen, clipHigh, clipLow, binExtension, emptyCounts, normalizationFactor);


	if (0 <= rangeMin) thread->useRangeMinValue(rangeMin);
	if (0 <= rangeMax) thread->useRangeMaxValue(rangeMax);

	QObject::connect(thread, SIGNAL(updateImage(QImage)), &myLabel, SLOT(setImage(QImage)));
	
	//connect ffc button to the thread's ffc action
	QObject::connect(button1, SIGNAL(clicked()), thread, SLOT(performFFC()));
	// toggle freeze unfreeze button
	QObject::connect(button2, SIGNAL(clicked()), thread, SLOT(freezeToggle()));
	// slider for clip Low
	QObject::connect(slider1, SIGNAL(valueChanged(int)), thread, SLOT(clipLowSlider(int)));
	// slider for midpoint
	QObject::connect(slider2, SIGNAL(valueChanged(int)), thread, SLOT(midpointSlider(int)));
	// slider for dampening
	QObject::connect(slider3, SIGNAL(valueChanged(int)), thread, SLOT(dampenSlider(int)));

	thread->start();
	
	myWidget->show();

	return a.exec();
}

