#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

class LSB{


  string S;
  string S4Bits;//Final Changed Position.
  string S2Bits;//Final Changed Position.
  string Sp4Bits;//Changed Pixel Position.
  string Sp2Bits;//Changed Pixel Position.
  string Sb4Bits;//Changed Bit Position.
  string Sb2Bits;//Changed Bit Position.

  string R;//Retrieved bit string of Message.
  string R4Bits;
  string R2Bits;
  string Rp;//Retrieved bit string of Message
            //that is changed Pixel Position.
  string Rp4Bits;
  string Rp2Bits;
  string Rb;//Retrieved bit string of Message
           //that is changed Bit Position.
  string Rb4Bits;
  string Rb2Bits;

  bool CBP;//Flag of used changeBitPosition.

  Mat imgC;
  Mat imgM;
  Mat imgS;
  Mat imgR;

  int numBit;//Number of Bits to Substitution.
  int numPos;//Number of Position.
  int numPossiPos;//Number of Possible Position.
  int numSec;

  int k0,k1;

  //double bestMSEb;

  struct pos{
    // string posPixel;//Possible Position.
    string posBit;
    double mse;
    double psnr;
    int n; //Number of member.
    // vector<pos> c;//child node for bit position.

  };

  vector<pos> collectorBitPosition;


public:

  string C;//Cover Image's data in bit string.
  string C4Bits;//Cover Image's data in bit string.
  string C2Bits;//Cover Image's data in bit string.
  string M;//Message Image's data in bit string.
  string M4Bits;//Message Image's data in bit string.
  string M2Bits;//Message Image's data in bit string.


  vector<int> bestPosPixel4Bits;
  vector<int> bestPosPixel2Bits;
  vector<int> bestPosBit4Bits;
  vector<int> bestPosBit2Bits;
  double bestPixel4BitsMSE;
  double bestPixel2BitsMSE;
  double bestBit4BitsMSE;
  double bestBit2BitsMSE;

  LSB(Mat&,Mat&,int);
  LSB(Mat&,Mat&,int,bool);

  ~LSB();

  void ToralAutomorphisms();
  void comparePosition();
  void comparePixelPosition();
  void compareBitPosition();
  void changePosition();

  Mat insertion(bool);
  Mat extraction(bool);

private:

  void init();

  void create2BitPosition();
  void create3BitPosition();
  void create4BitPosition();

  int Factorial(int x);
  int byteToDec(string byte);
};//class LSB.
