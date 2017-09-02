#include <iostream>
#include <vector>
#include <cv.h>
#include "lsb.h"
#include "aco.h"

using namespace std;
using namespace cv;

LSB::LSB(Mat& imgCover,Mat& imgMsg,int r){

  std::cout << "[ LSB ] Create Object." << std::endl;
  numSec = 1;

  CBP = false;
  if(r==2 || r==3 || r==4){
    numBit = r;
    numPos = pow(2,numBit);
    numPossiPos = Factorial(pow(2,numBit));
  }else{
    numBit = 2;
    numPos = pow(2,numBit);
    numPossiPos = Factorial(pow(2,numBit));
  }

  imgC = imgCover;
  imgM = imgMsg;

  imgS = imgC.clone();

  std::cout << "- Get Bits String from Message image." << std::endl;
  //Get Bits String from Message image.
  for (size_t i = 0; i < imgM.rows; i++) {
    for (size_t j = 0; j < imgM.cols; j++) {

        string byte = bitset<8>(imgM.at<uchar>(i,j)).to_string();
        M +=byte;

    }//for j index.
  }//for i index.

  int bitMsgLength = M.size();

  std::cout << "- Get Bits String from Cover image." << std::endl;
  //Get Bits String from Cover image.
  for (size_t i = 0; i < imgC.rows; i++) {
    if(C.size() == bitMsgLength)break;
    for (size_t j = 0; j < imgC.cols; j++) {
      if(C.size() == bitMsgLength)break;
        string byte = bitset<8>(imgC.at<uchar>(i,j)).to_string();
        //int x = imgCover.at<uchar>(i,j);
        //std::cout << byte <<" : "<< x <<" : "<< byte.substr(8-numBit,numBit)<< std::endl;
        C +=byte.substr(8-numBit,numBit);

    }//for j index.
  }//for i index.

  S = M;//without compare Position.

  std::cout << "imageS = "<< imgS.size() << std::endl;
  std::cout << "S.size "<< S.size() << std::endl;

}//LSB

LSB::LSB(Mat& imgCover,Mat& imgMsg,int r, bool cbp){

  std::cout << "[ LSB ] Create Object." << std::endl;

  if(cbp){
    CBP = true;
  }else{
    CBP = false;
  }//replace.


  if(r==2 || r==3 || r==4 ){
    numBit = r;
    numPos = pow(2,numBit);
    numPossiPos = Factorial(pow(2,numBit));
  }else{
    numBit = 2;
    numPos = pow(2,numBit);
    numPossiPos = Factorial(pow(2,numBit));
  }

  imgC = imgCover;
  imgM = imgMsg;

  init();


}//LSB

LSB::~LSB(){
  vector<pos>().swap(collectorBitPosition);
}

void LSB::init(){

  std::cout << "[ LSB ] Init Function." << std::endl;

  // k0 = 1724;
  // k1 = 531;
  k0 = 1019;
  k1 = 17;

  imgS = imgC.clone();

  std::cout << "- Get Bits String from Message image." << std::endl;
  //Get Bits String from Message image.
  for (size_t i = 0; i < imgM.rows; i++) {
    for (size_t j = 0; j < imgM.cols; j++) {

        string byte = bitset<8>(imgM.at<uchar>(i,j)).to_string();
        M +=byte;

    }//for j index.
  }//for i index.

  int bitMsgLength = M.size();

  std::cout << "- Get Bits String from Cover image." << std::endl;

  //Get C 2,4bits.
  for (size_t i = 0; i < imgC.rows; i++) {

    for (size_t j = 0; j < imgC.cols; j++) {

      string byte = bitset<8>(imgC.at<uchar>(i,j)).to_string();

      int currentDec = byteToDec(byte.substr(0,4));

      vector<string>bVect(8,"");

      if(
        (i!=0 && j!=0) &&
        i!=0 &&
        (i!=0 && j!=imgC.cols-1) &&
        (i!=imgC.rows-1 && j!=0) &&
        j!=0
      ){
        bVect[0] = bitset<8>(imgC.at<uchar>(i-1,j-1)).to_string();
      }
      if(
        (i!=0 && j!=0) &&
        i!=0 &&
        (i!=0 && j!=imgC.cols-1)
      ){
        bVect[1] = bitset<8>(imgC.at<uchar>(i-1,j)).to_string();
      }
      if(
        (i!=0 && j!=0) &&
        i!=0 &&
        (i!=0 && j!=imgC.cols-1) &&
        j!=imgC.cols-1 &&
        (i!=imgC.rows-1 && j!=imgC.cols-1)
      ){
        bVect[2] = bitset<8>(imgC.at<uchar>(i-1,j+1)).to_string();
      }
      if(
        (i!=0 && j!=0) &&
        (i!=imgC.rows-1 && j!=0) &&
        j!=0
      ){
        bVect[3] = bitset<8>(imgC.at<uchar>(i,j-1)).to_string();
      }
      if(
        (i!=0 && j!=imgC.cols-1) &&
        j!=imgC.cols-1 &&
        (i!=imgC.rows-1 && j!=imgC.cols-1)&&
        j!=imgC.cols-1
      ){
        bVect[4] = bitset<8>(imgC.at<uchar>(i,j+1)).to_string();
      }
      if(
        (i!=0 && j!=0) &&
        (i!=imgC.rows-1 && j!=imgC.cols-1) &&
        i!=imgC.rows-1 &&
        (i!=imgC.rows-1 && j!=0) &&
        j!=0
      ){
        bVect[5] = bitset<8>(imgC.at<uchar>(i+1,j-1)).to_string();
      }
      if(
        (i!=imgC.rows-1 && j!=imgC.cols-1) &&
        i!=imgC.rows-1 &&
        (i!=imgC.rows-1 && j!=0)
      ){
        bVect[6] = bitset<8>(imgC.at<uchar>(i+1,j)).to_string();
      }
      if(
        (i!=0 && j!=imgC.cols-1) &&
        j!=imgC.cols-1 &&
        (i!=imgC.rows-1 && j!=imgC.cols-1) &&
        i!=imgC.rows-1 &&
        (i!=imgC.rows-1 && j!=0)
      ){
        bVect[7] = bitset<8>(imgC.at<uchar>(i+1,j+1)).to_string();
      }

      int countSame = 0;

      for (size_t k = 0; k < 8; k++) {
        if (bVect[k]!="") {

          string s = bVect[k].substr(0,4);
          int d = byteToDec(s);

          for (size_t l = 0; l < 8; l++) {
            if(k==l)continue;
            string sl = bVect[l].substr(0,4);
            int dl = byteToDec(sl);
            if (currentDec == d) {
              countSame++;
            }//if.
            if(d == dl) countSame++;
          }//for l.

        }//if.

      }//for k.

      if (countSame < 4) {
        C4Bits += byte.substr(4,4);
      }else{
        C2Bits += byte.substr(6,2);
      }

    }
  }

  //Fix the size.
  std::cout << "C4Bits.size() "<< C4Bits.size() << std::endl;
  std::cout << "C2Bits.size() "<< C2Bits.size() << std::endl;

  int remod = C4Bits.size()%64;
  C4Bits.resize(C4Bits.size()-remod);


  string tmp  = M.substr(C4Bits.size(),M.size()-C4Bits.size());
  C2Bits.resize(tmp.size());


  std::cout << "M4Bits "<< M4Bits.size() << std::endl;
  std::cout << "M2Bits "<< M2Bits.size() << std::endl;

  std::cout << "C4Bits.size() "<< C4Bits.size() << std::endl;
  std::cout << "C2Bits.size() "<< C2Bits.size() << std::endl;
  std::cout << "M.size() "<< M.size() << std::endl;

  //S = M;//without compare Position.
  std::cout << "==============================" << std::endl;
  std::cout << "M4Bits - "<< M4Bits.substr(0,8) << std::endl;
  std::cout << "M2Bits - "<< M2Bits.substr(0,8) << std::endl;
  std::cout << "==============================" << std::endl;

}//init function.

void LSB::ToralAutomorphisms(){

    int s = M.size()/numBit;

    vector<string> tmpM(s);
    int c = 0;
    for (size_t i = 0; i < M.size(); i+=numBit) {

      int x = (k0+k1*c)%s;

      tmpM[x] = M.substr(i,numBit);
      c++;
    }
    M = "";
    for (size_t i = 0; i < s; i++) {

      M += tmpM[i];

    }

}//End of ToralAutomorphisms.

void LSB::comparePosition(){

  std::cout << "[ LSB ] Compare Pixel Position." << std::endl;

  M4Bits = M.substr(0,C4Bits.size());
  M2Bits = M.substr(C4Bits.size(),M.size()-C4Bits.size());
  std::cout << "++++++++++++++++++++++++++++" << std::endl;
  std::cout << "M.size "<< M.size() << std::endl;
  std::cout << "M4 + M2 "<< M4Bits.size()+M2Bits.size() << std::endl;
  std::cout << "++++++++++++++++++++++++++++" << std::endl;

  //Checking for Change Bit's Position or not.
  if(!CBP){

    ACO pixelAnt4Bits(C4Bits,M4Bits,numBit,false);

    vector<int> bestPixel4Bits = pixelAnt4Bits.bestTour;
    for (size_t i = 0; i < numPos; i++) {
      bestPosPixel4Bits.push_back(bestPixel4Bits[i]);
    }
    bestPixel4BitsMSE = pixelAnt4Bits.bestTourLength;

    ACO pixelAnt2Bits(C2Bits,M2Bits,2,false);

    vector<int> bestPixel2Bits = pixelAnt2Bits.bestTour;
    for (size_t i = 0; i < 4; i++) {
      bestPosPixel2Bits.push_back(bestPixel2Bits[i]);
    }
    bestPixel2BitsMSE = pixelAnt2Bits.bestTourLength;

  }else if(CBP){
    //FOR 4 BITS.
    //Swap Pixel position.
    ACO pixelAnt4Bits(C4Bits,M4Bits,numBit,false);

    vector<int> bestPixel4Bits = pixelAnt4Bits.bestTour;
    for (size_t i = 0; i < numPos; i++) {
      bestPosPixel4Bits.push_back(bestPixel4Bits[i]);
    }

    //change position.
    string tmpM4Bits = "";
    //Swap Pixel Position.4Bits
    for (size_t j = 0; j < M4Bits.size(); j+= numBit*numPos) {

      int ptrTmpMp = 0;
      vector<string> vTmpMp(numPos);//

      for (size_t k = 0; k < numBit*numPos; k+=numBit) {
        // if(!tmpM[j+k])break;
        vTmpMp[ptrTmpMp] = M4Bits.substr(j+k,numBit);
        ptrTmpMp++;
      }//for k index.

      for (size_t k = 0; k < numPos; k++) {

        tmpM4Bits += vTmpMp[bestPosPixel4Bits[k]];

      }

    }//for j index.

    M4Bits = tmpM4Bits;

    //Swap Bit position.
    ACO bitAnt4Bits(C4Bits,M4Bits,numBit,true);
    vector<int> bestBit4Bits = bitAnt4Bits.bestTour;
    for (size_t i = 0; i < numPos; i++) {
      bestPosBit4Bits.push_back(bestBit4Bits[i]);
    }

    //FOR 2 BITS.
    //Swap Pixel position.
    ACO pixelAnt2Bits(C2Bits,M2Bits,2,false);

    vector<int> bestPixel2Bits = pixelAnt2Bits.bestTour;
    for (size_t i = 0; i < 4; i++) {
      bestPosPixel2Bits.push_back(bestPixel2Bits[i]);
    }

    string tmpM2Bits = "";
    //Swap Pixel Position.2Bits
    for (size_t j = 0; j < M2Bits.size(); j+= 2*4) {

      int ptrTmpMp = 0;
      vector<string> vTmpMp(4);//

      for (size_t k = 0; k < 2*4; k+=2) {
        // if(!tmpM[j+k])break;
        vTmpMp[ptrTmpMp] = M2Bits.substr(j+k,2);
        ptrTmpMp++;
      }//for k index.

      for (size_t k = 0; k < 4; k++) {

        tmpM2Bits += vTmpMp[bestPosPixel2Bits[k]];

      }

    }//for j index.
    M2Bits = tmpM2Bits;
    ACO bitAnt2Bits(C2Bits,M2Bits,2,true);
    vector<int> bestBit2Bits = bitAnt2Bits.bestTour;
    for (size_t i = 0; i < 4; i++) {
      bestPosBit2Bits.push_back(bestBit2Bits[i]);
    }

  }//if change Bit positoon.

}//compare.

void LSB::comparePixelPosition(){

  M4Bits = M.substr(0,C4Bits.size());
  M2Bits = M.substr(C4Bits.size(),M.size()-C4Bits.size());

  ACO pixelAnt4Bits(C4Bits,M4Bits,numBit,false);

  vector<int> bestPixel4Bits = pixelAnt4Bits.bestTour;
  for (size_t i = 0; i < numPos; i++) {
    bestPosPixel4Bits.push_back(bestPixel4Bits[i]);
  }

  bestPixel4BitsMSE = pixelAnt4Bits.bestTourLength;

  ACO pixelAnt2Bits(C2Bits,M2Bits,2,false);

  vector<int> bestPixel2Bits = pixelAnt2Bits.bestTour;
  for (size_t i = 0; i < 4; i++) {
    bestPosPixel2Bits.push_back(bestPixel2Bits[i]);
  }

  bestPixel2BitsMSE = pixelAnt2Bits.bestTourLength;

}//comparePixel.

void LSB::compareBitPosition(){

  if(CBP){

    //change position.
    string tmpM4Bits = "";
    //Swap Pixel Position.4Bits
    for (size_t j = 0; j < M4Bits.size(); j+= numBit*numPos) {

      int ptrTmpMp = 0;
      vector<string> vTmpMp(numPos);

      for (size_t k = 0; k < numBit*numPos; k+=numBit) {

        vTmpMp[ptrTmpMp] = M4Bits.substr(j+k,numBit);
        ptrTmpMp++;

      }//for k index.

      for (size_t k = 0; k < numPos; k++) {

        tmpM4Bits += vTmpMp[bestPosPixel4Bits[k]];

      }

    }//for j index.


    string tmpM2Bits = "";
    //Swap Pixel Position.2Bits
    for (size_t j = 0; j < M2Bits.size(); j+= 2*4) {

      int ptrTmpMp = 0;
      vector<string> vTmpMp(4);//

      for (size_t k = 0; k < 2*4; k+=2) {

        vTmpMp[ptrTmpMp] = M2Bits.substr(j+k,2);
        ptrTmpMp++;

      }//for k index.

      for (size_t k = 0; k < 4; k++) {

        tmpM2Bits += vTmpMp[bestPosPixel2Bits[k]];

      }

    }//for j index.

    //ACO Compare.
    ACO bitAnt4Bits(C4Bits,tmpM4Bits,numBit,true);
    vector<int> bestBit4Bits = bitAnt4Bits.bestTour;
    for (size_t i = 0; i < numPos; i++) {
      bestPosBit4Bits.push_back(bestBit4Bits[i]);
    }

    bestBit4BitsMSE = bitAnt4Bits.bestTourLength;


    ACO bitAnt2Bits(C2Bits,tmpM2Bits,2,true);
    vector<int> bestBit2Bits = bitAnt2Bits.bestTour;
    for (size_t i = 0; i < 4; i++) {
      bestPosBit2Bits.push_back(bestBit2Bits[i]);
    }

    bestBit2BitsMSE = bitAnt2Bits.bestTourLength;

  }//End of CBP.

}//compareBit.

void LSB::changePosition(){
  std::cout << "[ LSB ] Change Position." << std::endl;

  if(!CBP){
    std::cout << "Change Position with this order : " ;
    for (size_t i = 0; i < numPos; i++) {
      std::cout << bestPosPixel4Bits[i]<< " ";

    }std::cout  << std::endl;
    std::cout << "Change Position with this order : " ;
    for (size_t i = 0; i < 4; i++) {
      std::cout << bestPosPixel2Bits[i]<< " ";

    }std::cout  << std::endl;

  }else{
    std::cout << "Change Position with this order : " ;
    for (size_t i = 0; i < numPos; i++) {
      std::cout << bestPosPixel4Bits[i]<< " ";

    }std::cout  << std::endl;
    std::cout << "Change Position with this order : " ;
    for (size_t i = 0; i < 4; i++) {
      std::cout << bestPosPixel2Bits[i]<< " ";

    }std::cout  << std::endl;
    std::cout << "Change Position with this order : " ;
    for (size_t i = 0; i < numPos; i++) {
      std::cout << bestPosBit4Bits[i]<< " ";

    }std::cout  << std::endl;
    std::cout << "Change Position with this order : " ;
    for (size_t i = 0; i < 4; i++) {
      std::cout << bestPosBit2Bits[i]<< " ";

    }std::cout  << std::endl;
  }

  S4Bits = "";
  S2Bits = "";
  Sp4Bits = "";
  Sp2Bits = "";
  Sb4Bits = "";
  Sb2Bits = "";

    if(!CBP){

      //Swap Pixel Position.4Bits
      for (size_t j = 0; j < M4Bits.size(); j+= numBit*numPos) {

        int ptrTmpMp = 0;
        vector<string> vTmpMp(numPos);

        for (size_t k = 0; k < numBit*numPos; k+=numBit) {

          vTmpMp[ptrTmpMp] = M4Bits.substr(j+k,numBit);
          ptrTmpMp++;

        }//for k index.

        for (size_t k = 0; k < numPos; k++) {

          Sp4Bits += vTmpMp[bestPosPixel4Bits[k]];

        }

      }//for j index.

      //Swap Pixel Position.2Bits
      for (size_t j = 0; j < M2Bits.size(); j+= 2*4) {

        int ptrTmpMp = 0;
        vector<string> vTmpMp(4);//

        for (size_t k = 0; k < 2*4; k+=2) {

          vTmpMp[ptrTmpMp] = M2Bits.substr(j+k,2);
          ptrTmpMp++;

        }//for k index.

        for (size_t k = 0; k < 4; k++) {

          Sp2Bits += vTmpMp[bestPosPixel2Bits[k]];

        }

      }//for j index.

    //Swap Bit Position.
    }else if(CBP){
      //Swap Pixel Position.4Bits
      for (size_t j = 0; j < M4Bits.size(); j+= numBit*numPos) {

        int ptrTmpMp = 0;
        vector<string> vTmpMp(numPos);//

        for (size_t k = 0; k < numBit*numPos; k+=numBit) {

          vTmpMp[ptrTmpMp] = M4Bits.substr(j+k,numBit);
          ptrTmpMp++;

        }//for k index.

        for (size_t k = 0; k < numPos; k++) {

          Sp4Bits += vTmpMp[bestPosPixel4Bits[k]];

        }

      }//for j index.

      M4Bits = Sp4Bits;

      //Swap bit Position.
      for (size_t j = 0; j < M4Bits.size(); j+= numPos) {

        int ptrTmpMb = 0;
        vector<string> vTmpMb(numPos);

        for (size_t k = 0; k < numPos; k++) {

          vTmpMb[ptrTmpMb] = M4Bits.substr(j+k,1);
          ptrTmpMb++;

        }//for k index.

        for (size_t k = 0; k < numPos; k++) {

          Sb4Bits += vTmpMb[bestPosBit4Bits[k]];

        }

      }//for j index.

      //Swap Pixel Position.2Bits
      for (size_t j = 0; j < M2Bits.size(); j+= 2*4) {

        int ptrTmpMp = 0;
        vector<string> vTmpMp(4);//

        for (size_t k = 0; k < 2*4; k+=2) {

          vTmpMp[ptrTmpMp] = M2Bits.substr(j+k,2);
          ptrTmpMp++;

        }//for k index.

        for (size_t k = 0; k < 4; k++) {

          Sp2Bits += vTmpMp[bestPosPixel2Bits[k]];

        }

      }//for j index.

      M2Bits = Sp2Bits;

      //Swap bit Position.
      for (size_t j = 0; j < M2Bits.size(); j+= 4) {

        int ptrTmpMb = 0;
        vector<string> vTmpMb(4);

        for (size_t k = 0; k < 4; k++) {

          vTmpMb[ptrTmpMb] = M2Bits.substr(j+k,1);
          ptrTmpMb++;

        }//for k index.

        for (size_t k = 0; k < 4; k++) {

          Sb2Bits += vTmpMb[bestPosBit2Bits[k]];

        }

      }//for j index.

    }//if change Bit Position.

  if(!CBP){
    S4Bits = Sp4Bits;
    S2Bits = Sp2Bits;
  }else{
     S4Bits = Sb4Bits;
     S2Bits = Sb2Bits;
   }

}//changePosition.

Mat LSB::insertion(bool flag){

  std::cout << "[ LSB ] Insertion Function." << std::endl;

  if(flag){
    int ptrS4 = 0;
    int ptrS2 = 0;

    for (size_t i = 0; i < imgS.rows; i++) {

      for (size_t j = 0; j < imgS.cols; j++) {

          string byte = bitset<8>(imgS.at<uchar>(i,j)).to_string();
          string first4Bit = byte.substr(0,4);
          int first4BitDec = byteToDec(first4Bit);

          vector<string>bVect(8,"");
          if(
            (i!=0 && j!=0) &&
            i!=0 &&
            (i!=0 && j!=imgS.cols-1) &&
            (i!=imgS.rows-1 && j!=0) &&
            j!=0
          ) bVect[0] = bitset<8>(imgS.at<uchar>(i-1,j-1)).to_string();
          if(
          (i!=0 && j!=0) &&
            i!=0 &&
            (i!=0 && j!=imgS.cols-1)
          ) bVect[1] = bitset<8>(imgS.at<uchar>(i-1,j)).to_string();
          if(
          (i!=0 && j!=0) &&
            i!=0 &&
            (i!=0 && j!=imgS.cols-1) &&
            j!=imgS.cols-1 &&
            (i!=imgS.rows-1 && j!=imgS.cols-1)
          ) bVect[2] = bitset<8>(imgS.at<uchar>(i-1,j+1)).to_string();
          if(
          (i!=0 && j!=0) &&
            (i!=imgS.rows-1 && j!=0) &&
            j!=0
          ) bVect[3] = bitset<8>(imgS.at<uchar>(i,j-1)).to_string();
          if(
          (i!=0 && j!=imgS.cols-1) &&
            j!=imgS.cols-1 &&
            (i!=imgS.rows-1 && j!=imgS.cols-1)&&
            j!=imgS.cols-1
          ) bVect[4] = bitset<8>(imgS.at<uchar>(i,j+1)).to_string();
          if(
          (i!=0 && j!=0) &&
            (i!=imgS.rows-1 && j!=imgS.cols-1) &&
            i!=imgS.rows-1 &&
            (i!=imgS.rows-1 && j!=0) &&
            j!=0
          ) bVect[5] = bitset<8>(imgS.at<uchar>(i+1,j-1)).to_string();
          if(
          (i!=imgS.rows-1 && j!=imgS.cols-1) &&
            i!=imgS.rows-1 &&
            (i!=imgS.rows-1 && j!=0)
          ) bVect[6] = bitset<8>(imgS.at<uchar>(i+1,j)).to_string();
          if(
          (i!=0 && j!=imgS.cols-1) &&
            j!=imgS.cols-1 &&
            (i!=imgS.rows-1 && j!=imgS.cols-1) &&
            i!=imgS.rows-1 &&
            (i!=imgS.rows-1 && j!=0)
          ) bVect[7] = bitset<8>(imgS.at<uchar>(i+1,j+1)).to_string();

          int countSame = 0;

          for (size_t k = 0; k < 8; k++) {

            if (bVect[k]!="") {

              string tmpBit = bVect[k].substr(0,4);
              int tmpDec = byteToDec(tmpBit);

              for (size_t l = 0; l < 8; l++) {
                if(k==l)continue;
                string tmpBitl = bVect[l].substr(0,4);
                int tmpDecl = byteToDec(tmpBitl);
                if (first4BitDec == tmpDec) {
                  countSame++;
                }
                if(tmpDec == tmpDecl) countSame++;
              }

            }

          }
          if (countSame < 4 && ptrS4 < M4Bits.size()) {

            string s = S4Bits.substr(ptrS4,numBit);

            byte.replace(8-numBit,numBit,s,0,numBit);

            imgS.at<uchar>(i,j) = byteToDec(byte);

            ptrS4+=numBit;


          }else if(ptrS2 < M2Bits.size() ){

            string s = S2Bits.substr(ptrS2,2);

            byte.replace(8-2,2,s,0,2);

            imgS.at<uchar>(i,j) = byteToDec(byte);

            ptrS2+=2;

          }

      }//for j index.

    }//for i index.

  }else{
    int ptrS = 0;
    for (size_t i = 0; i < imgS.rows; i++) {
      for (size_t j = 0; j < imgS.cols; j++) {
        if(S[ptrS]){
          string byte = bitset<8>(imgS.at<uchar>(i,j)).to_string();
          string s = S.substr(ptrS,numBit);

          byte.replace(8-numBit,numBit,s,0,numBit);

          imgS.at<uchar>(i,j) = byteToDec(byte);

          ptrS+=numBit;
        }//if.
      }//for j index.
    }//for i index.

  }
  return imgS;

}//insertion.
Mat LSB::extraction(bool flag){

  std::cout << "[ LSB ] Extraction Function." << std::endl;

  R = "";
  Rp = "";
  Rb = "";

  //Get R.
  for (size_t i = 0; i < imgS.rows; i++) {
    if(R.size() == S.size())break;
    for (size_t j = 0; j < imgS.cols; j++) {
      if(R.size() == S.size())break;
      string byte = bitset<8>(imgS.at<uchar>(i,j)).to_string();
      R += byte.substr(8-numBit,numBit);

    }
  }//for i index.

  if(flag){

    //Get C 2,4bits.
    for (size_t i = 0; i < imgS.rows; i++) {

      for (size_t j = 0; j < imgS.cols; j++) {

        string byte = bitset<8>(imgS.at<uchar>(i,j)).to_string();
        int currentDec = byteToDec(byte.substr(0,4));

        vector<string>bVect(8,"");
        if(
          (i!=0 && j!=0) &&
          i!=0 &&
          (i!=0 && j!=imgS.cols-1) &&
          (i!=imgS.rows-1 && j!=0) &&
          j!=0
        ) bVect[0] = bitset<8>(imgS.at<uchar>(i-1,j-1)).to_string();
        if(
        (i!=0 && j!=0) &&
          i!=0 &&
          (i!=0 && j!=imgS.cols-1)
        ) bVect[1] = bitset<8>(imgS.at<uchar>(i-1,j)).to_string();
        if(
        (i!=0 && j!=0) &&
          i!=0 &&
          (i!=0 && j!=imgS.cols-1) &&
          j!=imgS.cols-1 &&
          (i!=imgS.rows-1 && j!=imgS.cols-1)
        ) bVect[2] = bitset<8>(imgS.at<uchar>(i-1,j+1)).to_string();
        if(
        (i!=0 && j!=0) &&
          (i!=imgS.rows-1 && j!=0) &&
          j!=0
        ) bVect[3] = bitset<8>(imgS.at<uchar>(i,j-1)).to_string();
        if(
        (i!=0 && j!=imgS.cols-1) &&
          j!=imgS.cols-1 &&
          (i!=imgS.rows-1 && j!=imgS.cols-1)&&
          j!=imgS.cols-1
        ) bVect[4] = bitset<8>(imgS.at<uchar>(i,j+1)).to_string();
        if(
        (i!=0 && j!=0) &&
          (i!=imgS.rows-1 && j!=imgS.cols-1) &&
          i!=imgS.rows-1 &&
          (i!=imgS.rows-1 && j!=0) &&
          j!=0
        ) bVect[5] = bitset<8>(imgS.at<uchar>(i+1,j-1)).to_string();
        if(
        (i!=imgS.rows-1 && j!=imgS.cols-1) &&
          i!=imgS.rows-1 &&
          (i!=imgS.rows-1 && j!=0)
        ) bVect[6] = bitset<8>(imgS.at<uchar>(i+1,j)).to_string();
        if(
        (i!=0 && j!=imgS.cols-1) &&
          j!=imgS.cols-1 &&
          (i!=imgS.rows-1 && j!=imgS.cols-1) &&
          i!=imgS.rows-1 &&
          (i!=imgS.rows-1 && j!=0)
        ) bVect[7] = bitset<8>(imgS.at<uchar>(i+1,j+1)).to_string();

        int countSame = 0;

        for (size_t k = 0; k < 8; k++) {
          if (bVect[k]!="") {

            string s = bVect[k].substr(0,4);
            int d = byteToDec(s);

            for (size_t l = 0; l < 8; l++) {
              if(k==l)continue;
              string sl = bVect[l].substr(0,4);
              int dl = byteToDec(sl);
              if (currentDec == d) {
                countSame++;
              }//if.
              if(d == dl) countSame++;
            }//for l.

          }//if.

        }//for k.

        if (countSame < 4) {
          R4Bits += byte.substr(4,4);
        }else{
          R2Bits += byte.substr(6,2);
        }


      }
    }
    
    std::cout << "R4Bits size "<< R4Bits.size() << std::endl;
    std::cout << "R2Bits size "<< R2Bits.size() << std::endl;
    R4Bits.resize(M4Bits.size());
    R2Bits.resize(M2Bits.size());
    std::cout << "AFTER RESIZE" << std::endl;
    std::cout << "R4Bits size "<< R4Bits.size() << std::endl;
    std::cout << "R2Bits size "<< R2Bits.size() << std::endl;

    //Swap pixel and bit for retrieved image .
      string tmpR = "";//for each section.
      string tmpRb = "";
      string tmpRp = "";

      if (CBP) {

        //FOR 4 BITS.
        //Swap Bit Position.
        for (size_t j = 0; j < R4Bits.size(); j+= numPos) {

          vector<string> vTmpRb4(numPos);
          for (size_t k = 0; k < numPos; k++) {

            vTmpRb4[bestPosBit4Bits[k]] = R4Bits.substr(j+k,1);

          }//for k index.

          for (size_t k = 0; k < numPos; k++) {

            Rb4Bits += vTmpRb4[k];

          }

        }//for j index.

        //Swap Pixel Position.
        for (size_t j = 0; j < Rb4Bits.size(); j+= numBit*numPos) {

          int ptrVTmpRp = 0;
          vector<string> vTmpRp4(numPos);

          for (size_t k = 0; k < (numBit*numPos); k+=numBit) {

            vTmpRp4[bestPosPixel4Bits[ptrVTmpRp]] = Rb4Bits.substr(j+k,numBit);
            ptrVTmpRp++;

          }

          for (size_t k = 0; k < numPos; k++) {

            Rp4Bits += vTmpRp4[k];
          }

        }//for j index.


        //FOR 2BITS.

        //Swap Bit Position.
        for (size_t j = 0; j < R2Bits.size(); j+= 4) {

          vector<string> vTmpRb2(4);
          for (size_t k = 0; k < 4; k++) {

            vTmpRb2[bestPosBit2Bits[k]] = R2Bits.substr(j+k,1);

          }//for k index.

          for (size_t k = 0; k < 4; k++) {

            Rb2Bits += vTmpRb2[k];

          }

        }//for j index.

        //Swap Pixel Position.
        for (size_t j = 0; j < Rb2Bits.size(); j+= 2*4) {

          int ptrVTmpRp = 0;
          vector<string> vTmpRp2(4);

          for (size_t k = 0; k < (2*4); k+=2) {

            vTmpRp2[bestPosPixel2Bits[ptrVTmpRp]] = Rb2Bits.substr(j+k,2);
            ptrVTmpRp++;

          }

          for (size_t k = 0; k < 4; k++) {
            //if(vTmpRp[k].size()<numBit)break;
            Rp2Bits += vTmpRp2[k];
          }

        }//for j index.

        R = Rp4Bits + Rp2Bits;
        std::cout << "==============================" << std::endl;
        std::cout << "Rb4Bits - "<< Rb4Bits.substr(0,8) << std::endl;
        std::cout << "Rb2Bits - "<< Rb2Bits.substr(0,8) << std::endl;
        std::cout << "==============================" << std::endl;
        //if change Bit position.
      }else if(!CBP){

        //Swap Pixel Position.
        for (size_t j = 0; j < R4Bits.size(); j+= numBit*numPos) {

          int ptrVTmpRp = 0;
          vector<string> vTmpRp4(numPos);

          for (size_t k = 0; k < (numBit*numPos); k+=numBit) {

            vTmpRp4[bestPosPixel4Bits[ptrVTmpRp]] = R4Bits.substr(j+k,numBit);
            ptrVTmpRp++;

          }

          for (size_t k = 0; k < numPos; k++) {

            Rp4Bits += vTmpRp4[k];
          }

        }//for j index.

        //Swap Pixel Position.
        for (size_t j = 0; j < R2Bits.size(); j+= 2*4) {

          int ptrVTmpRp = 0;
          vector<string> vTmpRp2(4);

          for (size_t k = 0; k < (2*4); k+=2) {

            vTmpRp2[bestPosPixel2Bits[ptrVTmpRp]] = R2Bits.substr(j+k,2);
            ptrVTmpRp++;

          }

          for (size_t k = 0; k < 4; k++) {

            Rp2Bits += vTmpRp2[k];

          }


        }//for j index.

      R = Rp4Bits + Rp2Bits;
      std::cout << "==============================" << std::endl;
      std::cout << "Rp4Bits - "<< Rp4Bits.substr(0,8) << std::endl;
      std::cout << "Rp2Bits - "<< Rp2Bits.substr(0,8) << std::endl;
      std::cout << "==============================" << std::endl;
    }//end if else.

    //ToralAutomorphisms.
      int s = R.size()/numBit;

      vector<string> tmpR2(s);
      vector<string> tmpR1(s);

      int b = 0;
      for (size_t i = 0; i < s; i++) {

        tmpR1[i] = R.substr(b,numBit);
        b+=numBit;

      }

      int c = 0;
      for (size_t i = 0; i < s; i++) {

        int x = (k0+k1*c)%s;
        if (c<5) {
          std::cout << "******* x2 : "<< x << std::endl;
        }
        tmpR2[i] = tmpR1[x];
        c++;
      }
      R = "";
      for (size_t i = 0; i < s; i++) {

        R += tmpR2[i];

      }

      //initialize retrieved image.
      int ptrR = 0;
      imgR = imgM.clone();
      for (size_t i = 0; i < imgR.rows; i++) {
        if(!R[ptrR])break;
        for (size_t j = 0; j < imgR.cols; j++) {
          if(!R[ptrR])break;
          imgR.at<uchar>(i,j) = byteToDec(R.substr(ptrR,8));
          ptrR +=8;
        }
      }
  }else{
    //For LSB insertion without changed position.
    //initialize retrieved image.
    int ptrR = 0;
    imgR = imgM.clone();
    for (size_t i = 0; i < imgR.rows; i++) {
      if(!R[ptrR])break;
      for (size_t j = 0; j < imgR.cols; j++) {
        if(!R[ptrR])break;
        imgR.at<uchar>(i,j) = byteToDec(R.substr(ptrR,8));
        ptrR +=8;

      }//for j index.
    }//for i index.

  }//else.
  return imgR;
}//extraction.

void LSB::create2BitPosition(){
  std::cout << "- Create Bit Position Collector." << std::endl;

    for (int a = 0; a < numBit; a++) {
      for (int b = 0; b < numBit; b++) {
        if (b!=a) {

          collectorBitPosition.push_back(pos());
          collectorBitPosition.back().posBit += to_string(a)+to_string(b);

        }
      }
    }

}//create2Position.

void LSB::create3BitPosition(){
  std::cout << "- Create Bit Position Collector." << std::endl;

    for (int a = 0; a < numBit; a++) {
      for (int b = 0; b < numBit; b++) {
        if (b!=a) {
          for (int c = 0; c < numBit; c++) {
            if (c!=a && c!=b) {

                  collectorBitPosition.push_back(pos());
                  collectorBitPosition.back().posBit += to_string(a)+to_string(b)+to_string(c);

            }
          }
        }
      }
    }


}//create3Position.

void LSB::create4BitPosition(){
  std::cout << "- Create Bit Position Collector." << std::endl;

    for (int a = 0; a < numBit; a++) {
      for (int b = 0; b < numBit; b++) {
        if (b!=a) {
          for (int c = 0; c < numBit; c++) {
            if (c!=a && c!=b) {
              for (int d = 0; d < numBit; d++) {
                if (d!=a && d!=b && d!=c) {

                  collectorBitPosition.push_back(pos());
                  collectorBitPosition.back().posBit += to_string(a)+to_string(b)+to_string(c)+to_string(d);

                }
              }
            }
          }
        }
      }
    }

}//create4Position.

int LSB::Factorial(int x) {
  return (x == 1 ? x : x * Factorial(x - 1));
}

int LSB::byteToDec(string byte){
  int dec = 0;

  for (int i = byte.size()-1; i >= 0; i--) {
    if (byte[(byte.size()-1)-i]=='1') {
      dec += pow(2,i);
    }
  }
  return dec;
}
