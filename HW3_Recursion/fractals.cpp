/*
 * File: fractals.cpp
 * --------------------------
 * Name: Jian Zhong
 * This file contains fractal problems for CS106B.
 */


#include "fractals.h"
#include <cmath>
#include "gbufferedimage.h"

using namespace std;

const int LEAF_COLOR = 0x2e8b57;   /* Color of all leaves of recursive tree (level 1) */
const int BRANCH_COLOR = 0x8b7765; /* Color of all branches of recursive tree (level >=2) */

/**
 ******************************** HW 3.1 Sierpinski **************************************
 * Draws a Sierpinski triangle of the specified size and order, placing its
 * bottom-left corner at position (x, y).
 *
 * This will be called by fractalgui.cpp.
 *
 * @param gw - The window in which to draw the Sierpinski triangle.
 * @param x - The x coordinate of the bottom-left corner of the triangle.
 * @param y - The y coordinate of the bottom-left corner of the triangle.
 * @param size - The length of one side of the triangle.
 * @param order - The order of the fractal.
 *  **************************************************************************************
 */
void drawATriangle(GWindow& gw, double x, double y, double size) {
    gw.drawLine(x, y, x + size, y);
    gw.drawLine(x + size, y, x + (size/2), y + (sqrt(3)/2) * size);
    gw.drawLine(x + (size/2), y + (sqrt(3)/2) * size, x, y);
}

void drawSierpinskiTriangle(GWindow& gw, double x, double y, double size, int order) {
    if (x < 0 || y < 0 || size < 0 || order < 0) { //causes error
        throw("invalid input");
        return;
    }

    if (order == 1) {
        drawATriangle(gw, x, y, size); // draw a triangle
    } else {
        drawSierpinskiTriangle(gw, x,          y,                      size/2, order-1);
        drawSierpinskiTriangle(gw, x+(size/2), y,                      size/2, order-1);
        drawSierpinskiTriangle(gw, x+(size/4), y+(sqrt(3)/2)*(size)/2, size/2, order-1);
    }
}


/**
 ******************************** HW 3.2 Recurisve Tree **********************************
 * Draws a recursive tree fractal image of the specified size and order,
 * placing the bounding box's top-left corner at position (x,y).
 *
 * This will be called by fractalgui.cpp.
 *
 * @param gw - The window in which to draw the recursive tree.
 * @param x - The x coordinate of the top-left corner of the "bounding box". // 800
 * @param y - The y coordinate of the top-left corner of the "bounding box". // 650
 * @param size - The length of one side of the bounding box. // 650
 * @param order - The order of the fractal.
 * **************************************************************************************
 */
void drawTreeHelper(GWindow& gw, double x, double y, double size, int order, int angle) {
    if (order < 0) {
        error("*** Invalid input: order should be >= 0. ***");
    } else if (order != 0) {

        // Set color
        if (order == 1) {         // the outter most area
            gw.setColor(LEAF_COLOR);
        } else {                  // NOT outter most area
            gw.setColor(BRANCH_COLOR);
        }

        // Draw tree trunk and get the trunk's tip
        GPoint tip = gw.drawPolarLine(x, y, size, angle);

        // Recursive Case: while NOT leaf, draw branches by recursion:
        int j = 45;
        for (int i = 0; i < 7; i++) {
            drawTreeHelper(gw, tip.getX(), tip.getY(), size/2, order-1, angle-90+j);  // angle-90+j: 15, 30, 45, 60, 75, 90, 105
            j += 15;
        }
    }
}

void drawTree(GWindow& gw, double x, double y, double size, int order) {
    drawTreeHelper(gw, x + size/2, y + size, size / 2, order, 90);
}

/**
 ******************************** HW 3.3 Mandelbrot Set **********************************
 * Draws a Mandelbrot Set in the graphical window give, with maxIterations
 * (size in GUI) and in a given color (zero for palette)
 *
 * This will be called by fractalgui.cpp.
 *
 * @param gw - The window in which to draw the Mandelbrot set.
 * @param minX - left-most column of grid
 * @param incX - increment value of columns of grid
 * @param minY - top-most row of grid
 * @param incY - increment value of rows of grid
 * @param maxIterations - The maximum number of iterations to run recursive step
 * @param color - The color of the fractal; zero if palette is to be used
 * **************************************************************************************
 */
void mandelbrotSet(GWindow& gw, double minX, double incX,
                   double minY, double incY, int maxIter, int color) {

    color = 0;
    Vector<int> palette = setPalette();            // setup the palette
    int width = gw.getCanvasWidth();               // Canvas is the whole white window
    int height = gw.getCanvasHeight();
    GBufferedImage image(width, height, 0xffffff); // create an image of black color
    gw.add(&image);                                // add image into (leftX, topY)
    Grid<int> pixels = image.toGrid();             // Convert the entire image to pixels in Gird collection

    // traverse all pixels in the canvas
    for (int c = 0; c < pixels.numCols(); c++) {
        for (int r = 0; r < pixels.numRows(); r++) {

            // Draw pixel in M set
            int numIterations = mandelbrotSetIterations(Complex(minX + c*incX, minY + r*incY), maxIter);
            if (color != 0) { // if color is non-zero, set as int color.
                if (numIterations == maxIter) {
                    pixels[r][c] = color;  // C is in M set, draw the pixel
                }
            } else {          // else color is zero, use the palette of colors for all pixels
                pixels[r][c] = palette[numIterations % palette.size()];
            }

        } // end inner for-loop
    } // end outer for-loop

    image.fromGrid(pixels); // Converts and puts the grid back into the image onscreen
}

int mandelbrotSetIterations(Complex Z, Complex C, int maxIterations, int numIterations) {

    // if exhaust max iteration    =>  in M set
    // if absolute value of Z > 4  =>  Not in M set
    if (Z.abs() > 4 || numIterations == maxIterations) {
        return numIterations;
    } else {
        return mandelbrotSetIterations(Z * Z + C, C, maxIterations, numIterations + 1);
    }
}

int mandelbrotSetIterations(Complex cpx, int maxIterations) {
    // first Z starts is 0,
    // first iteration is 0.
    return mandelbrotSetIterations(Complex (0, 0), cpx, maxIterations, 0);
}

// Helper function to set the palette
Vector<int> setPalette() {
    Vector<int> colors;

    // Feel free to replace with any palette.
    // You can find palettes at:
    // http://www.colourlovers.com/palettes

    // Example palettes:
    // http://www.colourlovers.com/palette/4480793/in_the_middle
    // string colorSt = "#A0B965,#908F84,#BF3C43,#9D8E70,#C9BE91,#A0B965,#908F84,#BF3C43";

    // http://www.colourlovers.com/palette/4480786/Classy_Glass
    // string colorSt = "#9AB0E9,#C47624,#25269A,#B72202,#00002E,#9AB0E9,#C47624,#25269A";

    // The following is the "Hope" palette:
    // http://www.colourlovers.com/palette/524048/Hope
    string colorSt =  "#04182B,#5A8C8C,#F2D99D,#738585,#AB1111,#04182B,#5A8C8C,#F2D99D";
    Vector<string>colorsStrVec = stringSplit(colorSt,",");
    for (string color : colorsStrVec) {
        colors.add(convertColorToRGB(trim(color)));
    }
    return colors;
}
