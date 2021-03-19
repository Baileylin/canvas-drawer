#ifndef canvas_H_
#define canvas_H_

#include <string>
#include <vector>
#include "ppm_image.h"

namespace agl
{
   enum PrimitiveType {UNDEFINED, LINES, TRIANGLES, POINTS};
   class canvas
   {
   public:
      canvas(int w, int h);
      virtual ~canvas();

      // Save to file
      void save(const std::string& filename);

      // Draw primitives with a given type (either LINES or TRIANGLES)
      // For example, the following draws a red line followed by a green line
      // begin(LINES);
      //    color(255,0,0);
      //    vertex(0,0);
      //    vertex(100,0);
      //    color(0,255,0);
      //    vertex(0, 0);
      //    vertex(0,100);
      // end();
      void begin(PrimitiveType type);
      void end();

      // Specifiy a vertex at raster position (x,y)
      // x corresponds to the column; y to the row
      void vertex(int x, int y);

      // Specify a color. Color components are in range [0,255]
      void color(unsigned char r, unsigned char g, unsigned char b);

      // Fill the canvas with the given background color
      void background(unsigned char r, unsigned char g, unsigned char b);

      void drawLine(int aColumn, int aRow, int bColumn, int bRow);

      void barycentricFill(int aColumn, int aRow, int bColumn, int bRow, int cColumn, int cRow);

      std::vector<int> getBoundingBoxCordinates(int aColumn, int aRow, int bColumn, int bRow, int cColumn, int cRow);

      float f_ab(int aColumn, int aRow, int bColumn, int bRow, int x, int y);
      float f_ac(int aColumn, int aRow, int cColumn, int cRow, int x, int y);
      float f_bc(int bColumn, int bRow, int cColumn, int cRow, int x, int y);

   private:
      ppm_image _canvas;
      std::vector<int> vertices;
      ppm_pixel pixelColor;
      PrimitiveType shape;
   };
}

#endif


