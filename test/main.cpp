/**
   Run some tests. Results need to be verified visually at the moment...

   For each file in the test/data directory, it will:
      - read it into a QImage
      - convert it to a cv::Mat using ASM::QImageToCvMat()
      - write out that Mat using OpenCV to <name>_cvMat.png
      - split the Mat into its 3 or 4 channels
      - write each channel to its own file
         <name>_red.png, <name>_green.png, <name>_blue.png, and (if exists) <name>_alpha.png
      - convert Mat back to QImage using ASM::cvMatToQImage()
      - write it out to <name>_converted.<extesion>

   To check the ASM::QImageToCvMat() conversion, you can look at the <name>_cvMat.png file
   and each of the channels for the image.

   To check the ASM::cvMatToQImage(), look at the <name>_converted.<extension> file and verify
   that it is the same as the <name>_cvMat.png and original input file.

   This could be made a lot fancier/more complete, but for now it's enough to check the 2 main
   conversions present in the asmOpenCV.h file.

   If you want to test one of your image files, drop it in the data directory and rerun.

   Andy Maloney <asmaloney@gmail.com>
   https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap
**/

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QMap>
#include <QVector>

#include "asmOpenCV.h"
#include "opencv2/imgcodecs/imgcodecs.hpp"

static void  sRunTests( const QString &inInputFile, const QString &inOutputFile );

// Simple map of enum value to string since C++ STILL doesn't provide this...
static QMap<QImage::Format, QString>   sImageFormatEnumMap = {
   { QImage::Format::Format_Invalid, "Format_Invalid" },
   { QImage::Format::Format_Mono, "Format_Mono" },
   { QImage::Format::Format_MonoLSB, "Format_MonoLSB" },
   { QImage::Format::Format_Indexed8, "Format_Indexed8" },
   { QImage::Format::Format_RGB32, "Format_RGB32" },
   { QImage::Format::Format_ARGB32, "Format_ARGB32" },
   { QImage::Format::Format_ARGB32_Premultiplied, "Format_ARGB32_Premultiplied" },
   { QImage::Format::Format_RGB16, "Format_RGB16" },
   { QImage::Format::Format_ARGB8565_Premultiplied, "Format_ARGB8565_Premultiplied" },
   { QImage::Format::Format_RGB666, "Format_RGB666" },
   { QImage::Format::Format_ARGB6666_Premultiplied, "Format_ARGB6666_Premultiplied" },
   { QImage::Format::Format_RGB555, "Format_RGB555" },
   { QImage::Format::Format_ARGB8555_Premultiplied, "Format_ARGB8555_Premultiplied" },
   { QImage::Format::Format_RGB888, "Format_RGB888" },
   { QImage::Format::Format_RGB444, "Format_RGB444" },
   { QImage::Format::Format_ARGB4444_Premultiplied, "Format_ARGB4444_Premultiplied" },
   { QImage::Format::Format_RGBX8888, "Format_RGBX8888" },
   { QImage::Format::Format_RGBA8888, "Format_RGBA8888" },
   { QImage::Format::Format_RGBA8888_Premultiplied, "Format_RGBA8888_Premultiplied" },
   { QImage::Format::Format_BGR30, "Format_BGR30" },
   { QImage::Format::Format_A2BGR30_Premultiplied, "Format_A2BGR30_Premultiplied" },
   { QImage::Format::Format_RGB30, "Format_RGB30" },
   { QImage::Format::Format_A2RGB30_Premultiplied, "Format_A2RGB30_Premultiplied" },
   { QImage::Format::Format_Alpha8, "Format_Alpha8" },
   { QImage::Format::Format_Grayscale8, "Format_Grayscale8" }
};

int  main( int argc, char **argv )
{
   QCoreApplication   app( argc, argv );

   const QString  cInputDir = TEST_INPUT_DIR;
   const QString  cOutputDir( "./output" );

   qInfo() << "   input dir:" << cInputDir;
   qInfo() << "  output dir:" << cOutputDir;

   QDir  outputDir( cOutputDir );

   outputDir.removeRecursively();

   QDir().mkpath( cOutputDir );

   const QFileInfoList  cInfo = QDir( cInputDir ).entryInfoList( QDir::Files |
                                                                QDir::Readable |
                                                                QDir::NoDotAndDotDot |
                                                                QDir::NoSymLinks );

   for ( const QFileInfo &fileInfo : cInfo )
   {
      qInfo() << "====";
      qInfo() << "Processing:" << fileInfo.fileName();

      sRunTests( fileInfo.filePath(), cOutputDir );
   }

   return 0;
}

static QString sQImageFormatToStr( QImage::Format inFormat )
{
   return sImageFormatEnumMap[inFormat];
}

static QString sCVTypeToStr( int inType )
{
   QString str( "CV_" );

   switch ( CV_MAT_DEPTH( inType ) )
   {
      case CV_8U:    str += "8U"; break;
      case CV_8S:    str += "8S"; break;
      case CV_16U:   str += "16U"; break;
      case CV_16S:   str += "16S"; break;
      case CV_32S:   str += "32S"; break;
      case CV_32F:   str += "32F"; break;
      case CV_64F:   str += "64F"; break;
      default:       str += "User"; break;
   }

   str += QStringLiteral( "C%1" ).arg( QString::number( CV_MAT_CN( inType ) ) );

   return str;
}

// Given a CV_8UC3 or CV_8UC4 cv::Mat, split the channels and write them to individual files
void  sSplitAndWriteChannels( const cv::Mat &inMat, const QString &inOutputDir, const QString &inBaseFileName )
{
   const int   cNumChannels = inMat.channels();
   const int   cOutputFormat = inMat.type();

   if ( (inMat.type() != CV_8UC3) && (inMat.type() != CV_8UC4) )
   {
      qWarning() << " Cannot split this cv::Mat - type" << sCVTypeToStr( inMat.type() );
      return;
   }

   QVector<cv::Mat>  mats( cNumChannels );

   cv::split( inMat, mats.data() );

   cv::Mat emptyImage = cv::Mat::zeros( inMat.rows, inMat.cols, CV_8UC1 );
   cv::Mat blueMat( inMat.rows, inMat.cols, cOutputFormat );
   cv::Mat greenMat( inMat.rows, inMat.cols, cOutputFormat );
   cv::Mat redMat( inMat.rows, inMat.cols, cOutputFormat );

   QVector<int> fromTo = { 0,0, 1,1, 2,2 };

   if ( cNumChannels == 4 )
   {
      fromTo += { 3, 3 };
   }

   // blue
   QVector<cv::Mat> inBlue = { mats[0], emptyImage, emptyImage };

   if ( cNumChannels == 4 )
   {
      inBlue += mats[3];
   }

   cv::mixChannels( inBlue.data(), cNumChannels, &blueMat, 1, fromTo.data(), cNumChannels );

   // green
   QVector<cv::Mat> inGreen = { emptyImage, mats[1], emptyImage };

   if ( cNumChannels == 4 )
   {
      inGreen += mats[3];
   }

   cv::mixChannels( inGreen.data(), cNumChannels, &greenMat, 1, fromTo.data(), cNumChannels );

   // red
   QVector<cv::Mat> inRed = { emptyImage, emptyImage, mats[2] };

   if ( cNumChannels == 4 )
   {
      inRed += mats[3];
   }

   cv::mixChannels( inRed.data(), cNumChannels, &redMat, 1, fromTo.data(), cNumChannels );

   cv::imwrite( QStringLiteral( "%1/%2_blue.png" ).arg( inOutputDir ).arg( inBaseFileName ).toLocal8Bit().constData(),
                blueMat );

   cv::imwrite( QStringLiteral( "%1/%2_green.png" ).arg( inOutputDir ).arg( inBaseFileName ).toLocal8Bit().constData(),
                greenMat );

   cv::imwrite( QStringLiteral( "%1/%2_red.png" ).arg( inOutputDir ).arg( inBaseFileName ).toLocal8Bit().constData(),
                redMat );

   if ( cNumChannels == 4 )
   {
      cv::imwrite( QStringLiteral( "%1/%2_alpha.png" ).arg( inOutputDir ).arg( inBaseFileName ).toLocal8Bit().constData(),
                   mats[3] );
   }
}

void  sRunTests( const QString &inInputFile, const QString &inOutputDir )
{
   QImageReader   reader( inInputFile );

   if ( !reader.canRead() )
   {
      qWarning() << " Could not read image" << inInputFile;
      return;
   }

   QImage   image = reader.read();

   if ( image == QImage() )
   {
      qWarning() << " Error reading image" << inInputFile;
      return;
   }

   const QFileInfo   fileInfo( inInputFile );

   const QString  cBaseFileName = fileInfo.baseName();
   const QString  cExtension = fileInfo.suffix();

   qInfo().noquote() << " Input image format:" << sQImageFormatToStr( image.format() );

   // Convert QImage to a cvMat
   cv::Mat  cvMat = ASM::QImageToCvMat( image, false );

   qInfo().noquote() << " cv::Mat format:" << sCVTypeToStr( cvMat.type() );

   cv::imwrite( QStringLiteral( "%1/%2_cvMat.png" ).arg( inOutputDir ).arg( cBaseFileName ).toLocal8Bit().constData(), cvMat );

   sSplitAndWriteChannels( cvMat, inOutputDir, cBaseFileName );

   // Convert back to QImage and write it out
   QImage converted = ASM::cvMatToQImage( cvMat );

   qInfo().noquote() << " Converted image format:" << sQImageFormatToStr( converted.format() );

   QImageWriter   writer( QStringLiteral( "%1/%2_converted.%3" ).arg( inOutputDir ).arg( cBaseFileName ).arg( cExtension ), cExtension.toLocal8Bit() );

   writer.write( converted );
}
