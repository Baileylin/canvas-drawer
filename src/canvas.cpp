#include "canvas.h"
#include <cassert>
#include <cmath>

using namespace std;
using namespace agl;

canvas::canvas(int w, int h) : _canvas(w, h)
{
}

canvas::~canvas()
{
}

void canvas::save(const std::string& filename)
{
   _canvas.save(filename);
}

void canvas::begin(PrimitiveType type)
{
	shape = type;
}

void canvas::end()
{
	int index = 0;
	if(shape == LINES) 
	{
		drawLine(vertices[index], vertices[index+1], vertices[index+2], vertices[index+3]);
	}
	else if (shape == TRIANGLES)
	{
		drawLine(vertices[index], vertices[index + 1], vertices[index + 2], vertices[index + 3]);
		drawLine(vertices[index+2], vertices[index + 3], vertices[index + 4], vertices[index + 5]);
		drawLine(vertices[index+4], vertices[index + 5], vertices[index], vertices[index + 1]);
	}
	vertices.clear();
}

void canvas::vertex(int x, int y)
{
	if (x > (_canvas.width() - 1))
		x = _canvas.width() - 1;
	if (x < 0)
		x = 0;
	if (y > (_canvas.height() - 1))
		y = _canvas.height() - 1;
	if (y < 0)
		y = 0;
	vertices.push_back(x);
	vertices.push_back(y);
	_canvas.set(y, x, { pixelColor });
}

void canvas::color(unsigned char r, unsigned char g, unsigned char b)
{
	pixelColor = { r, g, b };
}

void canvas::background(unsigned char r, unsigned char g, unsigned char b)
{
	for (int row = 0; row < _canvas.height(); row++)
	{
		for (int column = 0; column < _canvas.width(); column++)
		{
			_canvas.set(row, column, ppm_pixel{r, g, b});
		}
	}
}

void canvas::drawLine(int aColumn, int aRow, int bColumn, int bRow) 
{
	int y = aRow;  //y1 is used for the case which ax < bx
	float x = aColumn;
	int update = 1;
	int width = bColumn - aColumn;
	int height = bRow - aRow;
	int F1 = 2 * height - width;    //F1 is used for the case which width is larger than height
	int F2 = 2 * width - height;    //F2 is used for the case which width is smaller than height
	ppm_pixel aColor = _canvas.get(aRow, aColumn);
	ppm_pixel bColor = _canvas.get(bRow, bColumn);
	float interpolationTValue;
	
	//the following else statement is for the case which width > height
	if (abs(width) > abs(height))
	{
		//the following if statement is for the case which xRow < yRow
		if (aColumn < bColumn) 
		{
			if (height < 0) 
			{
				update = -1;
				height = -height;
			}
			for (int x = aColumn; x <= bColumn; x++) 
			{
				interpolationTValue = (x - aColumn) / (bColumn - aColumn);
				ppm_pixel temporary = { aColor.r * (1 - interpolationTValue) + bColor.r * interpolationTValue, 
										aColor.g * (1 - interpolationTValue) + bColor.g * interpolationTValue, 
										aColor.b * (1 - interpolationTValue) + bColor.b * interpolationTValue};
				_canvas.set(y, x, temporary);
				if (F1 > 0)
				{
					y = y + update;
					F1 += 2 * (height - width);
				}
				else 
				{
					F1 += 2 * height;
				}
			}
		}
		//swap x, y coordinates to meet the assumptions of Bresenham algorithm
		else 
		{
			if (height < 0)
			{
				update = -1;
				height = -height;
			}
			for (int x = aColumn; x >= bColumn; x--)
			{
				interpolationTValue = (x - aColumn) / (bColumn - aColumn);
				ppm_pixel temporary = { aColor.r * (1 - interpolationTValue) + bColor.r * interpolationTValue,
										aColor.g * (1 - interpolationTValue) + bColor.g * interpolationTValue,
										aColor.b * (1 - interpolationTValue) + bColor.b * interpolationTValue };
				_canvas.set(y, x, temporary);
				if (F1 > 0)
				{
					y=y+update;
					F1 += 2 * (height - width);
				}
				else
				{
					F1 += 2 * height;
				}
			}
		}
	}
	//the following else statement is for the case which width < height
	else 
	{
		if (aRow < bRow) 
		{
			if (width < 0)
			{
				update = -1;
				width = -width;
			}
			for (int y = aRow; y <= bRow; y++)
			{
				interpolationTValue = (x - aColumn) / (bColumn - aColumn);
				ppm_pixel temporary = { aColor.r * (1 - interpolationTValue) + bColor.r * interpolationTValue,
										aColor.g * (1 - interpolationTValue) + bColor.g * interpolationTValue,
										aColor.b * (1 - interpolationTValue) + bColor.b * interpolationTValue };
				_canvas.set(y, x, temporary);
				if (F2 > 0)
				{
					x = x + update;
					F2 += 2 * (width - height);
				}
				else
				{
					F2 += 2 * width;
				}
			}
		}
		else 
		{
			if (width < 0)
			{
				update = -1;
				width = -width;
			}
			for (int y = aRow; y >= bRow; y--)
			{
				interpolationTValue = (x - aColumn) / (bColumn - aColumn);
				ppm_pixel temporary = { aColor.r * (1 - interpolationTValue) + bColor.r * interpolationTValue,
										aColor.g * (1 - interpolationTValue) + bColor.g * interpolationTValue,
										aColor.b * (1 - interpolationTValue) + bColor.b * interpolationTValue };
				_canvas.set(y, x, temporary);
				if (F2 > 0)
				{
					x = x + update;
					F2 += 2 * (width - height);
				}
				else
				{
					F2 += 2 * width;
				}
			}
		}
	}
}

