#ifndef TERMINALGRPHICS_V1_1_H
#define TERMINALGRPHICS_V1_1_H

//TerminalGraphics By Rajas Sharma
//This is a class containing functions for renderinf graphics into terminal using ascii chars
//It is ideal for quick and easy graphical applications or prototypes because of the speed of setup and use
//This method supports direct pixel manipulation and draw functions like line() and circle() (as regPolygon)

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cmath>
#include <fstream>
#include <sstream>
#include <cstdlib>
#define PI 3.14159265358979323846
namespace tglh {

	class TerminalGraphics {
	private:
		// ------- PRIVATE FUNCTIONS ------- --> dev. QOL functions

		void swap(double& a, double& b) { double tmp; tmp = a; a = b; b = tmp; }	//standard swap function for doubles

	public:

		// ------- DECLARE VARIABLES -------
		int wallx, wally;
		int framerate = 1;
		std::vector<std::vector<int>> grid;
		std::string tileset;
		std::string border = "  ";
		std::string world;		//the string that is printed to the terminal every frame

		std::vector < std::vector<std::vector<int>>> storedImages;


		// ------- INITSILISATION FUNCTIONS -------

		void setWindow(int wallx, int wally) {
			this->wallx = wallx; this->wally = wally;	//set wallx and wally
			std::vector<int> temp;
			for (int i = 0; i < wallx; i++) {
				temp.clear();
				for (int j = 0; j < wally; j++) {
					temp.push_back(0);
				}
				grid.push_back(temp);
			}
		}

		//tilseset function for QOL. only - dont want to manually set tileset because it is confusing and awkward to implement a default tileset 

		void setTileset(std::string tileset) { this->tileset = tileset; }		//create a tileset using chars - e.g. "123" --> "1" is tile 0
		void setBorder() { border = "|-"; }


		// ------- WORLD FUNCTIONS -------
		void sleep() { std::this_thread::sleep_for(std::chrono::microseconds(1000000 / framerate)); }
		void clear(int value) {		//clears the grid to a value --> method accepts argument for value to set grid (x,y) to for tileset manipulation
			for (int i = 0; i < wally; i++) {
				for (int j = 0; j < wallx; j++) {
					grid[j][i] = value;
				}
			}
		}
		void render() {		//access what is in grid array (int) - accesses according tile from tilseset string
			world = "";
			for (int i = 0; i < wally; i++) {
				for (int j = 0; j < wallx; j++) {

					//have to have duplicate lines of code otherwise it skips every other tile for some reason

					if (grid[j][i] >= tileset.size()) { std::cerr <<'\n' << "\033[1;4;31m [ERROR] ATTEMPTED TO ACCESS TILESET INDEX OUT OF BOUNDS\033[0m\n"; std::exit(EXIT_FAILURE); }

					world += tileset[grid[j][i]]; world += tileset[grid[j][i]];
				}
				world += border[0]; world += '\n';
			}
			for (int i = 0; i < wallx; i++) { world += border[1]; world += border[1]; }
			std::cout << "\033[2J\033[1;1H";	//ANSI escape code to clear terminal (doesnt work partially??!?)
			std::cout << world;
		}

		// -------  GRAPHICAL FUNCTIONS  -------

		//takes many small steps from point one to point two and 
		//sets the grid position that is lands on to specified int
		//used for drawing lines
		void line(double x1, double y1, double x2, double y2, int steps, int value) {
			if(x1==x2&&y1==y2){grid[round(x1)][round(y1)]=value;}
			else{
				int vx = x2 - x1; int vy = y2 - y1;
				double error = 0.01;
				double stepsizeX = (vx + error) / (steps + error); double stepsizeY = (vy + error) / (steps + error);
				for (int i = 0; i < steps + 1; i++) {
					int x = round(x1 + stepsizeX * i);
					int y = round(y1 + stepsizeY * i);
					if (x >= 0 && x < wallx
						&& y >= 0 && y < wally) { //check if point in bounds
						grid[x][y] = value;
					}
				}
			}
		}

		//[INSERT] an attempt at using Bresenham's line algorithm :) 
		// --> find out more at https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
		//more efficient than line() function

		//picks two points around center and draw a line between them
		//two points are found by using trig and varying an angle theta 
		//around center
		void regPolygon(double x, double y, double radius, int vertecies, int value, double theta1) {
			double error = 0.001;
			double theta = theta1;
			int resolution = 2000;
			double deltaX = radius * cos(theta);
			double deltaY = radius * sin(theta);
			for (int i = 0; i < vertecies; i++) {
				theta += (PI * 2) / (vertecies);
				double deltaX1 = radius * cos(theta);
				double deltaY1 = radius * sin(theta);
				line((int)(x + deltaX), (int)(y + deltaY), (int)(x + deltaX1), (int)(y + deltaY1), resolution, value);
				deltaX = deltaX1;
				deltaY = deltaY1;
			}
		}

		//draw rect using 4 lines
		void rect(double x1, double y1, double x2, double y2, int value) {
			int steps = 100;
			line(x1, y1, x2, y1, steps, value);
			line(x2, y1, x2, y2, steps, value);
			line(x2, y2, x1, y2, steps, value);
			line(x1, y2, x1, y1, steps, value);
		}
		void fillrect(double x1, double y1, double x2, double y2, int value) {
			int steps = 100;
			if (x2 < x1) { swap(x2, x1); }
			if (y2 < y1) { swap(y2, y1); }
			for (int i = y1; i < y2 - y1; i++) {
				for (int j = x1; j < x2 - x1; j++) {
					if (j < wallx && j >= 0
						&& i < wally && i >= 0) { //check if in bounds
						grid[j][i] = value;
					}
				}
			}
			line(x1, y1, x2, y1, steps, value);
			line(x2, y1, x2, y2, steps, value);
			line(x2, y2, x1, y2, steps, value);
			line(x1, y2, x1, y1, steps, value);
		}
		void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, int steps, int value) {
			line(x1, y1, x2, y2, steps, value);
			line(x2, y2, x3, y4, steps, value);
			line(x3, y3, x4, y4, steps, value);
			line(x4, y4, x1, y1, steps, value);
		}

		//find equation of ellipse and draw lines between points that lie on the equation
		//equation is: ((xpos/radiusX)-x)^2 + ((ypos/radiusY)-y)^2 = 1
		//NOTE: the intended x,y (center) should be divided by radiusX and radiusY repectively because of the shift in center for the variaating radii
		//after rearragngin the euqation to find ypos : ypos = y +/- radiusY * sqrt(1 - ((xpos - x)/radiusX)^2)
		void ellipse(double x, double y, double radiusX, double radiusY, int steps, int value) {
			double error = 0.001;
			int resolution = 1000;
			double stepSize = 4 * radiusX / (steps + error);
			double xpos = x - radiusX, ypos = y;
			double xposhold, yposhold;

			//positive square root

			for (int i = 0; i < steps / 2; i++) {
				xposhold = xpos;
				yposhold = ypos;
				xpos += stepSize;
				ypos = y + radiusY * (sqrt(1 - pow(((xpos - x) / radiusX), 2)));
				line(xposhold, yposhold, xpos, ypos, resolution, value);
			}

			//negative square root

			xpos = x - radiusX, ypos = y;
			for (int i = 0; i < steps / 2; i++) {
				xposhold = xpos;
				yposhold = ypos;
				xpos += stepSize;
				ypos = y - radiusY * (sqrt(1 - pow(((xpos - x) / radiusX), 2)));
				line(xposhold, yposhold, xpos, ypos, resolution, value);
			}
		}

		// ------- IMAGED-BASED FUNCTIONS ---------

		void encodeImage() {
		}

		void drawImage(std::string filetxt, double x, double y, int xlengthN, int xlengthM, int ylengthN, int ylengthM) {  //NEED TO IMLEMENT XLENGTH AND YLENGTH

			if (xlengthM <=0) { std::cerr << '\n' << "\033[1;4;31m [ERROR] (drawImage) INVAID XLENGTH DIVISOR:\033[0m"<< xlengthM<<'\n'; std::exit(EXIT_FAILURE); }
			if (ylengthM <= 0) { std::cerr << '\n' << "\033[1;4;31m [ERROR] (drawImage) INVAID YLENGTH DIVISOR:\033[0m" << ylengthM << '\n'; std::exit(EXIT_FAILURE); }

			std::string inputFileType = filetxt.substr(filetxt.find('.'));

			if (inputFileType!=".txt") { std::cerr << '\n' << "\033[1;4;31m [ERROR] (drawImage) INVALID INPUT FILETYPE - PLEASE INPUT .txt FILE\033[0m\n"; std::exit(EXIT_FAILURE); }

			std::ifstream infile(filetxt);

			if (!infile) { std::cerr <<'\n' << "\033[1;4;31m [ERROR] (drawImage) .txt FILE NOT FOUND\033[0m\n"; std::exit(EXIT_FAILURE); }

			std::vector<std::vector<int>> array;
			std::string line;
			int xcount = 0;
			int ycount = 0;
			while (std::getline(infile, line)) {
				std::vector<int> row;
				std::istringstream iss(line);
				int value;
				while (iss >> value) {
					if (xcount % xlengthM == 0) {
						for (int i = 0; i < xlengthN; i++) {
							row.push_back(value);
						}
						xcount = 0;
					}
					xcount++;
				}
				if (ycount % ylengthM == 0) {
					for (int i = 0; i < ylengthN; i++) {
						array.push_back(row);
					}
					ycount = 0;
				}
				ycount++;
			}
			for (int i = 0; i < wally; i++) {
				if (y + i < wally && y + i >= 0) {
					for (int j = 0; j < wallx; j++) {
						if (x + j < wallx && i < array.size() && j < array[0].size() && x + j >= 0) {
							grid[(int)(x + j)][(int)(y + i)] = array[i][j];
						}
					}
				}
				else if (y + i >= wally) { break; }
			}
		}

		void storeImage(std::string filetxt, int xlengthN, int xlengthM, int ylengthN, int ylengthM) {
			if (xlengthM <= 0) { std::cerr << '\n' << "\033[1;4;31m [ERROR] (storeImage) INVAID XLENGTH DIVISOR:\033[0m"<< xlengthM<<'\n'; std::exit(EXIT_FAILURE); }
			if (ylengthM <= 0) { std::cerr << '\n' << "\033[1;4;31m [ERROR] (storeImage) INVAID YLENGTH DIVISOR:\033[0m"<< ylengthM<<'\n'; std::exit(EXIT_FAILURE); }

			std::string inputFileType = filetxt.substr(filetxt.find('.'));

			if (inputFileType != ".txt") { std::cerr << '\n' << "\033[1;4;31m [ERROR] (storeImage) INVALID INPUT FILETYPE - PLEASE INPUT .txt FILE \033[0m\n"; std::exit(EXIT_FAILURE); }

			std::ifstream infile(filetxt);

			if (!infile) { std::cerr << '\n' << "\033[1;4;31m[ERROR] (storeImage) .txt FILE NOT FOUND\033[0m\n"; std::exit(EXIT_FAILURE); }

			std::vector<std::vector<int>> array;
			std::string line;
			int xcount = 0;
			int ycount = 0;
			while (std::getline(infile, line)) {
				std::vector<int> row;
				std::istringstream iss(line);
				int value;
				while (iss >> value) {
					if (xcount % xlengthM == 0) {
						for (int i = 0; i < xlengthN; i++) {
							row.push_back(value);
						}
						xcount = 0;
					}
					xcount++;
				}
				if (ycount % ylengthM == 0) {
					for (int i = 0; i < ylengthN; i++) {
						array.push_back(row);
					}
					ycount = 0;
				}
				ycount++;
			}
			storedImages.push_back(array);
		}

		void drawStoredImage(int id, double x, double y) {

			if (id>=storedImages.size()) { std::cerr << '\n' << "\033[1;4;31m [ERROR] (drawStoredImage) ATTEMPT TO ACCESS STORED IMAGE OUT OF BOUNDS OF STOREDIMAGES VECTOR - INVALID ID NUMBER\033[0m\n"; std::exit(EXIT_FAILURE); }

			std::vector<std::vector<int>> array;
			for (int i = 0; i < storedImages[id].size(); i++) {
				std::vector<int> row;
				for (int j = 0; j < storedImages[id][0].size(); j++) {
					row.push_back(storedImages[id][i][j]);
				}
				array.push_back(row);
			}
			for (int i = 0; i < wally; i++) {
				if (y + i < wally&&y+i>=0) {
					for (int j = 0; j < wallx; j++) {
						if (x + j < wallx && i < array.size() && j < array[0].size() && x + j >= 0) {
							grid[(int)(x + j)][(int)(y + i)] = array[i][j];
						}
					}
				}
				else if(y+i>=wally) { break; }
			}
		}
	};
}
#endif
