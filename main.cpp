#include <iostream>
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "lsb.h"
#include "aco.h"

using namespace std;
using namespace cv;

struct resultCompare{
    double mse;
    double psnr;
};

resultCompare compareResult(Mat imgA,Mat imgB,int max);
double psnr(double mse,int max);

int main( int argc, char** argv ){

  int numBit = 4;
  // do {
  //     std::cout << "Number of Bit(s) for LSB insertion :  ";
  // } while((cin>>numBit) == 0);

  //Get image.
  Mat imgCover = imread(argv[1],1);
  Mat imgMsg = imread(argv[2],1);
  Mat imgGCover;
  Mat imgGMsg;

  //Get GreyScale
  cvtColor(imgCover, imgGCover, CV_BGR2GRAY );
  cvtColor(imgMsg, imgGMsg, CV_BGR2GRAY );

  //LSB object.
  LSB lsb1(imgGCover,imgGMsg,numBit);
  Mat imgSteg1 = lsb1.insertion(false);
  Mat imgRe1 = lsb1.extraction(false);

  //LSB object.
  LSB lsb2(imgGCover,imgGMsg,numBit,false);
  lsb2.ToralAutomorphisms();
  lsb2.comparePixelPosition();
  lsb2.changePosition();

  Mat imgSteg2 = lsb2.insertion(true);
  Mat imgRe2 = lsb2.extraction(true);

  //LSB object.
  LSB lsb3(imgGCover,imgGMsg,numBit,true);
  lsb3.ToralAutomorphisms();
  lsb3.comparePixelPosition();

  if (lsb2.bestPixel4BitsMSE < lsb3.bestPixel4BitsMSE) {
    lsb3.bestPixel4BitsMSE = lsb2.bestPixel4BitsMSE;
    lsb3.bestPosPixel4Bits = lsb2.bestPosPixel4Bits;
  }
  if (lsb2.bestPixel2BitsMSE < lsb3.bestPixel2BitsMSE) {
    lsb3.bestPixel2BitsMSE = lsb2.bestPixel2BitsMSE;
    lsb3.bestPosPixel2Bits = lsb2.bestPosPixel2Bits;
  }

  lsb3.compareBitPosition();
  lsb3.changePosition();

  Mat imgSteg3 = lsb3.insertion(true);
  Mat imgRe3 = lsb3.extraction(true);

  resultCompare r1 = compareResult(imgGCover,imgSteg1,255);
  resultCompare r2 = compareResult(imgGCover,imgSteg2,255);
  resultCompare r3 = compareResult(imgGCover,imgSteg3,255);

  //Print Result.
  std::cout << "Compare LSB insertion." << std::endl;
  std::cout << "MSE : "<< r1.mse << std::endl;
  std::cout << "PSNR : "<< r1.psnr << std::endl;

  std::cout << "Compare LSB insertion with comparetive Pixel Position." << std::endl;
  std::cout << "MSE : "<< r2.mse << std::endl;
  std::cout << "PSNR : "<< r2.psnr << std::endl;

  std::cout << "Compare LSB insertion with comparetive Bit Position." << std::endl;
  std::cout << "MSE : "<< r3.mse << std::endl;
  std::cout << "PSNR : "<< r3.psnr << std::endl;

  //Show Cover Image, Stego-Image and Secret Image.
  namedWindow("Image", CV_WINDOW_AUTOSIZE);
  namedWindow("ImageSteg1", CV_WINDOW_AUTOSIZE);
  namedWindow("ImageRe1", CV_WINDOW_AUTOSIZE);
  namedWindow("ImageSteg2", CV_WINDOW_AUTOSIZE);
  namedWindow("ImageRe2", CV_WINDOW_AUTOSIZE);
  namedWindow("ImageSteg3", CV_WINDOW_AUTOSIZE);
  namedWindow("ImageRe3", CV_WINDOW_AUTOSIZE);

  imshow("Image",imgGCover);
  imshow("ImageSteg1",imgSteg1);
  imshow("ImageRe1",imgRe1);
  imshow("ImageSteg2",imgSteg2);
  imshow("ImageRe2",imgRe2);
  imshow("ImageSteg3",imgSteg3);
  imshow("ImageRe3",imgRe3);

  waitKey();

  cvDestroyWindow("Image");
  cvDestroyWindow("ImageSteg1");
  cvDestroyWindow("ImageRe1");
  cvDestroyWindow("ImageSteg2");
  cvDestroyWindow("ImageRe2");
  cvDestroyWindow("ImageSteg3");
  cvDestroyWindow("ImageRe3");

  return 0;
}
////////////////////
//Compare Function./
////////////////////
resultCompare compareResult(Mat imgA,Mat imgB,int max){

  //Calculate MSE.
  double sum = 0;
  int l = imgA.rows * imgA.cols;

  for (size_t y = 0; y < imgA.rows; y++) {
    for (size_t x = 0; x < imgA.cols; x++) {

        sum += pow(imgA.at<uchar>(y, x) - imgB.at<uchar>(y, x),2);

    }//End of x index For Loop.
  }//End of y index For Loop.

  double mse = sum / (double)l;

  resultCompare c;
  c.mse = mse;
  c.psnr = psnr(mse,max);

  return c ;
}//End of compareResult Function.

///////////////////
//PSNR Function.//
/////////////////
double psnr(double mse,int max){
  return 10 * log10((max*max)/mse);
}
