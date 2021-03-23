#include "canvas.h"
#include <cassert>
#include <cmath>
#include <algorithm>

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
		barycentricFill(vertices[0], vertices[1], vertices[2], vertices[3], vertices[4], vertices[5]);
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

void agl::canvas::barycentricFill(int aColumn, int aRow, int bColumn, int bRow, int cColumn, int cRow)
{
	vector<int> maxCordinates = getBoundingBoxCordinates(aColumn, aRow, bColumn, bRow, cColumn, cRow);
	int alpha, beta, gamma;
	int xmin = maxCordinates[0];
	int ymin = maxCordinates[1];
	int xmax = maxCordinates[2];
	int ymax = maxCordinates[3];
	int fab_c = f_ab(aColumn, aRow, bColumn, bRow, cColumn, cRow);  //gamma
	int fac_b = f_ac(aColumn, aRow, cColumn, cRow, bColumn, bRow);  //beta
	int fbc_a = f_bc(bColumn, bRow, cColumn, cRow, aColumn, aRow);  //alpha
	ppm_pixel aColor = _canvas.get(aRow, aColumn);
	ppm_pixel bColor = _canvas.get(bRow, bColumn);
	ppm_pixel cColor = _canvas.get(cRow, cColumn);
	ppm_pixel temporary;

	for (int row = ymin; row <= ymax; row++) 
	{
		for (int column = xmin; column <= xmax; column++) 
		{
			alpha = f_bc(bColumn, bRow, cColumn, cRow, column, row) / fbc_a;
			beta = f_ac(aColumn, aRow, cColumn, cRow, column, row) / fac_b;
			gamma = f_ab(aColumn, aRow, bColumn, bRow, column, row) / fab_c;
			if (alpha >= 0 && beta >= 0 && gamma >= 0) 
			{
				bool drawAlpha = ((alpha > 0) || (fbc_a * f_bc(bColumn, bRow, cColumn, cRow, -1, -1)) > 0);
				bool drawBeta = ((beta > 0) || (fac_b * f_ac(aColumn, aRow, cColumn, cRow, -1, -1)) > 0);
				bool drawGamma= ((gamma > 0) || (fab_c * f_ab(aColumn, aRow, bColumn, bRow, -1, -1)) > 0);
				if (drawAlpha && drawBeta && drawGamma)
				{
					temporary.r = alpha * aColor.r;
					temporary.g = beta * aColor.g;
					temporary.b = gamma * aColor.b;
					_canvas.set(row, column, temporary);
				}	
			}
		}
	}
}



float agl::canvas::f_ab(int aColumn, int aRow, int bColumn, int bRow, int x, int y)
{
	return (((float)bRow - aRow) * (x - aColumn) - ((float)bColumn - aColumn) * (y - aRow));
}

float agl::canvas::f_ac(int aColumn, int aRow, int cColumn, int cRow, int x, int y)
{
	return (((float)cRow - aRow) * (x - aColumn) - ((float)cColumn - aColumn) * (y - aRow));
}

float agl::canvas::f_bc(int bColumn, int bRow, int cColumn, int cRow, int x, int y)
{
	return (((float)cRow - bRow) * (x - bColumn) - ((float)cColumn - bColumn) * (y - bRow));
}

vector<int> agl::canvas::getBoundingBoxCordinates(int aColumn, int aRow, int bColumn, int bRow, int cColumn, int cRow)
{
	vector<int> columns = { aColumn, bColumn, cColumn };
	vector<int> rows = { aRow, bRow, cRow };
	vector<int> results;
	int xmin = columns[0];
	int ymin = rows[0];
	int xmax = columns[0];
	int ymax = rows[0];
	for (int i = 1; i < columns.size(); i++) 
	{
		if (xmin > columns[i])
		{
			xmin = columns[i];
		}
		if(xmax < columns[i])
		{
			xmax = columns[i];
		}
	}
	for (int i = 1; i < rows.size(); i++)
	{
		if (ymin > rows[i])
		{
			ymin = rows[i];
		}
		if (ymax < rows[i])
		{
			ymax = rows[i];
		}
	}
	results.push_back(xmin);
	results.push_back(ymin);
	results.push_back(xmax);
	results.push_back(ymax);
	return results;
}


void canvas::drawLine(int aColumn, int aRow, int bColumn, int bRow) 
{
	float y = aRow;  //y1 is used for the case which ax < bx
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
			for (float i = aColumn; i <= bColumn; i++) 
			{
				interpolationTValue = (i - aColumn) / (bColumn - aColumn);
				ppm_pixel temporary = { aColor.r * (1 - interpolationTValue) + bColor.r * interpolationTValue, 
										aColor.g * (1 - interpolationTValue) + bColor.g * interpolationTValue, 
										aColor.b * (1 - interpolationTValue) + bColor.b * interpolationTValue};
				_canvas.set(y, i, temporary);
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
			for (float i = aColumn; i >= bColumn; i--)
			{
				interpolationTValue = (i - aColumn) / (bColumn - aColumn);
				ppm_pixel temporary = { aColor.r * (1 - interpolationTValue) + bColor.r * interpolationTValue,
										aColor.g * (1 - interpolationTValue) + bColor.g * interpolationTValue,
										aColor.b * (1 - interpolationTValue) + bColor.b * interpolationTValue };
				_canvas.set(y, i, temporary);
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
			for (float i = aRow; i <= bRow; i++)
			{	
				if(aColumn == bColumn)
					interpolationTValue = (i - aRow) / (bRow - aRow);
				interpolationTValue = (i - aColumn) / (bColumn - aColumn);
				ppm_pixel temporary = { aColor.r * (1 - interpolationTValue) + bColor.r * interpolationTValue,
										aColor.g * (1 - interpolationTValue) + bColor.g * interpolationTValue,
										aColor.b * (1 - interpolationTValue) + bColor.b * interpolationTValue };
				_canvas.set(i, x, temporary);
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
			for (float i = aRow; i >= bRow; i--)
			{
				if (aColumn == bColumn)
					interpolationTValue = (i - aRow) / (bRow - aRow );
				interpolationTValue = (i - aColumn) / (bColumn - aColumn);
				ppm_pixel temporary = { aColor.r * (1 - interpolationTValue) + bColor.r * interpolationTValue,
										aColor.g * (1 - interpolationTValue) + bColor.g * interpolationTValue,
										aColor.b * (1 - interpolationTValue) + bColor.b * interpolationTValue };
				_canvas.set(i, x, temporary);
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





