/**
   Simple example of applying Gaussian blur using OpenCV.
   Converts between OpenCV's cv::Mat and Qt's QImage using utility functions.

   Andy Maloney <asmaloney@gmail.com>
   https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap
**/

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>

#include "asmOpenCV.h"

static void  sImageBlur( const QString &inInputFile, const QString &inOutputFile );


int  main( int argc, char **argv )
{
   QCoreApplication   app( argc, argv );

   QCommandLineParser parser;

   parser.setApplicationDescription( "Apply gaussian blur to a given image." );
   parser.addHelpOption();

   parser.addOptions({
                        { "i", "Image file", "file", "./exampleImage.png" },
                        { "o", "Output file", "file", "./blurred.png" },
                     });

   parser.process( app );

   const QString  cInputFile = parser.value( "i" );
   const QString  cOutputFile = parser.value( "o" );

   qInfo() << "   input:" << cInputFile;
   qInfo() << "  output:" << cOutputFile;

   sImageBlur( cInputFile, cOutputFile );

   return 0;
}

void  sImageBlur( const QString &inInputFile, const QString &inOutputFile )
{
   QImageReader   reader( inInputFile );

   if ( !reader.canRead() )
   {
      qWarning() << "Could not read image" << inInputFile;
      return;
   }

   QImage   image = reader.read();

   if ( image == QImage() )
   {
      qWarning() << "Error reading image" << inInputFile;
      return;
   }

   qInfo() << "Input image format:" << image.format();

   // Convert QImage to a cvMat
   cv::Mat  cvMat = ASM::QImageToCvMat( image, false );

   // ... do any required OpenCV processing
   cv::GaussianBlur( cvMat, cvMat, cv::Size( 11, 11 ), 0, 0, cv::BORDER_DEFAULT );

   // Convert back to QImage so we can write it out
   QImage   blurred = ASM::cvMatToQImage( cvMat );

   QImageWriter   writer( inOutputFile, "png" );

   writer.write( blurred );
}
