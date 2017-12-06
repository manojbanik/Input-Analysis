
//	Programming Assignment One on MSIM 541 - Computer Graphics & Visualization
//	Input Analysis Using Histograms


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>

#include <math.h>
#pragma comment(lib, "glew32.lib")
#include <GL/glew.h>
#include <GL/glut.h>

#define PI 22.0/7.0

using namespace std;

//	File related
char *fileName1, *fileName2, *fileName3, *fileName4, *fileName5;
string currentFile;

//	Input data
float *data, minimum, maximum;
float sampleMean, samStdDev;
int numDataPoints;

//	Histogram
int numIntervals = 30;
float *endPoints, *prob, maxProb = -1;

//	Distribution
int curveType = 0;
int numCurvePoints = 100;
float* curveX = new float[numCurvePoints];
float* curveY = new float[numCurvePoints];


//Parameters
float mu = 2.2;
float sigma = 1;
float beta = 1.25;
float parameterStep = .05;


//	Display parameters
int width = 800;
int height = 600;
float world_x_min, world_x_max, world_y_min, world_y_max;
float axis_x_min, axis_x_max, axis_y_min, axis_y_max;


//	Find Y for a given X for Normal distribution
float findYforNormal(float x, float m, float s)
{
	return (1.0 / (sqrtf(2 * PI*s*s))) * exp(-((x - m)*(x - m)) / (2 * s*s));
}

//	Calculate the points for pdf for Normal distribution
void computeNormalFunc(float mu, float sigma)
{
	float step = (maximum - minimum) / (numCurvePoints - 1);

	for (int i = 0; i<numCurvePoints; i++)
	{
		curveX[i] = minimum + step*i;
		curveY[i] = findYforNormal(curveX[i], mu, sigma);
	}

}

//	Calculate the points for pdf for Exponential distribution
void computeExponentialFunc(float beta)
{
	float step = (maximum - minimum) / (numCurvePoints - 1);

	for (int i = 0; i<numCurvePoints; i++)
	{
		curveX[i] = minimum + step*i;
		curveY[i] = (1.0 / beta) * exp(-(curveX[i] / beta));
	}
}


//	Display a text according to world coordinates.
//	x & y determines the position, r, g & b for color, size for Font size
void outText(float x, float y, float r, float g, float b, int size, string text)
{
	glColor3f(r, g, b);

	//	Starting position for printing
	glRasterPos2f(x, y);

	int len, i;
	len = text.length();
	
	//	For small font size
	if (size == 1)
		for (i = 0; i < len; i++) 
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
		
	//	For medium font size
	else if (size == 2)
		for (i = 0; i < len; i++) 		
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, text[i]);
		
	//	For large font size
	else for (i = 0; i < len; i++) 
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

//	Display a text according to screen coordinates.
//	x & y determines the position, size for Font size
void outTextWin(int x, int y, int size, string text)
{
	int len, i;
	len = text.length();

	glWindowPos2i(x, y);

	//	For small font size
	if (size == 1)
		for (i = 0; i < len; i++)
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);

	//	For medium font size
	else if (size == 2)
		for (i = 0; i < len; i++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
}

void display(void)
{
	//glClearColor(0.98, 0.98, 0.98, 0);
	glClearColor(1.0, 1.0, 1.0, 0);		//	White background
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	
	glColor3f(0.3, 0.2, 0.5);

	glLineWidth(1.5);
	glBegin(GL_LINES);
	
	//	Draw the vertical line (Y-axis)
	glVertex2f(axis_x_min, 0);		
	glVertex2f(axis_x_min, world_y_max*.95);	

	//	Draw the maximum probability density mark
	glVertex2f(axis_x_min, maxProb);
	glVertex2f(axis_x_min + (maximum - minimum)*.015, maxProb);	

	glEnd();

	ostringstream buffer;	//	For converting float values in string

	buffer << maxProb;
	string str = buffer.str();

	glColor3f(0.6, 0.0, 0.0);
	//	Show the maximum probability value
	outText(axis_x_min + (maximum - minimum)*.015, maxProb*1.005, 0.5, 0.1, 0.7, 1, str+" = Max. Prob. Density");

	glColor3f(0.0, 0.0, 0.0);
	//	Display the word Data in big size
	outText(maximum - (maximum - minimum)*.03, maxProb*.05, 0.4, 0.2, 0.5, 3, "Data");

	//	Display the word Probability Density in big size
	outText(world_x_min + (world_x_max - world_x_min)*.05, maxProb*1.1, 0.4, 0.2, 0.5, 3, "Probability Density");


	
	//	Draw the histogram, all rectangles, height represents the frequency on that interval
	
	/*
	for (int i = 0; i<numIntervals; i++)
		glRectf(endPoints[i], 0, endPoints[i + 1], prob[i]);
	*/
	
	glColor3f(0.4, 0.8, 0.85);
	for (int i = 0; i<numIntervals; i++)
		glRectf(endPoints[i], 0, endPoints[i + 1], prob[i]);

	glColor3f(0.0, 0.0, 1.0);
	glLineWidth(1.0);
	
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < numIntervals; i++) {
		glVertex2f(endPoints[i], 0);
		glVertex2f(endPoints[i],prob[i]);
		glVertex2f(endPoints[i + 1], prob[i]);
		glVertex2f(endPoints[i + 1], 0);
	}
	glEnd();

	
	
	glLineWidth(2);
	glColor3f(0.92, 0.4, 0.065);
	//	Draw the distribution function line, joining the points (curveX[i], curveY[i]) 
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i<numCurvePoints; i++)
		glVertex2f(curveX[i], curveY[i]);
	glEnd();


	glLineWidth(1.5);
	glColor3f(0.4, 0.2, 0.5);
	glBegin(GL_LINES);

	//	Draw the horizontal line (X-axis)
	glVertex2f(axis_x_min - (axis_x_min-world_x_min)/2.0, 0);
	glVertex2f(axis_x_max, 0);
	

	//	Draw the vertical line (Y-axis)
	glVertex2f(axis_x_min, (world_y_min - axis_y_min) / 2.0);
	glVertex2f(axis_x_min, world_y_max*.95);

	//	Draw the maximum probability density mark
	glVertex2f(axis_x_min, maxProb);
	glVertex2f(axis_x_min + (maximum - minimum)*.015, maxProb);

	glEnd();
	

	//	Lets define some variable for display message on graphics window.
	int tX = width*(5.3 / 8.0);		//	tX will adjust as window size changes
	int tY = height*(5.5/6.0);		//	tY will adjust as window size changes
	int decY =(height/27.0);		//	By changing decY you can control the line-gap between messages.
	

	glColor3f(0.65, 0.14, 0.5);

	//	Display current file name
	outTextWin(tX, tY, 2, "File: " + currentFile);
	tY -= decY;

	buffer.str("");
	buffer << numDataPoints;

	//	Display number of data points
	outTextWin(tX, tY, 2, "Data Points: " + buffer.str());
	tY -= decY;
	tY -= decY / 2;


	//glColor3f(0.6, 0.4, 0.8);
	//glColor3f(0.07, 0.12, 0.47);
	glColor3f(0.0, 0.0, 1.0);

	buffer.str("");
	buffer << minimum;
	
	
	//	Display the minimum data value	
	outTextWin(tX, tY, 2, "Minimum: " + buffer.str());
	tY -= decY;
	
	//	Display the maximum data value
	buffer.str("");
	buffer << maximum;

	outTextWin(tX, tY, 2, "Maximum: " + buffer.str());
	tY -= decY;

	
	//intervals

	buffer.str("");
	buffer << numIntervals;
	
	//	Display total intervals number
	outTextWin(tX, tY, 2, "Intervals: " + buffer.str());
	tY -= decY;


	//glColor3f(0.2, 0.8, 0.2);

	buffer.str("");
	buffer << sampleMean;

	//	Display Sample Mean
	outTextWin(tX, tY, 2, "Sample Mean: " + buffer.str());
	tY -= decY;

	buffer.str("");
	buffer << samStdDev;

	//	Display Sample Standard Deviation
	outTextWin(tX, tY, 2, "Sample Std Dev: " + buffer.str());
	tY -= decY;

	tY -= decY/2;				//	Double line gap
	
	//glColor3f(1, 0, 0);
	//glColor3f(0.1, 0.5, 0.3);
	glColor3f(0.45, 0.26, 0.07);
	if (curveType == 0)		//	For "Normal Distribution"
	{
		outTextWin(tX, tY, 2, "Normal Distribution");
		tY -= decY;

		buffer.str("");
		buffer << mu;
		
		//	Display the value of "Mu:"
		outTextWin(tX, tY, 2, "Mu: " + buffer.str());
		tY -= decY;
		

		buffer.str("");
		buffer << sigma;
		
		//	Display the value of "Sigma:"
		outTextWin(tX, tY, 2, "Sigma: " + buffer.str());
		tY -= decY;
		
	}
	else if (curveType == 1)		//	For "Exponential Distribution"
	{
		
		
		outTextWin(tX, tY, 2, "Exponential dist");
		tY -= decY;
		
		buffer.str("");
		buffer << beta;
		
		//	Display the value of "beta"
		outTextWin(tX, tY, 2, "Beta: " + buffer.str());
		tY -= decY;
	}
	glFlush();
	glutSwapBuffers();

}

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


void computeProbability(int numIntervals)
{
	if (endPoints != NULL)	//	delete previous values if any
		delete endPoints;

	endPoints = new float[numIntervals + 1];

	//	Computing end-points for each interval (histogram)
	for (int i = 0; i <= numIntervals; i++)
	{
		endPoints[i] = minimum + ((maximum - minimum) / numIntervals)*i;
	}

	maxProb = -1;		//	For finding maximum probability density
	
	if (prob != NULL)	//	delete previous values if any
		delete prob;
	
	//	Find the width of each interval
	float barWidth = (maximum - minimum) / numIntervals;

	prob = new float[numIntervals];
	for (int i = 0; i<numIntervals; i++)
	{
		prob[i] = 0.0;
		for (int j = 0; j<numDataPoints; j++)
		{
			if (data[j] >= endPoints[i] && data[j]<endPoints[i + 1])
				prob[i] += 1.0;
		}
		prob[i] /= ((float)numDataPoints);	//	indicates bar area
		prob[i] /= barWidth;				//	indicates bar height

		if (prob[i]>maxProb)
			maxProb = prob[i];				//	Finding maximum probability density
	}
	
	world_y_min = -maxProb*.05;		// world_y_min = -maxProb*.05;
	world_y_max = maxProb*1.2;
}


void readFile(string fileName)
{
	ifstream inFile(fileName);

	if (!inFile.is_open())		//	file not exist or somehow it couldn't be opened
	{
		cout << fileName << " couldn't be opened.\n";
		system("pause");
		exit(1);
	}

	inFile >> numDataPoints;

	float sumData = 0.0;

	if (data != NULL) delete data;	//	when data contains previous values, delete them

	data = new float[numDataPoints];

	inFile >> data[0];	//	Read first data point

	sumData += data[0];	//	Summing all data points to find the Sample Mean

	minimum = data[0];	
	maximum = data[0];	//	Set minimum & maximum both to the first data

	//	To find maximum and minimum

	for (int i = 1; i < numDataPoints; i++)
	{
		inFile >> data[i];

		sumData += data[i];	//	Summing all data points to find the Sample Mean

		if (data[i] < minimum) minimum = data[i];
		else if (data[i] > maximum) maximum = data[i];
	}

	//	Find the sample mean
	sampleMean = sumData / (float)numDataPoints;

	//	Now find the Sample standard Deviation
	
	sumData = 0.0;	//	Just re-using the variable
	for (int i = 0; i < numDataPoints; i++)
	{
		sumData += ((data[i]) - sampleMean)*((data[i]) - sampleMean);
	}

	samStdDev = sqrt(sumData / (float)(numDataPoints - 1));	//	Sample standard Deviation


	//	Now set World coordinates and Axis values with respect to maximum & minimum
	world_x_min = minimum - (maximum - minimum) * .1;   //limits
	axis_x_min = minimum - (maximum - minimum)*.05;
	world_x_max = maximum + (maximum - minimum)*.1;
	axis_x_max = maximum + (maximum - minimum)*.05;
	
	//	Calculate the probability for the given data
	computeProbability(numIntervals);

	if (curveType == 0)
		computeNormalFunc(mu, sigma);
	else if (curveType == 1)
		computeExponentialFunc(beta);
}



void keyboard(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 'Q' || key == 27)
		exit(0);	//	Quit from program.

}

void specialKey(int key, int x, int y)
{
	if (curveType == 0)
	{
		if (key == GLUT_KEY_RIGHT)
		{
			mu += parameterStep;
			if (mu >= 5.0) mu = 4.99;	//	0 <= mu < 5.0
		}

		if (key == GLUT_KEY_LEFT)
		{
			mu -= parameterStep;
			if (mu < 0.0) mu = 0.0;	//	0 <= mu < 5.0
		}

		if (key == GLUT_KEY_UP)
		{
			sigma += parameterStep;
			if (sigma >= 3.0) sigma = 2.99;	//	0.02 < sigma < 3.0
		}

		if (key == GLUT_KEY_DOWN)
		{
			sigma -= parameterStep;
			if (sigma <= 0.02) sigma = 0.03;	//	0.02 < sigma < 3.0
		}

		computeNormalFunc(mu, sigma);	//	Compute the Normal function again
	}
	else if (curveType == 1)
	{
		if (key == GLUT_KEY_UP)
		{
			beta += parameterStep;
			if (beta >= 6.0) beta = 5.99;	//	0.1 < beta < 6.0
		}

		if (key == GLUT_KEY_DOWN)
		{
			beta -= parameterStep;
			if (beta <= 0.1) beta = 0.11;	//	0.1 < beta < 6.0
		}
			
		computeExponentialFunc(beta);	//	Compute the Exponential function again
	}
	
	glutPostRedisplay();	//	Re-display all
}

//	For Adjust and Exit menu option.
void topMenuFunc(int id)	
{
	switch (id)
	{
	case 1:
		if (sampleMean - samStdDev >= 0)
		{
			if (sampleMean - samStdDev < 0.1)	//	Will fit Exponential Distribution
			{
				beta = sampleMean;
				computeExponentialFunc(beta);
			}
			else
			{
				mu = sampleMean;
				sigma = samStdDev;
				computeNormalFunc(mu, sigma);
			}
		}
		else
		{
			if (samStdDev - sampleMean < 0.1)	//	Will fit Exponential Distribution
			{
				beta = sampleMean;
				computeExponentialFunc(beta);
			}
			else
			{
				mu = sampleMean;
				sigma = samStdDev;
				computeNormalFunc(mu, sigma);
			}
		}
		break;

	case 2:		//	For Exit menu option
		exit(0);
	}

	glutPostRedisplay();
}

void fileMenuFunction(int id)
{
	switch (id)
	{
	case 1:
		currentFile = fileName1;
		break;
	case 2:
		currentFile = fileName2;
		break;
	case 3:
		currentFile = fileName3;
		break;
	case 4:
		currentFile = fileName4;
		break;
	case 5:
		currentFile = fileName5;
	}
	
	readFile(currentFile);	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(world_x_min, world_x_max, world_y_min, world_y_max);

	glutPostRedisplay();

}

void funcMenuFunction(int id)
{
	switch (id)
	{
	case 1:
		curveType = 0;
		computeNormalFunc(mu, sigma);
		break;
	case 2:
		curveType = 1;
		computeExponentialFunc(beta);
		break;
	}

	glutPostRedisplay();
}

void histogramMenuFunction(int id)
{
	switch (id)
	{
	case 1:
		numIntervals = 30;
		computeProbability(numIntervals);
		break;
	case 2:
		numIntervals = 40;
		computeProbability(numIntervals);
		break;
	case 3:
		numIntervals = 50;
		computeProbability(numIntervals);
		break;
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(world_x_min, world_x_max, world_y_min, world_y_max);
	glutPostRedisplay();
}

void parameterStepMenuFunction(int id)
{
	switch (id)
	{
	case 1:
		parameterStep = .01;
		break;
	case 2:
		parameterStep = .02;
		break;
	case 3:
		parameterStep = .05;
		break;
	}
}

void createMenu()
{
	int fileMenu = glutCreateMenu(fileMenuFunction);
	glutAddMenuEntry(fileName1, 1);
	glutAddMenuEntry(fileName2, 2);
	glutAddMenuEntry(fileName3, 3);
	glutAddMenuEntry(fileName4, 4);
	glutAddMenuEntry(fileName5, 5);

	int funcMenu = glutCreateMenu(funcMenuFunction);
	glutAddMenuEntry("Normal", 1);
	glutAddMenuEntry("Exponential", 2);
	
	int histogramMenu = glutCreateMenu(histogramMenuFunction);
	glutAddMenuEntry("30", 1);
	glutAddMenuEntry("40", 2);
	glutAddMenuEntry("50", 3);

	int stepMenu = glutCreateMenu(parameterStepMenuFunction);
	glutAddMenuEntry(".01", 1);
	glutAddMenuEntry(".02", 2);
	glutAddMenuEntry(".03", 3);

	int topMenu = glutCreateMenu(topMenuFunc);
	glutAddSubMenu("File", fileMenu);
	glutAddSubMenu("Distribution", funcMenu);
	glutAddSubMenu("Histogram", histogramMenu);
	glutAddSubMenu("Parameter step", stepMenu);
	glutAddMenuEntry("Adjust", 1);
	glutAddMenuEntry("Exit", 2);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

}

void reshape(int w, int h)
{
	width = w, height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(world_x_min, world_x_max, world_y_min, world_y_max);

}

int main(int argc, char** argv)
{
	fileName1 = "expo.dat";
	fileName2 = "normal.dat";
	fileName3 = "4.dat";
	fileName4 = "13.dat";
	fileName5 = "10.dat";
	

	currentFile = fileName1;
	readFile(currentFile);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Input Analysis Using Histograms");
	glewInit();
	createMenu();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}

